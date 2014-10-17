#ifndef __XSERIAL_H__
#define __XSERIAL_H__

#include <termios.h> 

int     open_serial( char *dev_name, int baud, struct termios *oldtio) ;
void    close_serial( int fd ) ;
#endif

