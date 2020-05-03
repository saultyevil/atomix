/** ************************************************************************* */
/**
 * @file     query.c
 * @author   Edward Parkinson
 * @date     April 2020
 *
 * @brief
 *
 * Functions for querying information from the user.
 *
 * ************************************************************************** */

#include <form.h>
#include <string.h>
#include <stdlib.h>

#include "atomix.h"

/* ************************************************************************** */
/**
 * @brief  Clean up a form.
 *
 * @details
 *
 * Unposts the form and clears all of the memory allocated for a form.
 *
 * ************************************************************************** */

void
clean_up_form (FORM *form, FIELD **fields, int nfields)
{
  int i;

  unpost_form (form);
  free_form (form);
  for (i = 0; i < nfields; ++i)
    free_field (fields[i]);
  free (fields);
}

/* ************************************************************************** */
/**
 * @brief  Control a form given a key input
 *
 * @param[in]  form        The form which is to be controlled
 * @param[in]  ch          The key input to process
 * @param[in]  exit_index  The index of the field to exit from when enter is
 *                         pressed
 *
 * @return     input       An integer to say if form input should continue
 *                           * FORM_CONTINUE continue getting input
 *                           * FOR_EXIt      exit the form
 *
 * @details
 *
 * The purpose of this function is to control the form given a key input ch. If
 * enter is pressed on a field, then it'll go to the next field. If enter is
 * pressed on exit_input, then the form will exit.
 *
 * ************************************************************************** */

int
control_form (FORM *form, int ch, int exit_index)
{
  int current_index;
  int input = FORM_CONTINUE;

  switch (ch)
  {
    case '\t':  // This catches the tab key
    case KEY_DOWN:
      form_driver (form, REQ_NEXT_FIELD);
      form_driver (form, REQ_END_LINE);
      break;
    case KEY_UP:
      form_driver (form, REQ_PREV_FIELD);
      form_driver (form, REQ_END_LINE);
      break;
    case KEY_LEFT:
      form_driver (form, REQ_PREV_CHAR);
      break;
    case KEY_RIGHT:
      form_driver (form, REQ_NEXT_CHAR);
      break;
    case KEY_BACKSPACE:
    case 127:
      form_driver (form, REQ_DEL_PREV);
      break;
    case KEY_DC:
      form_driver (form, REQ_DEL_CHAR);
      break;
    case 10:  // This catches the enter/return key
      current_index = field_index (current_field (form));
      if (current_index != exit_index)
      {
        form_driver (form, REQ_NEXT_FIELD);
        form_driver (form, REQ_END_LINE);
      }
      else
      {
        input = FORM_BREAK;
      }
      break;
    default:
      form_driver (form, ch);
      break;
  }

  return input;
}

/* ************************************************************************** */
/**
 * @brief  Given a Query_t, create a form and get input from the user.
 *
 * @param[in]      w              The window to display the form in
 * @param[in,out]  q              The Query_t containing the form
 * @param[in]      nfields        The number of fields
 * @param[in]      title_message  A title message to prompt the user
 *
 * @return         The return from the form, either FORM_CONTINUE or FORM_QUIT
 *
 * @details
 *
 * It is assumed that the last input field is the exit field for when enter is
 * pressed.
 *
 * ************************************************************************** */

int
query_user (Window_t w, Query_t *q, int nfields, char *title_message)
{
  int i;
  int ch;
  int form_return = FORM_CONTINUE;
  FORM *form;
  WINDOW *the_win = w.win;
  FIELD **fields;

  wclear (the_win);
  keypad (the_win, TRUE);
  curs_set (1);

  bold_message (CONTENT_WINDOW, 1, 1, title_message);

  fields = calloc (nfields + 1, sizeof (FIELD *));
  if (fields == NULL)
    exit_atomix (EXIT_FAILURE, "query_user_for_input : unable to allocate memory for fields");

  fields[nfields] = NULL;

  for (i = 0; i < nfields; ++i)
  {
    fields[i] = q[i].field;
    set_field_buffer (q[i].field, q[i].buffer_number, q[i].buffer);
    if (q[i].background != FIELD_NO_BKG)
      set_field_back (q[i].field, q[i].background);
    if (q[i].opts_on != FIELD_SKIP)
      set_field_opts (q[i].field, q[i].opts_on);
    if (q[i].opts_off != FIELD_SKIP)
      field_opts_off (q[i].field, q[i].opts_off);
  }

  form = new_form (fields);
  set_form_win (form, the_win);
  set_form_sub (form, derwin (the_win, w.rows - 4, w.cols - 2, 3, 1));
  set_current_field (form, fields[1]);  // Always assume first input is default field
  update_status_bar ("Press F1 to cancel input");
  post_form (form);
  wrefresh (the_win);

  /*
   * Control the menu. Should exit when the enter key is pressed, or when
   * ch is (somehow) NULL.
   */

  while ((ch = wgetch (the_win)) && form_return == FORM_CONTINUE)
  {
    if (ch == KEY_F(1))
    {
      form_return = FORM_QUIT;
      break;
    }

    form_return = control_form (form, ch, nfields - 1);
    wrefresh (the_win);

    if (form_return == FORM_BREAK)
      break;
  }

  /*
   * We have to validate the current field to "save" the buffer - or something
   */

  form_driver (form, REQ_VALIDATION);

  /*
   * This copies the field buffer into the Query_t buffer
   */

  for (i = 0; i < nfields; ++i)
    strcpy (q[i].buffer, trim_whitespaces (field_buffer (fields[i], q[i].buffer_number)));

  clean_up_form (form, fields, nfields);
  curs_set (0);

  return form_return;
}

/* ************************************************************************** */
/**
 * @brief  Initialise a Query_t object for a form to query one input
 *         use.
 *
 * @param[out]  q       The Query_t object to initialise
 * @param[in]   label   The question to ask for the field
 * @param[in]   answer  The default answer field data to initialise
 *
 * @details
 *
 * ************************************************************************** */

void
init_single_question_form (Query_t *q, char *label, char *answer)
{
  q[0].buffer_number = 0;
  strcpy (q[0].buffer, label);
  q[0].field = new_field (1, strlen (q[0].buffer), 0, 0, 0, 0);
  q[0].opts_off = FIELD_SKIP;
  q[0].opts_on = O_VISIBLE | O_PUBLIC | O_AUTOSKIP;
  q[0].background = FIELD_NO_BKG;

  q[1].buffer_number = 0;
  strcpy (q[1].buffer, answer);
  q[1].field = new_field (1, FIELD_INPUT_LEN, 0, strlen (q[0].buffer) + 2, 0, 0);
  q[1].opts_off = O_AUTOSKIP;
  q[1].opts_on = O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE;
  q[1].background = A_REVERSE;
}

/* ************************************************************************** */
/**
 * @brief  Initialise a Query_t object for a form to query two inputs
 *
 * @param[out]  q        The Query_t object to initialise
 * @param[in]   label1   The label for field 1
 * @param[in]   label2   The label for field 2
 * @param[in]   answer1  The default answer for question 1
 * @param[in]   answer2  The default answer for question 2
 *
 * @details
 *
 * The "null field" is actually kinda redundant since a re-work of the query
 * function.
 *
 * ************************************************************************** */

void
init_two_question_form (Query_t *q, char *label1, char *label2, char *answer1, char *answer2)
{
  int label_len;

  label_len = MAX (strlen (label1), strlen (label2));

  q[0].buffer_number = 0;
  strcpy (q[0].buffer, label1);
  q[0].field = new_field (1, label_len, 0, 0, 0, 0);
  q[0].opts_off = FIELD_SKIP;
  q[0].opts_on = O_VISIBLE | O_PUBLIC | O_AUTOSKIP;
  q[0].background = FIELD_NO_BKG;

  q[1].buffer_number = 0;
  strcpy (q[1].buffer, answer1);
  q[1].field = new_field (1, FIELD_INPUT_LEN, 0, label_len + 2, 0, 0);
  q[1].opts_off = O_AUTOSKIP;
  q[1].opts_on = O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE;
  q[1].background = A_REVERSE;

  q[2].buffer_number = 0;
  strcpy (q[2].buffer, label2);
  q[2].field = new_field (1, label_len, 2, 0, 0, 0);
  q[2].opts_off = FIELD_SKIP;
  q[2].opts_on = O_VISIBLE | O_PUBLIC | O_AUTOSKIP;
  q[2].background = FIELD_NO_BKG;

  q[3].buffer_number = 0;
  strcpy (q[3].buffer, answer2);
  q[3].field = new_field (1, FIELD_INPUT_LEN, 2, label_len + 2, 0, 0);
  q[3].opts_off = O_AUTOSKIP;
  q[3].opts_on = O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE;
  q[3].background = A_REVERSE;
}

/* ************************************************************************** */
/**
 * @brief  Get the wavelength range to consider.
 *
 * @param[out]  wmin  The smallest wavelength transition to find.
 * @param[out]  wmax  The largest wavelength transition to find.
 *
 * @details
 *
 * This function will keep looping until either the user quits, or until the
 * input is correct; i.e., wmin < wmax. Echo mode is temporarily turned on for
 * this function, so the user can see what they are typing.
 *
 * ************************************************************************** */

int
query_wavelength_range (double *wmin, double *wmax)
{
  int form_return;
  int valid = FALSE;
  WINDOW *the_win = CONTENT_WINDOW.win;

  static int init_default = FALSE;
  static char string_wmin[FIELD_INPUT_LEN];
  static char string_wmax[FIELD_INPUT_LEN];
  static Query_t wavelength_query[5];

  if (init_default == FALSE)
  {
    strcpy (string_wmin, "");
    strcpy (string_wmax, "");
    init_default = TRUE;
  }

  while (valid != TRUE)
  {
    wclear (the_win); 
    init_two_question_form (wavelength_query, "Minimum Wavelength : ", "Maximum Wavelength : ", string_wmin,
                            string_wmax);
    form_return = query_user (CONTENT_WINDOW, wavelength_query, 4, "Input the wavelength range");

    if (form_return == FORM_QUIT)
      return form_return;

    strcpy (string_wmin, wavelength_query[1].buffer);
    strcpy (string_wmax, wavelength_query[3].buffer);
    *wmin = strtod (wavelength_query[1].buffer, NULL);
    *wmax = strtod (wavelength_query[3].buffer, NULL);
    if (*wmax > *wmin)
    {
      valid = TRUE;
    }
    else
    {
      update_status_bar ("Invalid input for wavelength range %f - %f (minimum - maximum)", *wmin, *wmax);
    }
  }

  return EXIT_SUCCESS;
}

/* ************************************************************************** */
/**
 * @brief
 *
 * @details
 *
 * ************************************************************************** */

int
query_atomic_number (int *z)
{
  int form_return;
  int valid = FALSE;
  WINDOW *win = CONTENT_WINDOW.win;

  static int init_query = FALSE;
  static char default_element[FIELD_INPUT_LEN];
  static Query_t element_query[2];

  if (init_query == FALSE)
  {
    strcpy (default_element, "");
    init_query = TRUE;
  }

  while (valid != TRUE)
  {
    wclear (win);
    init_single_question_form (element_query, "Atomic number : ", default_element);
    form_return = query_user (CONTENT_WINDOW, element_query, 2, "Please input the atomic number of the element");

    if (form_return == FORM_QUIT)
      return form_return;

    *z = (int) strtol (element_query[1].buffer, NULL, 10);
    if (*z > 0 && *z < 118)  // TODO: make constants in atomic.h
    {
      valid = TRUE;
      strcpy (default_element, element_query[1].buffer);
    }
    else
    {
      update_status_bar ("Invalid atomic number %i", *z);
    }
  }

  return EXIT_SUCCESS;
}

/* ************************************************************************** */
/**
 * @brief
 *
 * @details
 *
 * ************************************************************************** */

int
query_ion_input (int nion_or_z, int *z, int *istate, int *nion)
{
  int nfields;
  int form_return;
  int valid = FALSE;
  WINDOW *win = CONTENT_WINDOW.win;
  Query_t *q;

  static int init_name = FALSE;
  static char string_z[FIELD_INPUT_LEN];
  static char string_istat[FIELD_INPUT_LEN];
  static char string_nion[FIELD_INPUT_LEN];
  static Query_t nion_query[2];
  static Query_t z_istate_query[4];

  if (init_name == FALSE)
  {
    strcpy (string_z, "");
    strcpy (string_istat, "");
    strcpy (string_nion, "");
    init_name = TRUE;
  }

  while (valid != TRUE)
  {

    wclear (win);

    if (nion_or_z)
    {
      init_single_question_form (nion_query, "Ion Number : ", string_nion);
      q = nion_query;
      nfields = 2;
    }
    else
    {
      init_two_question_form (z_istate_query, "Atomic number : ", "Ionisation State : ", string_z, string_istat);
      q = z_istate_query;
      nfields = 4;
    }

    form_return = query_user (CONTENT_WINDOW, q, nfields, "Please select an ion");

    if (form_return == FORM_QUIT)
      return form_return;

    if (nion_or_z)
    {
      *nion = (int) strtol (q[1].buffer, NULL, 10);
      if (*nion >= 0 && *nion < nions)
      { 
        valid = TRUE;
      }
      else
      {
        update_status_bar ("Invalid ion number %i when there are %i ions", *nion, nions);
      }
    } 
    else
    {
      *z = (int) strtol (q[1].buffer, NULL, 10);
      *istate = (int) strtol (q[3].buffer, NULL, 10);
      if (*z > 0 && *istate > 0)
      {
        valid = TRUE;
      }
      else
      {
        update_status_bar ("Invalid atomic number %i or ionisation state %i, both should be non-zero", *z, *istate);
      }
    }
  }

  return EXIT_SUCCESS;
}

/* ************************************************************************** */
/**
 * @brief     Query the user for the atomic data name.
 *
 * @details
 *
 * Will keep asking the user for an atomic data name until the data can be read
 * in without error, or until the user quits the program.
 *
 * ************************************************************************** */

const
MenuItem_t ATOMIC_DATA_CHOICES[] = {
  {NULL, 0          , "CIIICIVCV_c10"             , ": Carbon III, IV and V Macro-atom"},
  {NULL, 1          , "CIIICIVCV_c10_CV1LVL"      , ": Carbon III, IV and V Macro-atom"},
  {NULL, 2          , "CIIICIV_c10"               , ": Carbon III and IV Macro-atom"},
  {NULL, 3          , "h10_hetop_lohe1_standard80", ": 10 Level H and He Macro-atom"},
  {NULL, 4          , "h10_hetop_standard80"      , ": 10 Level H and He Macro-atom"},
  {NULL, 5          , "h10_standard80"            , ": 10 Level H Macro-atom"},
  {NULL, 6          , "h20"                       , ": 20 Level H Macro-atom"},
  {NULL, 7          , "h20_hetop_standard80"      , ": 20 Level H and He Macro-atoms"},
  {NULL, 8          , "standard80"                , ": Standard Simple-atom"},
  {NULL, 9          , "standard80_reduced"        , ": Reduced Simple-atom"},
  {NULL, 10         , "standard80_sn_kurucz"      , ": Standard Supernova Simple-atom"},
  {NULL, ATOMIC_TEST, "standard80_test"           , ": Standard Test Simple-atom"},
  {NULL, INDEX_OTHER, "Other"                     , ": Custom data, needs to be in $PYTHON/xdata"}
};

void
switch_atomic_data (void)
{
  int valid_input = FALSE;
  int atomic_data_error;
  int relative = FALSE;
  char atomic_data_name[FIELD_INPUT_LEN];
  WINDOW *window = CONTENT_WINDOW.win;

  static int menu_index = 8;
  static int init_name = FALSE;
  static Query_t atomic_data_query[2];

  if (init_name == FALSE)
  {
    strcpy (atomic_data_name, "");
    init_name = TRUE;
  }

  /*
   * Continue to loop until valid atomic data has been loaded
   */

  while (valid_input != TRUE)
  {
    menu_index = create_menu (CONTENT_WINDOW, "Please select the atomic data to use", ATOMIC_DATA_CHOICES,
                              ARRAY_SIZE (ATOMIC_DATA_CHOICES), menu_index, MENU_CONTROL);

    if (menu_index == MENU_QUIT)
    {
      break;
    }
    else if (menu_index > MENU_QUIT)
    {
      if (ATOMIC_DATA_CHOICES[menu_index].index == ATOMIC_TEST)  // Special hardcoded case
      {
        strcpy (atomic_data_name, "../data/standard80_test.dat");
        relative = TRUE;
      }
      else if (ATOMIC_DATA_CHOICES[menu_index].index != INDEX_OTHER)
      {
        strcpy (atomic_data_name, ATOMIC_DATA_CHOICES[menu_index].name);
        strcat (atomic_data_name, ".dat");
      }
      else
      {
        relative = TRUE;
        init_single_question_form (atomic_data_query, "Master file : ", atomic_data_name);
        query_user (CONTENT_WINDOW, atomic_data_query, 2, "Please input the name of the atomic data master file");
        strcpy (atomic_data_name, atomic_data_query[1].buffer);
      }
    }

    clean_up_display (&ATOMIC_BUFFER);
    atomic_data_error = get_atomic_data (atomic_data_name, relative);

    if (atomic_data_error)
    {
      error_atomix ("Problem reading atomic data %s : errno = %i", atomic_data_name, atomic_data_error);
    }
    else
    {
      valid_input = TRUE;
      strcpy (AtomixConfiguration.atomic_data, atomic_data_name);
    }

    wrefresh (window);
  }

  atomic_summary_show (SCROLL_DISBALE);
  logfile ("\n");
  logfile_flush();
}
