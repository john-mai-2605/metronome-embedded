// From: http://stackoverflow.com/questions/13124271/driving-beaglebone-gpio
// -through-dev-mem
// user contributions licensed under cc by-sa 3.0 with attribution required
// http://creativecommons.org/licenses/by-sa/3.0/
// http://blog.stackoverflow.com/2009/06/attribution-required/
// Author: madscientist159 (http://stackoverflow.com/users/3000377/madscientist159)
//
// Read one gpio pin and write it out to another using mmap.
// Be sure to set -O3 when compiling.
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h> // Defines signal-handling functions (i.e. trap Ctrl-C)
#include <unistd.h> // close()
#include <pthread.h>
#include "userLEDmmap.h"

// Global variables
// int TimeSig;
// int Tempo;
// int Run;
// int i = 0;

volatile int keepgoing = 1; // Set to 0 when Ctrl-c is pressed
int p1[2] = {7, 1};
int p2[3] = {7, 1, 1};
int p3[4] = {7, 1, 3, 1};
int p4[6] = {7, 1, 1, 3, 1, 1};
int *p[4] = {p1, p2, p3, p4};
int size[4] = {2, 3, 4, 6};

int GPIO_LEDS[] = {GPIO_LED_1, GPIO_LED_2, GPIO_LED_3, GPIO_LED_4};
// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
    keepgoing = 0;
}

int fd;
volatile void *gpio_addr;
volatile unsigned int *gpio_datain;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

void gpio_map(){ 

}

    // printf("GPIO mapped to %p\n", gpio_addr);
    // printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    // printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);
    
int gpio_play(){
    signal(SIGINT, signal_handler); 
        if (Run) {
            if (ind%2) {   
                int result = p[TimeSig][ind/2];            
                printf("%d", result);
                fflush(stdout);
                ind = (ind + 1)%(2*size[TimeSig]);
                return result;
            } 
            else {
                // printf(".");
                ind = (ind + 1)%(2*size[TimeSig]);
                return 0;
            }
        }
    return -1;
}

void gpio_unmap(){
  
}
   