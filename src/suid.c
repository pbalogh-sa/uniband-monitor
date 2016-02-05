#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

static int channel_5g[24] = {36,40,44,48,52,56,60,64,100,104,108,112,116, \
							120,124,128,132,136,140,149,153,157,161,165};

int
main (int argc, char **argv)
{
	int i, j, k;
	char ip_commands[64];
	int chans[38];
	
	i = j = k = 0;
	setuid(0);
#ifdef DEBUG
	printf("Running commands as: \n");
	system("whoami");
	printf("SUID ---------------> argc: %d\n", argc);
	for (i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
#endif
	for (i = 0; i < 24; i++) {
		if (strcmp(argv[1 + i], "0")) {
			chans[j++] = channel_5g[i];
		}
	}
	for (i = 1; i < 15; i++) {
		if (strcmp(argv[24 + i], "0")) {
			chans[j++] = i;
		}
	}
	for (i = 39; i < 39 + j; i++) {
		sprintf(ip_commands, "ip link set dev %s down", argv[i]);
#ifdef DEBUG
		printf("%s\n", ip_commands);
#endif
		system(ip_commands);
		sprintf(ip_commands, "iw dev %s set monitor none", argv[i]);
#ifdef DEBUG
		printf("%s\n", ip_commands);
#endif
		system(ip_commands);
		sprintf(ip_commands, "ip link set dev %s promisc on up", argv[i]);
#ifdef DEBUG
		printf("%s\n", ip_commands);
#endif
		system(ip_commands);
		sprintf(ip_commands, "iw dev %s set channel %d", argv[i], chans[k++]);
#ifdef DEBUG
		printf("%s\n", ip_commands);
#endif
		system(ip_commands);
#ifdef DEBUG
		printf("Setting up: %s\n", argv[i]);
#endif
		
	}
	
	return 0;
}