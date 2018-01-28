.RECIPEPREFIX +=
CC = gcc
CFLAGS = -Wall -pthread -lrt

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/tasks/gpio.c -o obj/gpio
    $(CC) $(CFLAGS) -c src/tasks/uart.c -o obj/uart
    $(CC) $(CFLAGS) -c src/tasks/crc32.c -o obj/crc
    $(CC) $(CFLAGS) -c src/tasks/sensor_handler.c -o obj/sensor_handler
    $(CC) $(CFLAGS) -c src/tasks/timer.c -o obj/timer
    $(CC) $(CFLAGS) -c src/threads.c -o obj/threads
    $(CC) $(CFLAGS) -c src/main.c -o obj/main
    $(CC) $(CFLAGS) obj/gpio obj/uart obj/crc obj/sensor_handler obj/timer obj/threads obj/main -o bin/main

test:
    mkdir -p bin obj
    #$(CC) $(CFLAGS) -c src/tasks/gpio.c -o obj/gpio
    $(CC) $(CFLAGS) -c src/tasks/uart.c -o obj/uart
    #$(CC) $(CFLAGS) -c src/tasks/crc32.c -o obj/crc
    #$(CC) $(CFLAGS) -c src/tasks/sensor_handler.c -o obj/sensor_handler
    $(CC) $(CFLAGS) -c src/test_uart.c -o obj/test
    $(CC) $(CFLAGS)  obj/uart obj/test -o bin/test

run:
    sudo ./bin/main

runtest:
    sudo ./bin/test

clean:
    rm $(PROG) $(OBJS)