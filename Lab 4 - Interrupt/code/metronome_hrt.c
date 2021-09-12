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

#define NSEC_PER_SEC 1000000000L

#define timerdiff(a,b) ((float)((a)->tv_sec - (b)->tv_sec) + \
((float)((a)->tv_nsec - (b)->tv_nsec))/NSEC_PER_SEC)


static struct timespec prev = {.tv_sec=0,.tv_nsec=0};
static int count = 5;
time_t start = 1;
// int interval = 2;


// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

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
    char c;
    int echo;
    // Init termios: Disable buffered IO with arg 'echo'
    echo = 0;
    // Disable echo
    init_termios(echo);
    
    // Inform menu
    printf("Menu for Metronome TUI:\n");
    printf("z: \t Time signature\t 2/4 > 3/4 > 4/4 > 6/8 > 2/4 ...\n");
    printf("c: \t Dec tempo \t Dec tempo by 5\n");
    printf("b: \t Inc tempo \t Inc tempo by 5\n");
    printf("m: \t Start/Stop \t Toggles start and stop\n");
    printf("q: \t Quit this program\n");
    // Set defaukt value
    TimeSig = 1;
    Tempo = 90;
    Run = 0;
    char* timeSigs[4] = {"2/4", "3/4", "4/4", "6/8"};
    printf("Default values: Time Signature %s, Tempo %d, Run %d\n", timeSigs[TimeSig], Tempo, Run);
    
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
    while (keepgoing) {
        // if (count == 0) break;
        while (!key_hit()) {
            // i = ++i % 4;
            // printf("%c", waitchar[i]);
            // fflush(stdout);
            // usleep(1);
        }
        c = getch();
        if (c == 'q') {
            keepgoing = 0;
            break;
        }
        switch (c) {
            case 'z':
                TimeSig = (TimeSig + 1)%4;
                break;
            case 'c':
                Tempo = Tempo - 5;
                if (Tempo < 30) Tempo = 30;
                break;
            case 'b':
                Tempo = Tempo + 5;
                if (Tempo > 200) Tempo = 200;
                break;
            case 'm':
                Run = 1 - Run;
                // if (!Run) timer_delete(t_id);
                if (!Run) timer_settime(t_id, 0, &z_tim_spec, NULL);
                break;       
            default:
                ;
        }  

        if (c == 'z' || c == 'b' || c =='c'||c == 'm') {
            printf("Key %c:, Time Signature %s, Tempo %d, Run %d\n", c, timeSigs[TimeSig], Tempo, Run);
            ind = 0;
            if (Run) {
                period = 30000000000.0/Tempo;
                tim_spec.it_interval.tv_nsec=period%NSEC_PER_SEC;
                tim_spec.it_interval.tv_sec=period/NSEC_PER_SEC;
                timer_settime(t_id, 0, &tim_spec, NULL);
            }
        }
        // printf("%c", c);
        // //printf("%c %2x ", c, c);
        // fflush(stdout);
    }
    gpio_unmap();
    printf(" Quit!\n");
    fflush(stdout);
    // Reset termios
    reset_termios();
    return 0;
}
