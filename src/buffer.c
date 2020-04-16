/** ************************************************************************* */
/**
 * @file     buffer.c
 * @author   Edward Parkinson
 * @date     April 2020
 *
 * @brief
 *
 * Functions for adding and display various text buffers.
 * 
 * ************************************************************************** */

#include <stdlib.h>
#include <string.h>

#include "atomix.h"

/* ************************************************************************** */
/**
 * @brief  Clean up a text buffer, generally once it has been printed.
 *
 * @details
 * 
 * Free the memory for all of the lines in the DISPLAY buffer, before freeing
 * the memory for DISPLAY.lines and re-initialising DISPLAY.
 *
 * ************************************************************************** */

void
clean_up_display_buffer (void)
{
  int i;
  for (i = 0; i < DISPLAY.nlines; ++i)
    free (DISPLAY.lines[i].chars);
  free (DISPLAY.lines);
  DISPLAY.nlines = 0;
	DISPLAY.lines = NULL;
}

/* ************************************************************************** */
/**
 * @brief  Print the DISPLAY buffer in the provided window.
 *
 * @param[in]  win      The window to print the buffer in
 * @param[in]  start_y  The starting row to print from
 * @param[in]  start_x  The column to print from
 *
 * @details
 *
 * Simply iterates through all of the lines and prints them one by one before
 * refreshing the screen. If DISPLAY.nlines is zero, or if the lines pointer in
 * DISPLAY is NULL, an error message is displayed instead.
 * 
 * TODO: center the output
 * TODO: loop condition should be more generic rather than LINES - 2
 *
 * ************************************************************************** */

void
display_text_buffer (WINDOW *win, int start_y, int start_x)
{
  int i;

  wclear (win);

  if (DISPLAY.nlines == 0 || DISPLAY.lines == NULL)
  {
    bold_message (win, 1, 1, "No text buffer to display!");
    return;
  }

  for (i = 0; i < DISPLAY.nlines && i + start_y < LINES - 2; ++i)
    mvwprintw (win, i + start_y, start_x, "%s", DISPLAY.lines[i].chars);

  wrefresh (win);
  clean_up_display_buffer ();
}

/* ************************************************************************** */
/**
 * @brief  Add a fmt string to the DISPLAY buffer.
 *
 * @param[in]  fmt  The formatted string
 * @param[in]  ...  The arguments for the formatted string
 *
 * @details
 *
 * This function assumes that the string being added is a single, entire, line.
 * A new line character is required at the end, but isn't mandatory. I think 
 * this way you can slowly construct a single "display" line over multiple 
 * strings stored in DISPLAY ... though, this isn't the intended way to use this
 * function.
 * 
 * Uses a bit of a hack, vsnprintf, to figure out the storage requirement for
 * the string. The pointer for the new string is added to the DISPLAY buffer.
 * 
 * ************************************************************************** */

void
add_to_display_buffer (char *fmt, ...)
{
  int current_line;
  int len;
  va_list va, va_c;

  current_line = DISPLAY.nlines++;
  DISPLAY.lines = realloc (DISPLAY.lines, DISPLAY.nlines * sizeof (Line_t));

  if (!DISPLAY.lines)
  {
    cleanup_ncurses_stdscr ();
    printf ("BIG ERROR: Unable to add additional line to the display buffer :-(\n");
    exit (1);
  }

  DISPLAY.lines[current_line].len = 0;
  DISPLAY.lines[current_line].chars = NULL;

  /*
   * May be playing it extra safe here, but make a copy of va as va is not
   * necessarily preserved after vsnprintf.
   */

  va_start (va, fmt);
  va_copy (va_c, va);

  len = vsnprintf (NULL, 0, fmt, va);  // Hack: write 0 to NULL to determine length :-)
  DISPLAY.lines[current_line].chars = malloc (len * sizeof (char));
  len = vsprintf (DISPLAY.lines[current_line].chars, fmt, va_c);  // vsprintf NULL terminates the string
  DISPLAY.lines[current_line].len = len;
  
  va_end (va);
  va_end (va_c);
}

/* ************************************************************************* */
/**
 * @brief  Add a line of dashes to the DISPLAY buffer.
 *
 * @param[in]  len  The number of dashes to draw.
 *
 * @details
 *
 * memcpy is used instead of strcpy. There is no real reason for this, as strcpy
 * SHOULD copy the NULL terminator as well... but it felt safer to use memcpy.
 * 
 * ************************************************************************** */

void
append_separator_to_buffer (const int len)
{
  int i;
  char tmp[len + 2];

  for (i = 0; i < len; ++i)
    memcpy (&tmp[i], "-", 1);
  memcpy (&tmp[len], "\n\0", 2);

  add_to_display_buffer (tmp);
}
