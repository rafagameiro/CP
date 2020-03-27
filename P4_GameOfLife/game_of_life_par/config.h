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

#ifndef __CONFIG_H
#define __CONFIG_H

/**
 * Set of structures and functions to parse and manage the
 * program's configuration options.
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define CLI_ARGC 3
#define CLI_MAX_ARGC 6
#define TIME_MAX_VAL 1000000000

/**
 * Structure to hold the game options.
 */
typedef struct {
  struct timespec time; /**< Time that the program will sleep between iterations. */
  size_t quiet; /**< Flag to define if only the last iteration is printed.  */
  size_t generations; /**< Number of generations for which to run the game. */
  FILE *input_file;   /**< The file with the seed board. */
} GameConfig;

/**
 * Frees memory allocated for a GameConfig structure.
 *
 * @param config Pointer to a GameConfig structure.
 */
void game_config_free(GameConfig *config);

/**
 * Returns the number of generations for which to run the game.
 *
 * @param config Pointer to a GameConfig structure.
 *
 * @return The number of generations for which to run.
 */
size_t game_config_get_generations(GameConfig *config);

/**
 * Parses the command line and create a new GameConfig from it.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 *
 * @return A new GameConfig pointer.
 */
GameConfig *game_config_new_from_cli(int argc, char *argv[]);

#endif
