/* lines.c */
void bound_bound_header(void);
void bound_bound_line(int n);
void all_bound_bound(void);
void bound_bound_wavelength_range(void);
void bound_bound_element(void);
void bound_bound_ion(void);
/* buffer.c */
void clean_up_display(Display_t *buffer);
void add_display(Display_t *buffer, char *fmt, ...);
void add_sep_display(const int len);
void update_current_line_progress(Window_t win, int current_line, int total_lines);
void scroll_display(Display_t *buffer, Window_t win, _Bool persistent_header, int header_rows);
void display_buffer(Display_t *buffer, int scroll, _Bool persisent_header, int header_rows);
/* main.c */
int main(int argc, char *argv[]);
/* menu.c */
void clean_up_menu(MENU *menu, ITEM **items, int nitems);
int control_menu(MENU *menu, int c);
int create_main_menu(char *menu_message, MenuItem_t *menu_items, int nitems, int current_index, int control_this_menu);
int create_menu(Window_t win, char *menu_message, MenuItem_t *menu_items, int nitems, int current_index, int control_this_menu);
void main_menu(int control);
void bound_bound_main_menu(void);
void bound_free_main_menu(void);
void elements_main_menu(void);
void ions_main_menu(void);
void inner_shell_main_menu(void);
void levels_main_menu(void);
/* tools.c */
void get_element_name(int z, char *element);
void get_atomic_number(const char *element, int *atomic_number);
int find_element(int z);
void error_atomix(char *fmt, ...);
void exit_atomix(int errno, char *fmt, ...);
void menu_exit_atomix(void);
char *trim_whitespaces(char *str);
void count(int ndash, int count);
int create_string(char *str, char *fmt, ...);
/* ui.c */
void initialise_ncurses_stdscr(void);
void cleanup_ncurses_stdscr(void);
void initialise_main_windows(void);
void draw_window_boundaries(void);
void redraw_screen(int sig);
void bold_message(Window_t win, int y, int x, char *fmt, ...);
void update_status_bar(char *fmt, ...);
void home_screen(void);
/* photoionization.c */
void bound_free_header(void);
void bound_free_line(int nphot);
void all_bound_free(void);
void bound_free_wavelength_range(void);
void bound_free_element(void);
void bound_free_ion(void);
/* atomic_data.c */
void view_atomic_summary(void);
int fraction(double value, double array[], int npts, int *ival, double *f, int mode);
int linterp(double x, double xarray[], double yarray[], int xdim, double *y, int mode);
int index_phot_top(void);
int index_inner_cross(void);
void indexx(int n, float arrin[], int indx[]);
int limit_lines(double freqmin, double freqmax);
int check_xsections(void);
double a21(struct lines *line_ptr);
double upsilon(int n_coll, double u0);
int index_lines(void);
int get_atomic_data(char *masterfile, int use_relative);
/* query.c */
void clean_up_form(FORM *form, FIELD **fields, int nfields);
int control_form(FORM *form, int ch, int exit_index);
int query_user(Window_t w, Query_t *q, int nfields, char *title_message);
void init_single_question_form(Query_t *q, char *label, char *answer);
void init_two_question_form(Query_t *q, char *label1, char *label2, char *answer1, char *answer2);
int query_wavelength_range(double *wmin, double *wmax);
int query_atomic_number(int *z);
int query_ion_input(int nion_or_z, int *z, int *istate, int *nion);
void switch_atomic_data(void);
int query_atomic_number_by_symbol(int *z);
/* elements.c */
void elements_header(void);
void element_line(struct elements e);
void single_element_info(struct elements e, int detailed);
void all_elements(void);
void single_element(void);
/* ions.c */
void ion_header(void);
void ion_line(int nion);
void single_ion_info(int nion, int detailed);
void all_ions(void);
void single_ion_atomic_z(void);
void single_ion_nion(void);
void ions_for_element(void);
/* levels.c */
void atomic_level_header(void);
void atomic_level_line(int n);
void all_level_configurations(void);
/* inner.c */
void inner_shell_header(void);
void inner_shell_line(int nphot);
void all_inner_shell(void);
void inner_shell_wavelength_range(void);
void inner_shell_element(void);
void inner_shell_ion(void);
/* parse.c */
int check_command_line(int argc, char **argv);
