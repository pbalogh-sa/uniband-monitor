#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "main.h"

#define SUID_PATH PACKAGE_BIN_DIR"/suid"

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};

int
create_shared_mem(const char *substance)
{
	key_t shared_key;
	int shared_mem;

	shared_key=ftok(substance, 1);
	if (shared_key == -1) {
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	}

	shared_mem=shmget(shared_key, 1024, IPC_CREAT | IPC_EXCL);
	if (shared_mem == -1) {
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	}

	return(shared_mem);
}

int
start_record_threads (void)
{
	int i;
	int chans[38];
	char *dumpcap[38][10];
	unsigned int filecount;
	
	if (count_channels ()) {
		filecount = get_storage () / dumpsize / count_channels ();
	}
	
	i = dumper.j = 0;
	for (i = 0; i < 24; i++) {
		if (chan5[i]) {
			chans[dumper.j++] = channel_5g[i];
		}
	}
	for (i = 1; i < 15; i++) {
		if (chan24[i-1]) {
			chans[dumper.j++] = i;
		}
	}

	for (i = 0; i < dumper.j; i++) {
		printf("channels: -------------- > %d\n", chans[i]);
		dumpcap[i][0] = strdup(dumpcappath);
		dumpcap[i][1] = strdup("-i");
		dumpcap[i][2] = strdup(wificards[i]);
		dumpcap[i][3] = strdup("-w");
		dumpcap[i][4] = (char*)malloc(strlen(dumpprefix) + 16);
		sprintf(dumpcap[i][4], "%s_chan%02d.pcap", dumpprefix, chans[i]);
		dumpcap[i][5] = strdup("-b");
		dumpcap[i][6] = (char*)malloc(18);
		sprintf(dumpcap[i][6], "filesize:%d", dumpsize);
		dumpcap[i][7] = strdup("-b");
		dumpcap[i][8] = (char*)malloc(18);
		sprintf(dumpcap[i][8], "files:%d", filecount);
		dumpcap[i][9] = NULL;
		
		dumper.pid[i] = fork();
		if (dumper.pid[i] == 0) {
		/* child process */
			if (chdir(dumppath) == -1) {
				fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
				_exit(EXIT_FAILURE);
			}
#ifdef DEBUG
			printf("%s %s %s %s %s %s %s %s %s %s\n", dumpcap[i][0], dumpcap[i][1], dumpcap[i][2], dumpcap[i][3], dumpcap[i][4], dumpcap[i][5], dumpcap[i][6], dumpcap[i][7], dumpcap[i][8], dumpcap[i][9]);
#endif
			execv(dumpcap[i][0], dumpcap[i]);
#ifdef DEBUG
			printf("CHILD: my pid = %d\n", getpid());
			printf("CHILD: my parent's pid = %d\n",getppid());
			printf("CHILD: Sleeping...\n");
			sleep(1);
#endif
			_exit(EXIT_FAILURE);
		} else if (dumper.pid[i] == -1) {
			fprintf(stderr, "Fork failed\n");
			fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
			exit(EXIT_FAILURE);
		} else {
			/* parent */
#ifdef DEBUG
			printf("PARENT: my pid = %d\n", getpid());
			printf("PARENT: my child's pid = %d\n", dumper.pid[i]);
#endif
		}
		
	}
	return dumper.j;
}

void
start_kismet_server(void)
{
	pid_t pid;
	char *kis_ser[2*wifi_numbers+4];
	int i, checked_cards;
	char ncsource[32];
	
	kis_ser[0] = strdup(kisserpath);


kis_ser[1] = strdup("-f");
kis_ser[2] = strdup("/usr/local/share/uniband-monitor/conf/kismet.conf");
if(kislogenable) kis_ser[3] = strdup("");
else kis_ser[3] = strdup("-n");
kis_ser[4] = NULL;		

/*
	DRORE konfig miatt kikommentelve 
	
	if (scan_enabled) {
		kis_ser[1] = strdup("-c");
		kis_ser[2] = strdup(wificards[wifi_numbers-1]);
		if(kislogenable) kis_ser[3] = strdup("");
		else kis_ser[3] = strdup("-n");
		kis_ser[4] = NULL;	
	} else {
		checked_cards = count_channels ();
		for (i = 0; i < checked_cards; i++) {
			kis_ser[i*2+1] = strdup("-c");
			sprintf(ncsource, "%s:hop=false", wificards[i]);
			kis_ser[i*2+2] = strdup(ncsource);
		}
		if(kislogenable) kis_ser[2*checked_cards+1] = strdup("");
		else kis_ser[2*checked_cards+1] = strdup("-n");
		kis_ser[2*checked_cards+2] = strdup("-s");
		kis_ser[2*checked_cards+3] = NULL;
	}
*/
	
	pid = fork();
	if (pid == 0) {
		/* child process */
		if (chdir(kislogdir) == -1) {
			fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
			_exit(EXIT_FAILURE);
		}		
		execv(kis_ser[0], kis_ser);
#ifdef DEBUG
		printf(kis_ser[0], kis_ser[1], kis_ser[2], kis_ser[3], kis_ser[4], kis_ser[5]);
		printf("CHILD: my pid = %d\n", getpid());
		printf("CHILD: my parent's pid = %d\n",getppid());
		printf("CHILD: Sleeping...\n");
#endif
		_exit(EXIT_FAILURE);
	} else if (pid == -1) {
		fprintf(stderr, "Fork failed\n");
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	} else {
		/* parent */
		kismet_server_pid = pid;
#ifdef DEBUG
		printf("PARENT: my pid = %d\n", getpid());
		printf("PARENT: my child's pid = %d\n", pid);
#endif
	}
}

void
suid_card_thread(void)
{
	pid_t pid;
	int status;
	char *suid_thread[1+wifi_numbers+24+14+1];
	int i;
	char chan_tmp[2];
	
/* FIXME a suid program elérését végén kőbe*/
	suid_thread[0] = strdup(SUID_PATH);	
	for (i = 0; i < 24; i++) {
		sprintf(chan_tmp, "%d", chan5[i]);
		suid_thread[1+i] = strdup(chan_tmp);
	}
	for (i = 0; i < 14; i++) {
		sprintf(chan_tmp, "%d", chan24[i]);
		suid_thread[1+24+i] = strdup(chan_tmp);
	}
	for (i = 0; i < wifi_numbers; i ++) {
		suid_thread[1+24+14+i] = strdup(wificards[i]);
	}
	suid_thread[1+wifi_numbers+24+14] = NULL;

	suid_runned = TRUE;

	pid = fork();
	if (pid == 0) {
		/* child process */
#ifdef DEBUG
		printf("CHILD_SUID: my pid = %d\n", getpid());
		printf("CHILD_SUID: my parent's pid = %d\n",getppid());
#endif
		execv(suid_thread[0], suid_thread);
		_exit(EXIT_FAILURE);
	} else if (pid == -1) {
		fprintf(stderr, "Fork failed\n");
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	} else {
		/* parent */
#ifdef DEBUG
		printf("PARENT_SUID: my pid = %d\n", getpid());
		printf("PARENT_SUID: my child's pid = %d\n", pid);
		printf("PARENT_SUID: Waiting for the child_suid to exit...\n");
#endif
		waitpid(pid,  &status, WUNTRACED | WCONTINUED);
#ifdef DEBUG
		printf("PARENT_SUID: the child is dead.\n");
#endif
	}
}

