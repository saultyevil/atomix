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
clean_up_display (Display_t *buffer)
{
  int i;

  for (i = 0; i < buffer->nlines; ++i)
    free (buffer->lines[i].chars);
  free (buffer->lines);

  buffer->nlines = 0;
  buffer->lines = NULL;
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
 * PLS PLS PLS DO NOT HAVE A \n NEWLINE CHARACTER IN *fmt
 *
 * ************************************************************************** */

void
add_display (Display_t *buffer, char *fmt, ...)
{
  int line_index;
  int len;
  va_list va, va_c;

  buffer->nlines++;
  buffer->lines = realloc (buffer->lines, buffer->nlines * sizeof (Line_t));
  line_index = buffer->nlines - 1;

   if (buffer->lines == NULL)
    exit_atomix (EXIT_FAILURE, "Unable to add additional line to the display buffer");

  buffer->lines[line_index].len = 0;
  buffer->lines[line_index].chars = NULL;

  /*
   * May be playing it extra safe here, but make a copy of va as va is not
   * necessarily preserved after vsnprintf...
   */

  va_start (va, fmt);
  va_copy (va_c, va);

  len = vsnprintf (NULL, 0, fmt, va);  // Hack: write 0 to NULL to determine length :-)
  buffer->lines[line_index].chars = malloc (len * sizeof (char) + 1);
  len = vsprintf (buffer->lines[line_index].chars, fmt, va_c);
  buffer->lines[line_index].chars[len] = '\0';  // Redundant, but just in case
  buffer->lines[line_index].len = len;

  logfile ("%s\n", buffer->lines[line_index].chars);

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
 * ************************************************************************** */

void
add_sep_display (const int len)
{
  int i;
  char tmp[len + 1];

  for (i = 0; i < len; ++i)
    tmp[i] = '-';
  tmp[len] = '\0';

  display_add (tmp);
}

/* ************************************************************************** */
/**
 * @brief  Scroll the text buffer up and down.
 *
 * @param[in]  win  The Window_t containing the buffer to be scrolled
 *
 * @details
 *
 *
 * ************************************************************************** */

void
scroll_display (Window_t win)
{
  int i;
  int ch;
  int line_start;
  int current_row;
  WINDOW *window = win.win;

  line_start = 0;

  update_status_bar ("Press q of F1 to exit text view or use UP, DOWN, PG UP or PG DN to scroll the text");

  while ((ch = wgetch (window)))
  {
    if (ch == 'q' || ch == KEY_F(1))
      break;

    if (DISPLAY_BUFFER.nlines > win.rows - 2)
    {
      wclear (window);

      switch (ch)
      {
        case KEY_UP:
          line_start--;
          break;
        case KEY_DOWN:
          line_start++;
          break;
        case KEY_NPAGE:
          line_start += win.rows - 2;
          break;
        case KEY_PPAGE:
          line_start -= win.rows - 2;
          break;
        case KEY_HOME:
          line_start = 0;
          break;
        case KEY_END:
          line_start = DISPLAY_BUFFER.nlines - win.rows + 2;
          break;
        default:
          break;
      }

      if (line_start < 0)
        line_start = 0;
      if (line_start + win.rows - 2 > DISPLAY_BUFFER.nlines)
        line_start = DISPLAY_BUFFER.nlines - win.rows + 2;

      for (i = line_start, current_row = 1; i < DISPLAY_BUFFER.nlines && current_row < win.rows - 1; ++i, ++current_row)
        mvwprintw (window, current_row, 1, "%s", DISPLAY_BUFFER.lines[i].chars);
    }

    wrefresh (window);
  }
}

/* ************************************************************************** */
/**
 * @brief  Print the DISPLAY buffer in the provided window.
 *
 * @param[in]  *buffer
 * @param[in]  scroll
 *
 * @details
 *
 * Simply iterates through all of the lines and prints them one by one before
 * refreshing the screen. If DISPLAY.nlines is zero, or if the lines pointer in
 * DISPLAY is NULL, an error message is displayed instead.
 * 
 * TODO: center the output
 *
 * ************************************************************************** */

void
display_buffer (Display_t *buffer, int scroll)
{
  int i;
  WINDOW *window = CONTENT_WINDOW.win;

   wclear (window);

  if (buffer->nlines == 0 || buffer->lines == NULL)
  {
    bold_message (CONTENT_WINDOW, 1, 1, "No text in display buffer to show.");
    wrefresh (window);
  }
  else
  {
    for (i = 0; i < buffer->nlines && i < CONTENT_WINDOW.rows - 2; ++i)
      mvwprintw (window, i + 1, 1, "%s", buffer->lines[i].chars);

    wrefresh (window);

    if (scroll == SCROLL_ENABLE)
      scroll_display (CONTENT_WINDOW);
  }
}
