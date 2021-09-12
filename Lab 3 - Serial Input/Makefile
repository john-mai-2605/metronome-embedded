# Embedded Systems Lab1 b_makegcc Makefile
# Source file: taylor_ce.c, taylor_re.c, taylor_ok.c
# Do everything
all: met algo key keynb sim

# FINAL RESULT (simply type 'make met')
met:
	arm-linux-gnueabihf-gcc -o metronome_tui_thread metronome_tui_thread.c gpio_led_fu.c -pthread

# Experiment steps
algo:
	gcc -o algo_metronome_tui algo_metronome_tui.c gpio_led_fu.c -pthread
key:
	gcc -o test_single_key test_single_key.c
keynb:
	gcc -o test_single_key_nb test_single_key_nb.c
sim:
	gcc -o metronome_tui_thread_sim metronome_tui_thread_sim.c -pthread

# Clean up
clean:
	rm -f pushLEDmmap userLEDmmap
