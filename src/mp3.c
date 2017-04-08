#include <stdio.h>
#include <stdlib.h>
#include <error.h>

void playmusic()
{	int ret;
	ret = system("madplay test.mp3 -r &");
	
	if(ret < 0);
	{
		perror("play ");
		return -1;
	}
}