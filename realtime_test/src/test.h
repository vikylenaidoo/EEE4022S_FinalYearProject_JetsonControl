#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "tasks/gpio.h"
#define test_pin 78

static FILE* test_fp;
static int isActive = 1;

int test_function();