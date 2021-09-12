/*
Program test_single_key.c
Get a key input without hitting Enter key
using termios
Modified from
http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-
linux
Global old_termios and new_termios for efficient key inputs.
*/

#include <termios.h>
#include <stdio.h>
#include <unistd.h> // read()
#include <sys/select.h>
#include "key_input_fu.c"
#include <signal.h> // Defines signal-handling functions (i.e. trap Ctrl-C)

#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include "client_routine.c"

int TimeSig;
char* timeSig;
int Tempo;
int Run;
// int *p[4];
// int size[4];
// int ind;
int keepgoing = 1;

// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;


void signal_handler(int sig) {
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
    exit(0);
}



int main(int argc, char *argv[])
{
    // signal(SIGINT, signal_handler);        
    int sockfd = init_client(argc, argv);
    char c;
    int echo;
    char message[100];
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
            if (c == 'q') sprintf(message, "Quit");
    		if (send(sockfd, message, strlen(message), 0) == -1)
		    	perror("send");            
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
                break;       
            default:
                ;
        }  

        if (c == 'z' || c == 'b' || c =='c'||c == 'm') {
            printf("Key %c:, Time Signature %s, Tempo %d, Run %d\n", c, timeSigs[TimeSig], Tempo, Run);
            // ind = 0;
            if (c == 'm' && !Run) sprintf(message, "Stop");
            else sprintf(message, "Time Signature %d, Tempo %d, Run %d", TimeSig, Tempo, Run);

            // printf("The message was: %s\n", message);
    		if (send(sockfd, message, strlen(message), 0) == -1)
		    	perror("send");
        }

    }
    printf(" Quit!\n");
    fflush(stdout);
    // Reset termios
    reset_termios();
    close(sockfd);
    return 0;
}
