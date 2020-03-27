/*
 * Copyright (C) 2009 Raphael Kubo da Costa <kubito@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include "mem.h"

/* Error messages */
static const char *const usage_message =
    "\n"
    "Conway's Game of Life\n"
    "Raphael Kubo da Costa, RA 072201\n"
    "\n"
    "Usage: glife [-q] [-p time] GENERATIONS INPUT_FILE\n"
    "\n"
    "  [-q] is a flag that only prints the last board if used\n"
    "  [-p time] amount of time the program sleep between generations\n"
    "  GENERATIONS is the number of generations the game should run\n"
    "  INPUT_FILE  is a file containing an initial board state\n" "\n";

void game_config_free(GameConfig *config)
{
  if (config) {
    fclose(config->input_file);
    free(config);
  }
}

void set_sleep_time(struct timespec *time, long spause)
{
  long val = spause * 1000000;

  if(val >= TIME_MAX_VAL) {
    time->tv_sec = val / TIME_MAX_VAL;
    time->tv_nsec = val % TIME_MAX_VAL;
  }
  else {
    time->tv_sec = 0;
    time->tv_nsec = val;
  }
    
}

size_t game_config_get_generations(GameConfig *config)
{
  assert(config);

  return config->generations;
}

GameConfig *game_config_new_from_cli(int argc, char *argv[])
{
  char *endptr;
  FILE *file;
  GameConfig *config;
  long generations;
  long spause = 0;
  size_t qflag = 1;
   
  if (argc < CLI_ARGC || argc > CLI_MAX_ARGC) {
    fprintf(stderr, usage_message);
    return NULL;
  }

  int opt;
  while((opt = getopt(argc, argv, "p:q")) != -1)
  {
    switch(opt)
    {
        case 'p':
            if(optarg == argv[argc-2] || strcmp(optarg, "-q") == 0) {
                printf("\n[-p time] with time as the amount in milliseconds\n\n");
                return NULL;
            }
            spause = strtol(optarg, NULL, 10);
            break;
        case 'q':
            qflag = 0;
            break;
        case '?':
            printf("unknown option: %c\n", opt);
            return NULL;
            break;
    } 
  }
  
  generations = strtol(argv[argc-2], &endptr, 10);
  if ((*endptr != '\0') || (generations < 0)) {
    fprintf(stderr, "Error: GENERATIONS must be a valid positive integer\n");
    return NULL;
  }

  file = fopen(argv[argc-1], "r");
  if (!file) {
    fprintf(stderr, "Error: could not open '%s'\n", argv[2]);
    return NULL;
  }

  config = MEM_ALLOC(GameConfig);
  set_sleep_time(&config->time, spause);
  config->quiet = qflag;
  config->generations = (size_t) generations;
  config->input_file = file;

  return config;
}
