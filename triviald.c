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
      triviald [-p port] [-v]\n\
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

void removeZombieChildProcesses(int i) {
    pid_t pid;

    while( (pid = wait3(NULL, WNOHANG, NULL)) > 0 ) {
        printf(CHLD_DELETED, pid);
        fflush(stdout);
    }
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

  /* -------- Params --------*/
  int param;
  int verbose = 0;
  int show_help = 0;
  int server_port = RFC1350_PORT;

  /* -------- Sockets --------*/
  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t client_length;
  int server_socket;
  int recvfrom_size;

  char buffer[SERVER_BUFFER_SIZE];

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
    return -1;
  }

  /********************************************************

      Sockets initialization

   ********************************************************/
  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);
  server.sin_addr.s_addr = INADDR_ANY;

  server_socket = socket(PF_INET, SOCK_DGRAM, 0);

  /* We check for socket errors */
  if( server_socket == -1 ) {
    printf(SOCKET_CONSTRUCTION_ERR);
    return -1;
  }

  bind(server_socket, (struct sockaddr *)&server, sizeof(server));

  client_length = sizeof(struct sockaddr_in);

  /********************************************************

      Forks to manage queries

   ********************************************************/
  signal(SIGCHLD, removeZombieChildProcesses);

  while( 1 ){
    recvfrom_size = recvfrom(server_socket, buffer, SERVER_BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_length);
  }

  return 0;
}
