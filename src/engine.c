#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/statvfs.h>
#include <sys/file.h> 
#include <stdio.h>

#include "main.h"

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};

int
open_pidfile (void)
{
	char pidnum[6];
	
	pidfile = strcat(multidotdir, "/uniband.pid");
	pid_file = open(pidfile, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR );
printf("NAAAAAAAAAAA %s\n", pidfile);
	sprintf(pidnum, "%d", getpid());
	write (pid_file, pidnum, strlen(pidnum));
	if (pid_file == -1) { 
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
    	exit(EXIT_FAILURE);
	}
	if (flock(pid_file, LOCK_EX | LOCK_NB) == -1) {
		if	(errno == EWOULDBLOCK) {
			fprintf (stderr, "Application is already running... so exiting\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
    	exit(EXIT_FAILURE);
	}
	return 0;
}

int
close_pidfile (void)
{
	if( flock(pid_file, LOCK_UN) == -1 ) {
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	}
	if( close(pid_file) == -1 ) {
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	}
	return 0;
}

int
print_channels (void)
{
#ifdef DEBUG
	int i;

	printf("************ 5 GHz band ************\n");
	for (i=0; i<24; i++) {
		printf ("%3d ", channel_5g[i]);
	}
	printf("\n");
	for (i=0; i<24; i++) {
		printf("%3d ", chan5[i]);
	}
	printf("\n");
	printf("*********** 2.4 GHz band ***********\n");
	for (i=0; i<14; i++) {
		printf("%2d ", i+1);
	}
	printf("\n");
	for (i=0; i<14; i++) {
		printf("%2d ", chan24[i]);
		
	}
	printf ("\n");
	printf("Storagesize: %luGB\n", get_storage () / 1024 / 1024);
	if (count_channels ()) {
		printf("Filecount: %lu\n", get_storage () / dumpsize / count_channels ());
	}
	fflush (stdout);
#endif	
	return 0;
}

int count_channels (void)
{
	int i, count5, count24;

	count5 = count24 = 0;
	for (i=0; i<24; i++) {
		if (chan5[i]) count5++;
	}
	for (i=0; i<14; i++) {
		if (chan24[i]) count24++;
	}
	return count5 + count24;
}

unsigned long int
get_storage (void)
{
	struct statvfs fiData;

	if (statvfs(dumppath, &fiData) < 0) {
		printf("Failed to stat %s\n", dumppath);
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
	}
	return fiData.f_bsize / 1024 * fiData.f_bavail;
}
