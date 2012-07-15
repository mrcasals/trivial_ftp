/********************************************************
 * Xarxes de Computadors 2: Trivial FTP                 *
 * Autor:  Marc Riera      - 1242263                    *
 *                                                      *
 * Fitxer: common.c                                     *
 * Descripció: Codi comú pel client servidor TFTP.      *
 ********************************************************/

#include "common.h"

void printError(char *buffer, int size) {
    tftp_error_hdr error_message;
    
    memcpy(&error_message, buffer, size);
    printf("ERROR %d:\n%s\n", ntohs(error_message.code), error_message.message); 
}

void sendACK(unsigned short index, struct sockaddr_in destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE], int verbose) {
    tftp_ack_hdr message;
    int size = 0;
    socklen_t destination_addr_len = sizeof(struct sockaddr_in);

    message.opcode = htons(RFC1350_OP_ACK);
    size += sizeof(message.opcode);

    message.num_block = htons(index);
    size += sizeof(message.num_block);

    sendto(*sock, (char *)&message, size, 0, (struct sockaddr*)&destination_addr, destination_addr_len);
    if (verbose) {
      // log_info
    }
}

int recieveACK(unsigned short index, struct sockaddr_in *destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE], int verbose, int rexmt) {

    char buffer[516];
    int recv_size;
    socklen_t destination_addr_len = sizeof(struct sockaddr_in);

    int result;
    struct timeval t_retx;
    fd_set fds;

    FD_ZERO ( &fds ) ;
    FD_SET ( *sock, &fds ) ;

    t_retx.tv_sec = rexmt;
    t_retx.tv_usec = 0;

    result = select ( (*sock) + 1, &fds, NULL, NULL, &t_retx);
    if (result == 1) {
      recv_size = recvfrom(*sock, buffer, 516, 0, (struct sockaddr*)destination_addr, &destination_addr_len);

      if( verbose ) {
        //log_info
      }

      if (OPCODE(buffer) == RFC1350_OP_ERROR) {
        printError(buffer, recv_size);
        /* We recieved an error message */
        return -1;

      } else if (OPCODE(buffer) == RFC1350_OP_ACK) {
        tftp_ack_hdr message_ack;
        memcpy(&message_ack, buffer, recv_size);
        if (ntohs(message_ack.num_block) == index) {
          /* Expected ACK recieved. Send next data package */
          return 1;
        }
      }
    }

    /* ACK or message was unexpected. Package must be resent */
    return 0;
}

void sendData(char data[RFC1350_BLOCKSIZE], int size, unsigned short index, struct sockaddr_in destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE], int verbose) {

    tftp_ack_hdr message;
    socklen_t destination_addr_len = sizeof(struct sockaddr_in);

    memcpy(message.data, dades, size);

    message.opcode = htons(RFC1350_OP_DATA);
    size += sizeof(message.opcode);

    message.num_block = htons(index);
    size += sizeof(message.num_block);

    sendto(*sock, (char *)&message, size, 0, (struct sockaddr*)&destination_addr, destination_addr_len);
    if (verbose) {
      // log_info
    }
}

int recieveData(unsigned short index, char data[RFC1350_BLOCKSIZE], struct sockaddr_in *destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE], int verbose, int rexmt) {

    char buffer[516];
    int recv_size;
    socklen_t destination_addr_len = sizeof(struct sockaddr_in);

    int result;
    struct timeval t_retx;
    fd_set fds;

    FD_ZERO ( &fds ) ;
    FD_SET ( *sock, &fds ) ;

    t_retx.tv_sec = rexmt;
    t_retx.tv_usec = 0;

    result = select ( (*sock) + 1, &fds, NULL, NULL, &t_retx);
    if (result == 1) {
      recv_size = recvfrom(*sock, buffer, 516, 0, (struct sockaddr*)destination_addr, &destination_addr_len);

      if( verbose ) {
        //log_info
      }

      if (OPCODE(buffer) == RFC1350_OP_ERROR) {
        printError(buffer, recv_size);
        /* We recieved an error message */
        return -1;

      } else if (OPCODE(buffer) == RFC1350_OP_DATA) {
        tftp_ack_hdr message_ack;

        memcpy(&message_ack, buffer, recv_size);

        if (ntohs(message_ack.num_block) == index) {
          /* Expected DATA recieved. */
          memcpy(data, message.data, recv_size - 4);

          return recv_size;
        }
      }
    }

    /* DATA or message was unexpected. */
    return 0;
}
