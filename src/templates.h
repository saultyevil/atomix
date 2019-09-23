/* get_atomic_data.c */
int get_atomic_data(char masterfile[]);
int index_lines(void);
int index_phot_top(void);
int index_inner_cross(void);
void indexx(int n, float arrin[], int indx[]);
int limit_lines(double freqmin, double freqmax);
int check_xsections(void);
double q21(struct lines *line_ptr, double t);
double q12(struct lines *line_ptr, double t);
double a21(struct lines *line_ptr);
double upsilon(int n_coll, double u0);
int fraction(double value, double array[], int npts, int *ival, double *f, int mode);
int linterp(double x, double xarray[], double yarray[], int xdim, double *y, int mode);
/* rdpar.c */
int opar(char filename[]);
int add_par(char filename[]);
int cpar(char filename[]);
int rdpar_init(void);
int string_process(char question[], char dummy[]);
int string_process_from_command_line(char question[], char dummy[]);
int string_process_from_file(char question[], char dummy[]);
int rdpar_store_record(char *name, char *value);
int rdpar_comment(char *format, ...);
int message(char string[]);
int rdstr(char question[], char answer[]);
int rdchar(char question[], char *answer);
int rdint(char question[], int *answer);
int rdflo(char question[], float *answer);
int rddoub(char question[], double *answer);
int rdline(char question[], char answer[]);
int string2int(char *word, char *string_choices, char *string_values, char *string_answer);
int rdchoice(char question[], char answers[], char *answer);
int get_root(char root[], char total[]);
int rdpar_set_mpi_rank(int rank);
int rdpar_set_verbose(int vlevel);
/* synonyms.c */
int get_question_name_length(char question[]);
int are_synonym_lists_valid(void);
int is_input_line_synonym_for_question(char question[], char input_line[]);
/* xlog.c */
int Log_init(char *filename);
int Log_close(void);
int Log(char *format, ...);
int Log_silent(char *format, ...);
int Error(char *format, ...);
int Error_silent(char *format, ...);
int error_count(char *format);
int error_summary(char *message);
int error_summary_parallel(char *msg);
int Log_flush(void);
int Log_set_mpi_rank(int rank, int n_mpi);
int Log_parallel(char *format, ...);
int Debug(char *format, ...);
void Exit(int error_code);