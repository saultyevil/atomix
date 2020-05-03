/** ************************************************************************* */
/**
 * @file     main.c
 * @author   Edward Parkinson
 * @date     September 2019
 *
 * @brief    The main functions related to running this program.
 *
 * ************************************************************************** */

#include <string.h>
#include <stdlib.h>

#include "atomix.h"

const
MenuItem_t MAIN_MENU_CHOICES[] = {
  {&elements_main_menu   , 0        , "Elements"          , "Query the elements in the atomic data"},
  {&ions_main_menu       , 1        , "Ions"              , "Query the ions in the atomic data"},
  {&bound_bound_main_menu, 2        , "Bound-Bound"       , "Query possible bound-bound transitions"},
  {&bound_free_main_menu , 3        , "Bound-Free"        , "Query the photionization edges"},
  {&levels_main_menu     , 4        , "Levels"            , "Query an atomic configuration"},
  {&switch_atomic_data   , 5        , "Switch Atomic Data", "Switch atomic data data sets"},
  {&menu_exit_atomix     , MENU_QUIT, "Exit"              , "Exit Atomix"},
};

/* ************************************************************************* */
/**
 * @brief  The main function of the program.
 *
 * @param[in] argc  The number of arguments provided.
 * @param[in] argv  The arguments provided.
 *
 * @return    EXIT_SUCCESS
 *
 * @details
 * 
 * The way atomic data is read in and the diagnostics printed at the beginning 
 * of the program is a bit spaghetti-ish.
 * 
 * ************************************************************************** */

int
main (int argc, char *argv[])
{
  int print_atomic;
  int menu_index = 0;

  atexit (cleanup_ncurses_stdscr);

  if (getenv ("PYTHON") == NULL)
    exit_atomix (EXIT_FAILURE, "main : unable to find the required $PYTHON environment variable");

  DISPLAY_BUFFER.nlines = 0;
  DISPLAY_BUFFER.lines = NULL;

  ATOMIC_BUFFER.nlines = 0;
  ATOMIC_BUFFER.lines = NULL;

  /*
   * Initialise the log file, this should put AT LEAST the atomic data
   * diagnostics into the logfile
   */

  logfile_init("atomix.log.txt");
  print_atomic = check_command_line (argc, argv);

  if (!print_atomic)
    menu_index = ARRAY_SIZE (MAIN_MENU_CHOICES) - 2;  // Set the menu index to atomic data

  /*
   * Initialise ncurses, the window panels and draw the window borders
   */

  initialise_ncurses_stdscr ();
  initialise_main_windows ();
  draw_window_boundaries ();

  /*
   * Query the user for the atomic data file name, and read in that atomic
   * data. If there is an error with reading the atomic data, then the error
   * is handle in this function. The update_menu_window() call here is used
   * to just display the menu.
   */

  main_menu ("Main Menu", MAIN_MENU_CHOICES, ARRAY_SIZE (MAIN_MENU_CHOICES), menu_index, MENU_DRAW);

  if (print_atomic)
  {
    atomic_summary_show (SCROLL_DISBALE);
  }
  else
  {
    switch_atomic_data ();
  }

  /*
   * Loops over the main menu until it's time to quit :^)
   */

  while (TRUE)
  {
    menu_index = main_menu ("Main Menu", MAIN_MENU_CHOICES, ARRAY_SIZE (MAIN_MENU_CHOICES), menu_index, MENU_CONTROL);
    atomic_summary_show (SCROLL_DISBALE);

    if (menu_index == MENU_QUIT || MAIN_MENU_CHOICES[menu_index].index == MENU_QUIT)  // Safety really
      break;
  }

  return EXIT_SUCCESS;  // Don't need to clean up at exit due to atexit()
}
