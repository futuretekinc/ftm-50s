I've tried to compile and run the following simple program:

#include <stdio.h>
int main() { printf("Ok\n"); return(0); }

I used the command:

	armv5-linux-gcc -o try -O2 -DOVR_DBL_DIG=14 try.c
	/home/xtra/work/ftm-50s/rootfs/apps/perl-5.20.1/Cross/run-ssh ./try

and I got the following output:

ssh: Could not resolve hostname arm-linux: Name or service not known
ssh: Could not resolve hostname arm-linux: Name or service not known
The program compiled OK, but exited with status 255.
