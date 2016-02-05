#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <libcconf.h>

#include "main.h"

#define O_KISSERPATH	0
#define O_KISSERLOG		1
#define O_KISLOGPATH	2
#define O_HORSTPATH		3
#define O_SCANCARD		4
#define O_KISCLIPATH	5
#define O_ENVIRONMENTS	6
#define O_DUMPPATH		7
#define O_DUMPPREFIX	8
#define O_CARDPREFIX	9
#define O_DUMPCAPPATH	10
#define O_DUMPSIZE		11

#define CONF_FILE PACKAGE_DATA_DIR"/conf/uniband-monitor.conf"

int
read_options ()
{
	static struct config_options options[] = {
		{"kismetserverpath", O_KISSERPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"kismetserverlog", O_KISSERLOG, T_FLAG, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"kismetlogpath", O_KISLOGPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"horstpath", O_HORSTPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"ignoredcard", O_SCANCARD, T_STRING, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"kismetclientpath", O_KISCLIPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"environments", O_ENVIRONMENTS, T_STRING, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"dumppath", O_DUMPPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"dumpprefix", O_DUMPPREFIX, T_STRING, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"cardprefix", O_CARDPREFIX, T_STRING, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"dumpcappath", O_DUMPCAPPATH, T_FILENAME, 0, S_EMPTY, I_EMPTY, A_EMPTY},
		{"dumpsize", O_DUMPSIZE, T_INTEGER, 0, S_EMPTY, I_EMPTY, A_EMPTY},
	};

	config_load(options, CONF_FILE);

	kislogenable = options[O_KISSERLOG].intval;
	kisserpath = strdup(options[O_KISSERPATH].strval);
	kislogdir = strdup(options[O_KISLOGPATH].strval);
	mkdir(kislogdir, S_IRWXU);
	horstpath = strdup(options[O_HORSTPATH].strval);
	scancard = strdup(options[O_SCANCARD].strval);
	kisclipath = strdup(options[O_KISCLIPATH].strval);
	environments = strdup(options[O_ENVIRONMENTS].strval);
	dumppath = strdup(options[O_DUMPPATH].strval);
	mkdir(dumppath, S_IRWXU);
	dumpprefix = strdup(options[O_DUMPPREFIX].strval);
	cardprefix = strdup(options[O_CARDPREFIX].strval);
	dumpcappath = strdup(options[O_DUMPCAPPATH].strval);
	dumpsize = options[O_DUMPSIZE].intval;

#ifdef DEBUG
	printf("%s: %s\n", options[O_KISSERPATH].name, kisserpath);
	printf("%s: %d\n", options[O_KISSERLOG].name, kislogenable);
	printf("%s: %s\n", options[O_KISLOGPATH].name, kislogdir);
	printf("%s: %s\n", options[O_HORSTPATH].name, horstpath);
	printf("%s: %s\n", options[O_SCANCARD].name, scancard);
	printf("%s: %s\n", options[O_KISCLIPATH].name, kisclipath);
	printf("%s: %s\n", options[O_ENVIRONMENTS].name, environments);
	printf("%s: %s\n", options[O_DUMPPATH].name, dumppath);
	printf("%s: %s\n", options[O_DUMPPREFIX].name, dumpprefix);
	printf("%s: %s\n", options[O_CARDPREFIX].name, cardprefix);
	printf("%s: %s\n", options[O_DUMPCAPPATH].name, dumpcappath);
	printf("%s: %d\n", options[O_DUMPSIZE].name, dumpsize);
#endif

	multidotdir = (char*)malloc(strlen(getenv("HOME")) + 32);
	multidotdir = strcat (getenv("HOME"), "/.uniband-monitor");
	mkdir(multidotdir, S_IRWXU);
	
	return 0;
}