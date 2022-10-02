################################################################################
# SSL/TLS support: 0, disabled; 1, enabled.
HTTPS_ENABLED	?= 1

################################################################################
PRJ_DIR = .

################################################################################
INC_PATH = $(PRJ_DIR)/include
SRC_PATH = $(PRJ_DIR)/src
OBJ_PATH = $(PRJ_DIR)/obj
BIN_PATH = $(PRJ_DIR)/bin

BIN = ${BIN_PATH}/mini-httpd

################################################################################
CC = gcc

################################################################################
CFLAGS = -O2 -Wall
CFLAGS = -I${INC_PATH}
#CFLAGS = -g -I . 
CFLAGS += -D_GNU_SOURCE
ifeq ($(HTTPS_ENABLED),1)
CFLAGS += -DMINI_HTTPD_HTTPS_ENABLED=1
else
CFLAGS += -DMINI_HTTPD_HTTPS_ENABLED=0
endif

SOFLAGS =
#If you support https,then SOFLAGS add -lssl -lcrypto
ifeq ($(HTTPS_ENABLED),1)
SOFLAGS += -lssl -lcrypto 
endif
SOFLAGS += -lpthread 

################################################################################
SRC_CFILES = $(SRC_PATH)/main.c		\
	$(SRC_PATH)/mini_httpd_daemon.c	\
	$(SRC_PATH)/mini_httpd_debug.c	\
	$(SRC_PATH)/mini_httpd_helper.c	\
	$(SRC_PATH)/mini_httpd_log.c 	\
	$(SRC_PATH)/mini_httpd_mutex.c	\
	$(SRC_PATH)/parse_config.c 	\
	$(SRC_PATH)/parse_option.c	\
	$(SRC_PATH)/secure_access.c	\
	$(SRC_PATH)/wrap.c

################################################################################
SRC_OBJ_FILES = $(subst $(SRC_PATH)/, $(OBJ_PATH)/, $(SRC_CFILES:.c=.o))
SRC_OBJFILES := $(foreach n,$(SRC_OBJ_FILES),$(n))

################################################################################
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@echo ":: compiling $^ ..."
	@$(CC) $(CFLAGS) -c $^ -o $@

################################################################################
all: ${BIN} cgi

################################################################################
$(BIN): $(SRC_OBJFILES)
	@echo ":: generate $@ ..."
	@$(CC) -o $@ $^ $(SOFLAGS)

cgi:
	(cd cgi-bin; make)

clean:
	@(cd cgi-bin; make clean)
	@rm -f ${OBJ_PATH}/*.[do] ${BIN}

