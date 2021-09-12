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
#include "Metronome_led.h"

// Global variables

volatile int keepgoing = 1; // Set to 0 when Ctrl-c is pressed
int p[6] = {7, 1, 1, 3, 1, 1};

// Callback called when SIGINT is sent to the process (Ctrl-C)
void signal_handler(int sig) {
    printf( "\nCtrl-C pressed, cleaning up and exiting...\n" );
    keepgoing = 0;
}

int main(int argc, char *argv[]) {

    volatile void *gpio_addr;
    volatile unsigned int *gpio_datain;
    volatile unsigned int *gpio_setdataout_addr;
    volatile unsigned int *gpio_cleardataout_addr;
    
    // Set the signal callback for Ctrl-C
    signal(SIGINT, signal_handler);
    float time_signature = 6/8;
    float tempo = 60;
    float duty = 0.5;
    float period = tempo/60*1000000;

    int fd = open("/dev/mem", O_RDWR);
    
    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR, GPIO1_END_ADDR, GPIO1_SIZE);
    gpio_addr = mmap(0, GPIO1_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO1_START_ADDR);
    gpio_datain = gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;
    
    if(gpio_addr == MAP_FAILED) {
        printf("Unable to map GPIO\n");
        exit(1);
    }
    printf("GPIO mapped to %p\n", gpio_addr);
    printf("GPIO SETDATAOUTADDR mapped to %p\n", gpio_setdataout_addr);
    printf("GPIO CLEARDATAOUT mapped to %p\n", gpio_cleardataout_addr);
    
    int GPIO_LEDS[] = {GPIO_LED_1, GPIO_LED_2, GPIO_LED_3, GPIO_LED_4};

    while(keepgoing) {
        for (int i = 0; i < 6; i++){
            for (int j = 0; j < 4; j++) {
                if ((p[i] & (1 << j)) >> j)
                    *gpio_setdataout_addr = GPIO_LEDS[j]; //LED on
            }
            usleep(period*duty);

            for (int j = 0; j <4; j++){
                *gpio_cleardataout_addr = GPIO_LEDS[j]; //LED off
            }
            usleep(period*(1-duty));
        }
    }
    
    munmap((void *)gpio_addr, GPIO1_SIZE);
    close(fd);
    printf("Exit \n");    
    return 0;
}