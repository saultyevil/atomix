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

/* ************************************************************************** */
/**
 * @brief      Displays the home menu containing all of the main programs.
 *
 * @param[in]  int current_index   The index referring to the previously chosen
 *                                  menu entry
 * @return     int choice          An integer referring to the chosen menu item
 *
 * @details
 *
 * This function is the main menu in atomix. It essentially, as well as
 * process_main_menu_choices(), controls where the user will go next.
 *
 * ************************************************************************** */

char *MAIN_MENU_CHOICES[] = {
  "Bound-Bound Lines",
  "Photoionization Cross Sections",
  "Switch Atomic Data",
  "Exit",
  NULL
};

enum MAIN_MENU_CHOICES_ENUM
{
  BOUND_BOUND,
  PHOTOIONIZATION,
  SWITCH_ATOMIC_DATA,
  EXIT
};

int
main_menu (int current_index)
{
  int choice;

  choice = create_menu ("Main Menu", MAIN_MENU_CHOICES, ARRAY_SIZE (MAIN_MENU_CHOICES), current_index);

  return choice;
}

/* ************************************************************************** */
/**
 * @brief      Process the menu input choices.
 *
 * @param[in]  choice  The index for the menu choice
 *
 * @details
 *
 * This function is used to take the user to other sections of atomix.
 *
 * ************************************************************************** */

void
process_main_menu_choices (int choice)
{
  switch (choice)
  {
    case BOUND_BOUND:
      bound_bound_main_menu ();
      break;
    case PHOTOIONIZATION:
      photoionization_home_menu ();
      break;
    case SWITCH_ATOMIC_DATA:
      query_atomic_data ();
      break;
    case EXIT:
      clean_ncurses_screen ();
      exit (0);
    default:
      break;
  }
}

/* ************************************************************************* */
/**
 * @brief   The main function of the program.
 *
 * @param[in]   argc    The number of arguments provided.
 * @param[in]   argv    The arguments provided.
 *
 * @return      EXIT_SUCCESS
 *
 * @details
 *
 * ************************************************************************** */

int
main (int argc, char *argv[])
{
  int main_menu_choice;
  int atomic_provided;

  /*
   * Initialise the log file, this should put AT LEAST the atomic data
   * diagnostics into the logfile
   */

  Log_init ("atomix.out.txt");
  atomic_provided = check_command_line (argc, argv);

  /*
   * Initialise ncurses, and draw the window border
   */

  init_ncurses_screen ();
  write_banner_stdscr ();

  /*
   * Query the user for the atomic data file name, and read in that atomic
   * data. If there is an error with reading the atomic data, then the error
   * is handle in this function
   */

  if (!atomic_provided)
    query_atomic_data ();

  /*
   * Loops over the main menu until it's time to quit :^)
   */

  main_menu_choice = MENU_QUIT;
  while (TRUE)
  {
    main_menu_choice = main_menu (main_menu_choice);
    if (main_menu_choice == MENU_QUIT)
      break;
    process_main_menu_choices (main_menu_choice);
  }

  clean_ncurses_screen ();

  return EXIT_SUCCESS;
}
