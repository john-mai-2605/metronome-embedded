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
#include <pthread.h>  // for thread nanagement
#include "userLEDmmap.h"  // for gpio leds
#include "key_input_fu.c"  // for input 
#include <signal.h> // Defines signal-handling functions (i.e. trap Ctrl-C)


// GLobal termios structs
static struct termios old_tio;
static struct termios new_tio;

int main(void)
{
    pthread_t tid;
    int err = pthread_create(&tid, NULL, &gpio_led, NULL);
    if (err != 0) printf("Error in thread creation!");
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

    // Test loop
    // int i = 0;
    char waitchar[] = "|/-\\";
    while (keepgoing) {
        while (!key_hit()) {
            // i = ++i % 4;
            // printf("%c", waitchar[i]);
            fflush(stdout);
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
                break;     
            default:
                ;
        }  
        if (c == 'z' || c == 'b' || c =='c'||c == 'm') {
            printf("Key %c:, Time Signature %s, Tempo %d, Run %d\n", c, timeSigs[TimeSig], Tempo, Run);
            i = 0;
        }
        // printf("%c", c);
        // //printf("%c %2x ", c, c);
        // fflush(stdout);
    }
    printf(" Quit!\n");
    fflush(stdout);
    pthread_join(tid, NULL);
    // Reset termios
    reset_termios();
    return 0;
}
