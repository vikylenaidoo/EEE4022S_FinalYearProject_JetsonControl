.RECIPEPREFIX +=
CC = gcc
CFLAGS = -Wall -pthread -lrt

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/tasks/gpio.c -o obj/gpio
    $(CC) $(CFLAGS) -c src/test.c -o obj/test
    $(CC) $(CFLAGS) -c src/main.c -o obj/main
    $(CC) $(CFLAGS) obj/gpio obj/test obj/main -o bin/main

test:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/tasks/gpio.c -o obj/gpio
    $(CC) $(CFLAGS) -c src/test.c -o obj/test
    $(CC) $(CFLAGS) obj/gpio obj/test -o bin/test

run:
    sudo ./bin/main

runtest:
    sudo ./bin/test

clean:
    rm $(PROG) $(OBJS)