char *kislogdir;
char *kisserpath;
char *horstpath;
char *scancard;
char *kisclipath;
char *environments;
char *dumppath;
char *dumpprefix;
char *cardprefix;
int cardnumbers[16];
int kislogenable;
char *dumpcappath;
int dumpsize;
char *multidotdir;
char wificards[16][10];

int chan5[24];
int chan24[14];

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};

int print_channels ();
int read_options ();
int get_int ();

#define DEBUG
/* FIXME turn off after it's done */