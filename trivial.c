/********************************************************
 * Xarxes de Computadors 2: Trivial FTP                 *
 * Autor:  Marc Riera      - 1242263                    *
 *                                                      *
 * Fitxer: trivial.c                                    *
 * Descripció: Codi del client TFTP.                    *
 ********************************************************/

#include "common.h"

int showHelp() {
  printf("/********************************************************\n\
  \n\
      trivial -f file -H host [-p port] {-r|-w} -v\n\
  \n\
      Compulsory params:\n\
        -f: filename\n\
        -H: server host\n\
        [-r, -w]: reads/writes to the server\n\
  \n\
      Optional params:\n\
        -p: server port. If not set, port will be 69\n\
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

      Compulsory params:
        -f: filename
        -H: server host
        [-r, -w]: reads/writes to the server

      Optional params:
        -p: server port. If not set, port will be 69
        -v: use verbose mode

      Help:
        -h: shows this info

   ********************************************************/

  int param;
  int verbose = 0;
  int show_help = 0;
  int server_port = 69;
  int mode = 0;
  char *server_host, *target_file;

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

  /********************************************************

      Param control

   ********************************************************/

  if( target_file == NULL ){
    printf(NO_FILE_SET_ERR);
    fflush(stdout);
    show_help = 1;
  }

  if( server_host == NULL ){
    printf(NO_HOST_SET_ERR);
    fflush(stdout);
    show_help = 1;
  }

  if( mode == 0 ) {
    printf(NO_MODE_SET_ERR);
    fflush(stdout);
    show_help = 1;
  }

  if( mode < 0 ) {
    printf(TWO_MODES_SET_ERR);
    fflush(stdout);
    show_help = 1;
  }

  if( show_help == 1 ) {
    showHelp();
  }

  return 0;
}
