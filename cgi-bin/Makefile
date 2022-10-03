CGICC_BASE = /home/yangmeng/Code/Release/Module/3rdParty/cgicc

CC = gcc
CXX = g++

CFLAGS = -O2 -Wall -I../include -I${CGICC_BASE}/include
LDFLAGS = -L${CGICC_BASE}/lib/linux_x64 -lcgicc

all: getAuth postAuth upload.cgi uploadFile1 uploadFile2 testcgi

getAuth: getAuth.c
	@echo ":: generate $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@

postAuth: postAuth.c
	@echo ":: generate $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@

upload: upload.cpp
	@echo ":: generate $@ ..."
	@$(CXX) $(CFLAGS) $^ -o $@ ${LDFLAGS}

upload1: upload.c
	@echo ":: generate $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@

upload.cgi: upload2.cpp
	@echo ":: generate $@ ..."
	@$(CXX) $(CFLAGS) $^ -o $@ ${LDFLAGS}

uploadFile1: uploadFile.c
	@echo ":: generate $@ ..."
	@$(CC) $(CFLAGS) $^ -o $@

uploadFile2: uploadFile2.cpp ../src/mini_httpd_debug.c ../src/mini_httpd_mutex.c
	@echo ":: generate $@ ..."
	@$(CXX) $(CFLAGS) $^ -o $@ ${LDFLAGS}

testcgi: testcgi.cpp
	@echo ":: generate $@ ..."
	@$(CXX) $(CFLAGS) $^ -o $@ ${LDFLAGS}

clean:
	@rm -f getAuth postAuth upload.cgi uploadFile1 uploadFile2 testcgi *~

