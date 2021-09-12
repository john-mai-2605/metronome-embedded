/*
Program test_single_key.c
Get a key input without hitting Enter key
using termios
Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-
linux
Global old_termios and new_termios for efficient key inputs.
*/
#include "userLEDmmap.h"  // for gpio leds

#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()
#include <sys/select.h>
#include "key_input_fu.c"
#include <signal.h> // Defines signal-handling functions (i.e. trap Ctrl-C)

#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "server_routine_gpio.c"

#define NSEC_PER_SEC 1000000000L

#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
((float)((a)->tv_nsec - (b)->tv_nsec))/NSEC_PER_SEC)


static struct timespec prev = {.tv_sec=0,.tv_nsec=0};
static int count = 5;
time_t start = 1;

void handler( int signo )
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    gpio_play();
    prev = now;
    count --;
}

int main(int argc, char *argv[])
{
    // Set defaukt value
    TimeSig = 1;
    Tempo = 90;
    Run = 0;
    char* timeSigs[4] = {"2/4", "3/4", "4/4", "6/8"};
    
    int i = 0;
    timer_t t_id;
    long period = NSEC_PER_SEC/Tempo*30;
    struct itimerspec tim_spec = {.it_interval= {.tv_sec=period/NSEC_PER_SEC,.tv_nsec=period%NSEC_PER_SEC},
    .it_value = {.tv_sec=0,.tv_nsec=start}};
    const struct itimerspec z_tim_spec = {.it_interval= {.tv_sec=0,.tv_nsec=0},
    .it_value = {.tv_sec=0,.tv_nsec=0}};
    struct sigaction act;
    sigset_t set;

    sigemptyset( &set );
    sigaddset( &set, SIGALRM );

    act.sa_flags = 0;
    act.sa_mask = set;
    act.sa_handler = &handler;

    sigaction( SIGALRM, &act, NULL );

    if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
    perror("timer_create");

    if (timer_settime(t_id, 0, &tim_spec, NULL))
    perror("timer_settime");

    clock_gettime(CLOCK_MONOTONIC, &prev);
    gpio_map();
    // Test loop
    int sockfd = init_server();
    int new_fd;
	char message[MAXDATASIZE];
	while(keepgoing) {  // main accept() loop
        new_fd = server_connect(sockfd);
        if (new_fd < 0) continue;
		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			// pthread_t tid;
			// int err = pthread_create(&tid, NULL, send_loop, &new_fd);
			// if (err != 0) printf("Error in thread creation!");
            if (timer_create(CLOCK_MONOTONIC, NULL, &t_id))
            perror("timer_create");

			while (keepgoing) {
                if (server_do_main(new_fd, message)) continue;
                if (*message == 'Q') {
                    keepgoing = 0;
                    continue;
                }
                else if (*message == 'S') {
                    Run = 0;
                }
                else if (*message == 'T') {
                    sscanf(message, "Time Signature %d, Tempo %d, Run %d", &TimeSig, &Tempo, &Run);
                }      
                printf("Time Signature %s, Tempo %d, Run %d\n", timeSigs[TimeSig], Tempo, Run);              
                ind = 0;
                if (!Run) timer_settime(t_id, 0, &z_tim_spec, NULL);
                if (Run) {
                    period = 30000000000.0/Tempo;
                    tim_spec.it_interval.tv_nsec=period%NSEC_PER_SEC;
                    tim_spec.it_interval.tv_sec=period/NSEC_PER_SEC;
                    timer_settime(t_id, 0, &tim_spec, NULL);
                clock_gettime(CLOCK_MONOTONIC, &prev);
                }
    		}
			// usleep(SEC_TO_USEC/DEFAUlT_FREQ);
			// pthread_join(tid, NULL);	
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}
    gpio_unmap();
    printf(" Quit!\n");
    fflush(stdout);
    // Reset termios
    return 0;
}
