# Embedded Systems Lab1 b_makegcc Makefile
# Source file: taylor_ce.c, taylor_re.c, taylor_ok.c
# Do everything
all: met time sim

# FINAL RESULT (simply type 'make met')
met:
	arm-linux-gnueabihf-gcc -o metronome_hrt metronome_hrt.c gpio_led_fu.c -lrt

# Experiment steps
time:
	arm-linux-gnueabihf-gcc -o a_test_hrtimer test_hrtimer.c -lrt

sim:
	gcc -o metronome_hrt_sim metronome_hrt.c gpio_led_fu_sim.c -lrt

# Clean up
clean:
	rm -f metronome_hrt metronome_hrt_sim a_test_hrtimer
