#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <errno.h>
#include <string.h>

#include "main.h"

int
get_ints(void)
{  
	int i, j;  
	FILE *devs;
	char returnData[64];
	regex_t wifireg;
		
	if( regcomp( &wifireg, cardprefix, 0 ) != 0 ) {
		fprintf(stderr, "%s: File: %s Function: %s\n", strerror(errno), __FILE__, __FUNCTION__);
		exit(EXIT_FAILURE);
	}
	devs = popen("cat /proc/net/dev | grep wlan | awk '{print $1}' | tr -d :", "r");
	j = 0;
	while (fgets(returnData, 64, devs) != NULL) {
		strtok(returnData, "\n");
#ifdef DEBUG
		printf("Interface : %s\n", returnData);
#endif
		if( regexec( &wifireg, returnData, 0, NULL, 0 ) == 0 ) {
			if( strcmp(scancard, returnData) != 0 ) {
				strcpy(wificards[j++], returnData);
			}
		}
	}
	pclose(devs);
	
#ifdef DEBUG
	for (i = 0; i < j; i++) {
		printf("wificards: %s\n", wificards[i]);
	}
#endif

	return j;
}