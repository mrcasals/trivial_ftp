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
  int show_help = 0;
  int server_port = RFC1350_PORT;

  /* -------- Sockets --------*/
  struct sockaddr_in server;
  struct sockaddr_in client;
  socklen_t client_length;
  int server_socket;
  int recvfrom_size;

  retransmission_time = 5;

  /* -------- TFTP --------*/
  int final = -1;
  char buffer[SERVER_BUFFER_SIZE];
  pid_t child_process;
  FILE *file;

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

  bind(server_socket, (struct sockaddr *)&server, sizeof(server));

  /* We check for socket errors */
  if( server_socket == -1 ) {
    printf(SOCKET_CONSTRUCTION_ERR);
    return -1;
  }

  client_length = sizeof(struct sockaddr_in);

  /********************************************************

      Forks to manage queries

   ********************************************************/
  signal(SIGCHLD, (func)(removeZombieChildProcesses));

  while( 1 ){
    recvfrom_size = recvfrom(server_socket, buffer, SERVER_BUFFER_SIZE, 0, (struct sockaddr *)&client, &client_length);

    if(verbose == 1) {
      log_info(&buffer, recvfrom_size, VERBOSE_SUBJECT_SERVER, VERBOSE_ACTION_RECIEVE);
    }

    child_process = fork();

    if( child_process == 0 ){
      /* Child process was created and is currently running */

      /* -------- Variables --------*/
      int client_socket;
      struct sockaddr_in socket_addr;
      tftp_rwq_hdr message;

      socket_addr.sin_family = AF_INET;
      socket_addr.sin_port = htons(0);
      socket_addr.sin_addr.s_addr = INADDR_ANY;

      client_socket = socket(PF_INET, SOCK_DGRAM, 0);

      bind(client_socket, (struct sockaddr *)&socket_addr, sizeof(socket_addr));

      /* We check for socket errors */
      if ( client_socket == -1 ) {
        printf(SOCKET_CONSTRUCTION_ERR);
        return -1;
      }

      /* We get the filename and mode from the client */
      bzero(message.filename, MAXPATH_STRLEN);
      bzero(message.mode, MAXMODE_STRLEN);

      char* tmp_filename = (char*)(buffer + 2);
      char* tmp_mode = (char*)(buffer + strlen(tmp_filename) + 3);

      memcpy(message.filename, tmp_filename, strlen(tmp_filename));
      memcpy(message.mode, tmp_mode, strlen(tmp_mode));

      /* Switch actions depending on the mode asked by the client */
printf("OPCODE: %x\n", OPCODE(buffer)); // debug
      switch( OPCODE(buffer) ) {
      case RFC1350_OP_RRQ:
        /* The client wants to read from the server */

        file = fopen(message.filename, "rb");

        if( file == NULL ) {
          /* Tell the client the file does not exist */

          sendError(RFC1350_ERR_FNOTFOUND, client, &client_socket, VERBOSE_SUBJECT_SERVER);
        }

        final = sendData(file, &client, &client_socket, VERBOSE_SUBJECT_SERVER);

        fclose(file);

        break;

      case RFC1350_OP_WRQ:
        /* The client wants to write to the server */

        file = fopen(message.filename, "rb");

        if( file == NULL ){
          /* The file does not exist, so we can create it and write on it */
          file = fopen(message.filename, "wb");

          if( file == NULL ) {
            /* There has been some problems opening the file */
            printf("File could not be opened");
            fflush(stdout);
            return -1;
          }

          sendACK(0, client, &client_socket, VERBOSE_SUBJECT_SERVER);
          final = recieveData(file, &client, &client_socket, VERBOSE_SUBJECT_SERVER);

        } else {
          /* Tell the client to file could not be created */
          sendError(RFC1350_ERR_FEXISTS, client, &client_socket, VERBOSE_SUBJECT_SERVER);
        }

        fclose(file);

        break;

      default:
        sendError(RFC1350_ERR_ILLEGALOP, client, &client_socket, VERBOSE_SUBJECT_SERVER);
      }

      close(client_socket);

      if (final == 0) {
        printf("File %s was successfully transfered!\n", message.filename);
        fflush(stdout); 
      }

      exit(0);
    }
  }

  return 0;
}
