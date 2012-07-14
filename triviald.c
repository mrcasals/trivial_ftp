/********************************************************
 * Xarxes de Computadors 2: Trivial FTP                 *
 * Autor:  Marc Riera      - 1242263                    *
 *                                                      *
 * Fitxer: triviald.c                                    *
 * Descripció: Codi del servidor TFTP.                    *
 ********************************************************/

#include "common.h"

int showHelp() {
  printf("/********************************************************\n\
  \n\
      trivial [-p port] [-v]\n\
  \n\
      Optional params:\n\
        -p: port where to listen. If not set, port will be 69\n\
        -v: use verbose mode\n\
  \n\
      Help:\n\
        -h: shows this info\n\
  \n\
  ********************************************************/");
  fflush(stdout);
  return 1;
}

int main(int argc, char *argv[])
{
  /********************************************************

      Param initialitzation.

      Optional params:
        -p: server port. If not set, port will be 69
        -v: use verbose mode

      Help:
        -h: shows this info

   ********************************************************/

  int param, verbose, show_help;
  int server_port = 69;

  while( (param = getopt(argc, argv, "hvrwt:f:H:p:") ) != -1)
  {
    switch( (char) param) {
    case 'p':
      server_port = atoi(optarg);
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

  /********************************************************

      Param control

   ********************************************************/

  if( show_help == 1 ) {
    showHelp();
  }

  return 0;
}
