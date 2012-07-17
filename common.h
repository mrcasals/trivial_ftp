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

/*
   Copyright (c) 2007 Sergio Castillo P�rez

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ------------------------------------------------------------------------- */

#ifndef _TFTP_H_
#define _TFTP_H_

#define VERSION      "0.1.0"

#define RFC1350_OP_RRQ    1  /* Read request   */
#define RFC1350_OP_WRQ    2  /* Write request  */
#define RFC1350_OP_DATA    3  /* Data           */
#define RFC1350_OP_ACK    4  /* Acknowledgment */
#define RFC1350_OP_ERROR  5  /* Error          */

#define RFC1350_ERR_NOTDEF  0  /* Not defined, see err. messsage   */
#define RFC1350_ERR_FNOTFOUND  1  /* File not found                   */
#define RFC1350_ERR_ACCESS  2  /* Access violation                 */
#define RFC1350_ERR_DISKFULL  3  /* Disk full or allocation exceeded */
#define RFC1350_ERR_ILLEGALOP  4  /* Illegal TFTP operation           */
#define RFC1350_ERR_UKNOWID  5  /* Unknown transfer ID              */
#define RFC1350_ERR_FEXISTS  6  /* File already exists              */
#define RFC1350_ERR_NOUSER  7  /* No such user                     */

#define RFC1350_PORT    69  /* Transport TFTP port  */
#define RFC1350_BLOCKSIZE  512  /* Max. data block size */

/* ---------------- ERRORS ------------------- */
#define ACCESS_VIOLATION_ERR "Access violation error.\n"
#define COULD_NOT_CREATE_FILE_ERR "File could not be created.\n"
#define DISK_FULL_ERR "Disk full, file can't be created.\n"
#define FILE_ALREADY_EXISTS_ERR "File already exists.\n"
#define FILE_DOES_NOT_EXIST_ERR "File does not exist.\n"
#define FILE_NOT_FOUND_ERR "File not found.\n"
#define ILLEGAL_OP_ERR "Illegal operation.\n"
#define NO_FILE_SET_ERR "No file name has been set.\n"
#define NO_HOST_SET_ERR "No host has been set.\n"
#define NO_MODE_SET_ERR "No mode has been set.\n"
#define NO_SUCH_USER_ERR "No such user.\n"
#define SOCKET_CONSTRUCTION_ERR "There has been an error constructing the socket.\n"
#define TWO_MODES_SET_ERR "Two modes have been set. Please, set only one mode.\n"
#define UNKNOWN_ERR "Unknown error.\n"
#define UNKNOWN_TRANSFER_ID_ERR "Unknown transfer ID.\n"

/* ---------------- VERBOSE ------------------- */
#define VERBOSE_ACTION_SEND "E"
#define VERBOSE_ACTION_RECIEVE "R"
#define VERBOSE_SUBJECT_CLIENT "C:"
#define VERBOSE_SUBJECT_SERVER "S:"
#define VERBOSE_RRQ   "%s %s RRQ   [%s]\n"        //RRQ  [filename]
#define VERBOSE_WRQ   "%s %s WRQ   [%s]\n"        //WRQ [filename]
#define VERBOSE_DATA  "%s %s DATA  [%d#] [%d]\n"  //DATA [Block #] [size]
#define VERBOSE_ACK   "%s %s ACK   [%d#]\n"       //ACK: ACK [Block #]
#define VERBOSE_ERROR "%s %s ERROR [%s]\n"        //ERROR: ERROR [ErrMsg]
#define VERBOSE_OTHER "%s %s Unknown message\n"

/* ---------------- OTHERS ------------------- */
#define CHLD_DELETED "Child %d was deleted.\n"
#define SERVER_BUFFER_SIZE 1536
#define LOG_INFO_SUBJECT_SIZE 3
#define LOG_INFO_WHAT_SIZE 2

#define u_int16_t unsigned short
#define OPCODE(addr) (ntohs(* ((u_int16_t *) addr)))
#define st_u_int16(sval, addr) (*((u_int16_t *)(addr))=htons(sval))

/* ------------------------------------------------------------------------- */

typedef enum {
  FALSE,
  TRUE
} BOOL;

/* ------------------------------------------------------------------------- */

/*
   Read/Write header. See also RFC1350.

         .-------+------------+------+------------+------.
   RRQ/  | 01/02 |  Filename  |   0  |    Mode    |   0  |
   WRQ   '-------+------------+------+------------+------'
          2 bytes    string   1 byte     string    1 byte

*/

#define MAXPATH_STRLEN 1024  /* Should be enough for all systems */
#define MAXMODE_STRLEN 8  /* octet, mail, netascii */

typedef struct {
  u_int16_t opcode;
  char filename[MAXPATH_STRLEN + 1];
  char mode[MAXMODE_STRLEN + 1];
} tftp_rwq_hdr;


/* ------------------------------------------------------------------------- */

/*
   Data header. See also RFC1350.

         .-------+------------+----------------.
   DATA  | 03    |   Block #  |      Data      |
         '-------+------------+----------------'
          2 bytes    2 bytes     1..512 bytes
*/

typedef struct {
  u_int16_t opcode;
  u_int16_t num_block;
  char data[RFC1350_BLOCKSIZE];
} tftp_data_hdr;

/* ------------------------------------------------------------------------- */

/*
   Acknowlege header. See also RFC1350.

         .-------+------------.
   ACK   | 04    |   Block #  |
         '-------+------------'
          2 bytes  2 bytes
*/

typedef struct {
  u_int16_t opcode;
  u_int16_t num_block;
} tftp_ack_hdr;

/* ------------------------------------------------------------------------- */

/*
   Error header. See also RFC1350.

         .-------+------------+------------+------.
   ERROR | 05    |  ErrorCode |   ErrMsg   |   0  |
         '-------+------------+------------+------'
          2 bytes  2 bytes        string    1 byte
*/

#define MAX_ERROR_STRLEN 300

typedef struct {
  u_int16_t opcode;
  u_int16_t code;
  char message[MAX_ERROR_STRLEN + 1];
} tftp_error_hdr;


#endif

/* ------------------------------------------------------------------------- */

typedef void (*func)(int);

/* -------- Global variables --------*/
int retransmission_time; // Used on retransmission
int verbose;

void printError(char *buffer, int size);
void log_info(void *message, int size, char verbose_subject[LOG_INFO_SUBJECT_SIZE], char verbose_text[LOG_INFO_WHAT_SIZE]);
void sendError(unsigned short error_code, struct sockaddr_in destination_address, int *sock, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
void sendACK(unsigned short index, struct sockaddr_in destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
int recieveACK(unsigned short index, struct sockaddr_in *destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
void sendDataPackage(char data[RFC1350_BLOCKSIZE], int size, unsigned short index, struct sockaddr_in destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
int recieveDataPackage(unsigned short index, char data[RFC1350_BLOCKSIZE], struct sockaddr_in *destination_addr, int* sock, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
int sendData(FILE *file, struct sockaddr_in *reciever, int *sender_socket, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
int recieveData(FILE *file, struct sockaddr_in *sender, int *reciever_socket, char verbose_text[LOG_INFO_SUBJECT_SIZE]);
