/********************************************************
 * Xarxes de Computadors 2: Trivial FTP                 *
 * Autor:  Marc Riera      - 1242263                    *
 *                                                      *
 * Fitxer: trivial.c                                    *
 * Descripció: Codi del client TFTP.                    *
 ********************************************************/

#include <unistd.h> //per tipus com el socklen_t
#include <netdb.h> //pel gethostbyname
#include <errno.h> //per gestionar errors
#include <sys/types.h> //per tipus com el uint
#include <netinet/in.h> //pel INADDR_ANY
#include <sys/socket.h> //per la creaci? de sockets

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <signal.h> 

int main(int argc, char *argv[])
{
  /********************************************************

      Param initialitzation and control.

      Compulsory params:
        -f: filename
        -H: server host
        -p: server port
        [-r, -w]: reads/writes to the server

      Optional params:
        -v: use verbose mode

      Help:
        -h: shows this info

   ********************************************************/

  int param, mode, verbose, show_help;
  int server_port = 69;
  char *server_host, *target_file;

#define RFC1350_OP_RRQ 1
#define RFC1350_OP_WRQ 2

  while( (param = getopt(argc, argv, "hvrwt:f:H:p:") ) != -1)
  {
    switch( (char) param) {
    case 'f':
      target_file = optarg;
      break;

    case 'H':
      server_host = optarg;
      break;

    case 'p':
      server_port = atoi(optarg);
      break;

    case 'r':
      /* User wants to read data from the server. Flags -r and -w cannot be set
       * at the same time.*/
      if( mode != 0 ) {
        /* Mode is already set */
        mode = -1;
      } else {
        mode = RFC1350_OP_RRQ;
      }
      break;

    case 'w':
      /* User wants to write data to the server. Flags -r and -w cannot be set
       * at the same time.*/
      if( mode != 0 ) {
        /* Mode is already set */
        mode = -1;
      } else {
        mode = RFC1350_OP_WRQ;
      }
      break;

    case 'v':
      /* User wants verbose mode, so the program 
       * must give feedback to the user */
      verbose = 1;
      break;

    case 'h':
      show_help = 1;
      break;
    }
  }

  if( show_help == 1 ) {
    showHelp();
  }

  return 0;
}

int showHelp() {
  printf("/********************************************************\n\
  \n\
      trivial -f file -H host [-p port] {-r|-w} -v\n\
  \n\
      Compulsory params:\n\
        -f: filename\n\
        -H: server host\n\
        -p: server port\n\
        [-r, -w]: reads/writes to the server\n\
  \n\
      Optional params:\n\
        -v: use verbose mode\n\
  \n\
      Help:\n\
        -h: shows this info\n\
  \n\
  ********************************************************/");
  fflush(stdout);
  return 1;
}
