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

void sendQuery(tftp_rwq_hdr query, int *socket_origin, struct sockaddr_in server) {

  /* -------- Variables --------*/
  socklen_t server_length = sizeof(struct sockaddr_in);
  int query_size = 0;
  char buffer[516];

  bzero(buffer, 516);

  st_u_int16(query.opcode, buffer);
  query_size += sizeof(u_int16_t);

  strcpy(buffer + query_size, query.filename);
  query_size += (int) strlen(query.filename) + 1;

  strcpy(buffer + query_size, query.mode);
  query_size += (int) strlen(query.mode) + 1;

  sendto(*socket_origin, buffer, query_size, 0, (struct sockaddr *)&server, server_length);

  if( verbose == 1 ) {
    log_info(buffer, query_size, VERBOSE_SUBJECT_CLIENT, VERBOSE_ACTION_SEND);
  }
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

  /* --------- Params --------*/
  int param;
  int show_help = 0;
  int server_port = RFC1350_PORT;
  int mode = 0;
  char *server_host_name, *target_file;

  /* --------- Sockets --------*/
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *server_host;
  int client_socket;

  /* -------- FTP --------*/
  tftp_rwq_hdr query;
  FILE *file;

  while( (param = getopt(argc, argv, "hvrwt:f:H:p:") ) != -1)
  {
    switch( (char) param) {
    case 'f':
      target_file = optarg;
      break;

    case 'H':
      server_host_name = optarg;
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

  if( server_host_name == NULL ){
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
    return -1;
  }

  /********************************************************

      Sockets initialization

   ********************************************************/
  client.sin_family = AF_INET;
  client.sin_port = htons(0);
  client.sin_addr.s_addr = INADDR_ANY;

  client_socket = socket(PF_INET, SOCK_DGRAM, 0);

  /* We check for socket errors */
  if ( client_socket == -1 ) {
    printf(SOCKET_CONSTRUCTION_ERR);
    return -1;
  }

  bind(client_socket, (struct sockaddr*)&client, sizeof(client));

  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);

  server_host = gethostbyname(server_host_name);

  memcpy( (char *)&server.sin_addr, (char *)server_host->h_addr, server_host->h_length);

  /********************************************************

      Set the packages and send them

   ********************************************************/
  strcpy(query.filename, target_file);
  strcpy(query.mode, "octet");

  /* Switch actions depending on the mode given by the user */
  switch( mode ) {
  case RFC1350_OP_RRQ:
    /* The user wants to read from the server */
    /* Client sends and ACK, server sends data */

    file = fopen(target_file, "wb");

    if( file == NULL ) {
      /* File could not be created, so exit the program */
      printf(COULD_NOT_CREATE_FILE_ERR);
      fflush(stdout);
      return -1;
    }

    query.opcode = RFC1350_OP_RRQ;

    sendQuery(query, &client_socket, server);
    //get the data and put it in the file
    //YAY! DONE
    break;

  case RFC1350_OP_WRQ:
    /* The user wants to write to the server */
    /* Client sends data, server sends and ACK */

    file = fopen(target_file, "rb");

    if( file == NULL ) {
      /* File does not exist */
      printf(FILE_DOES_NOT_EXIST_ERR);
      fflush(stdout);
      return -1;
    }

    query.opcode = RFC1350_OP_WRQ;

    sendQuery(query, &client_socket, server);
    //get the ACK from the server
    //YAY! DONE
    break;
  }

  close(client_socket);

  return 0;
}
