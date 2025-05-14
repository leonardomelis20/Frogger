#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strutture.h"

void log_coordinates(int pid, int frog_x, int crocodile_x, int direzione);

#endif