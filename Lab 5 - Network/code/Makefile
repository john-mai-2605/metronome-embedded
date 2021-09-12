# Embedded Systems Lab1 b_makegcc Makefile
# Source file: taylor_ce.c, taylor_re.c, taylor_ok.c
# Do everything
all: stream datagram syncDB asyncDB sim1 sim2 sim3 sim4 met

# FINAL RESULT (simply type 'make met')
met:
	gcc -o metro_client 	metro_client.c 		-pthread
	arm-linux-gnueabihf-gcc -o metro_server		metro_server.c 		gpio_led_fu.c 	-lrt 

# Experiment steps
time:
	arm-linux-gnueabihf-gcc -o a_test_hrtimer test_hrtimer.c -lrt

sim1:
	gcc -o metro_client_pc1 metro_client_1.c 

sim2:
	gcc -o metro_client_pc2 metro_client_2.c
	gcc -o metro_server_sim2	metro_server_2.c gpio_led_fu_sim.c -lrt
sim3:
	gcc -o metro_client_pc3 metro_client_3.c
	gcc -o metro_server_sim3	metro_server_3.c gpio_led_fu_sim.c -lrt 

sim4:
	gcc -o metro_client_pc4 metro_client.c -pthread
	gcc -o metro_server_sim4	metro_server.c gpio_led_fu_sim.c -lrt 

stream:
	gcc -o serverPC server.c
	gcc -o client client.c
	arm-linux-gnueabihf-gcc -o server server.c
datagram:
	gcc -o talker talker.c
	gcc -o listenerPC listener.c
	arm-linux-gnueabihf-gcc -o listener listener.c

syncDB:
	gcc -o clientBD_sync clientBD_sync.c 
	gcc -o serverBD_syncPC serverBD_sync.c
	arm-linux-gnueabihf-gcc -o serverBD_sync serverBD_sync.c

asyncDB:
	gcc -o clientBD clientBD.c -pthread
	gcc -o serverBDPC	serverBD.c -pthread
	arm-linux-gnueabihf-gcc -o serverBD serverBD.c -pthread

# Clean up
clean:
	ls | grep -v "\.\|Makefile" | xargs rm -f
