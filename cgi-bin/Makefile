CC = gcc
CFLAGS = -O2 -Wall -I ../include

all: getAuth  postAuth

getAuth: getAuth.c
	$(CC) $(CFLAGS) -o getAuth getAuth.c

postAuth: postAuth.c
	$(CC) $(CFLAGS) -o postAuth postAuth.c

clean:
	rm -f getAuth postAuth *~
