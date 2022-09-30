HTTPS_ENABLED	?= 1

BIN = mini-httpd

CC = gcc

CFLAGS = -O2 -Wall -I .
#CFLAGS = -g -I . 
CFLAGS += -D_GNU_SOURCE
ifeq ($(HTTPS_ENABLED),1)
CFLAGS += -DMINI_HTTPD_HTTPS_ENABLED=1
else
CFLAGS += -DMINI_HTTPD_HTTPS_ENABLED=0
endif
#CFLAGS += -Wunused-result

SHARED =
#If you support https,then SHARED=-DHTTPS -lpthread -lssl -lcrypto
ifeq ($(HTTPS_ENABLED),1)
SHARED += -lssl -lcrypto 
endif
SHARED += -lpthread 

all: ${BIN}

${BIN}: main.o wrap.o parse_config.o daemon_init.o parse_option.o log.o secure_access.o cgi
	$(CC) -o $@ main.o wrap.o parse_config.o daemon_init.o parse_option.o log.o secure_access.o $(SHARED) 

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

wrap.o: wrap.c
	$(CC) $(CFLAGS) -c wrap.c

parse_config.o: parse_config.c parse.h
	$(CC) $(CFLAGS) -c parse_config.c

daemon_init.o: daemon_init.c parse.h
	$(CC) $(CFLAGS) -c daemon_init.c

parse_option.o: parse_option.c parse.h
	$(CC) $(CFLAGS) -c parse_option.c

log.o: log.c parse.h
	$(CC) $(CFLAGS) -c log.c

secure_access.o: secure_access.c  parse.h
	$(CC) $(CFLAGS) -c secure_access.c

cgi:
	(cd cgi-bin; make)

clean:
	rm -f *.o main access.log *~
	(cd cgi-bin; make clean)

