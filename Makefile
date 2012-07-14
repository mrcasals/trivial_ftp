CC=gcc

all: tftp

tftp:
  $(CC) triviald.c common.c -ggdb -Wall -o triviald
  $(CC) trivial.c common.c -ggdb -Wall -o trivial
clean:
  rm triviald trivial *.o