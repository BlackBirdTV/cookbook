/* ex_cmds.c */
void do_ascii(exarg_T *eap);
void ex_align(exarg_T *eap);
void ex_sort(exarg_T *eap);
int do_move(linenr_T line1, linenr_T line2, linenr_T dest);
void ex_copy(linenr_T line1, linenr_T line2, linenr_T n);
void free_prev_shellcmd(void);
void do_bang(int addr_count, exarg_T *eap, int forceit, int do_in, int do_out);
void do_shell(char_u *cmd, int flags);
char_u *make_filter_cmd(char_u *cmd, char_u *itmp, char_u *otmp);
void append_redir(char_u *buf, int buflen, char_u *opt, char_u *fname);
time_T vim_time(void);
void do_fixdel(exarg_T *eap);
void print_line_no_prefix(linenr_T lnum, int use_number, int list);
void print_line(linenr_T lnum, int use_number, int list);
int rename_buffer(char_u *new_fname);
void ex_file(exarg_T *eap);
void ex_update(exarg_T *eap);
void ex_write(exarg_T *eap);
int do_write(exarg_T *eap);
int check_overwrite(exarg_T *eap, buf_T *buf, char_u *fname, char_u *ffname, int other);
void ex_wnext(exarg_T *eap);
void do_wqall(exarg_T *eap);
int getfile(int fnum, char_u *ffname_arg, char_u *sfname_arg, int setpm, linenr_T lnum, int forceit);
int do_ecmd(int fnum, char_u *ffname, char_u *sfname, exarg_T *eap, linenr_T newlnum, int flags, win_T *oldwin);
void ex_append(exarg_T *eap);
void ex_change(exarg_T *eap);
void ex_z(exarg_T *eap);
int check_restricted(void);
int check_secure(void);
void do_sub(exarg_T *eap);
int do_sub_msg(int count_only);
void ex_global(exarg_T *eap);
void global_exe(char_u *cmd);
char_u *get_old_sub(void);
void set_old_sub(char_u *val);
void free_old_sub(void);
int prepare_tagpreview(int undo_sync, int use_previewpopup, use_popup_T use_popup);
void ex_help(exarg_T *eap);
void ex_helpclose(exarg_T *eap);
char_u *check_help_lang(char_u *arg);
int help_heuristic(char_u *matched_string, int offset, int wrong_case);
int find_help_tags(char_u *arg, int *num_matches, char_u ***matches, int keep_lang);
void fix_help_buffer(void);
void ex_exusage(exarg_T *eap);
void ex_viusage(exarg_T *eap);
void ex_helptags(exarg_T *eap);
void ex_smile(exarg_T *eap);
void ex_drop(exarg_T *eap);
char_u *skip_vimgrep_pat(char_u *p, char_u **s, int *flags);
void ex_oldfiles(exarg_T *eap);
/* vim: set ft=c : */
