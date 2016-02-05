#ifndef _MAIN_H
#define _MAIN_H

#include <gtk/gtk.h>

GtkBuilder *builder;
GtkWidget *horst_window;
char *kislogdir;
char *kisserpath;
char *horstpath;
char *scancard;
char *kisclipath;
char *environments;
char *dumppath;
char *dumpprefix;
char *cardprefix;
int kislogenable;
char *dumpcappath;
int dumpsize;
char *multidotdir;
char wificards[32][16];

int pid_file;
char *pidfile;
int chan5[24];
int chan24[14];
int scan_enabled;
int wifi_numbers;
pid_t kismet_server_pid;
typedef struct {
	int j;
	pid_t pid[38];
} dump_struct ;
dump_struct dumper;
int chan_detail;
int horst_chans[38];
gboolean record_active;
gboolean kismet_active;
gboolean suid_runned;
gboolean reconf_cards;

int print_channels (void);
int count_channels (void);
unsigned long int get_storage (void);
int read_options (void);
int get_ints (void);
int open_pidfile (void);
int close_pidfile (void);
void start_kismet_server (void);
void suid_card_thread (void);
int start_record_threads (void);
void checkbutton_unsensitive (void);
void checkbutton_sensitive (void);
void create_horst_dialog (void);

void select_details (GtkRadioButton *self, gpointer chan_num);
void del_horst_dialog (void);
void start_horst_win (GtkButton *self, gpointer chooser);
void horst_delete (GtkWidget *self, gpointer horst_window);
GtkWidget* create_horst_window (void);

#define DEBUG
/* FIXME turn off after it's done */
#endif