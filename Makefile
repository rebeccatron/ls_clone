CC = gcc

CFLAGS = -std=c99
CFLAGS += -g 
CFLAGS += -Wall 
CFLAGS += -Werror 
CFLAGS += -Wextra 
CFLAGS += -pedantic 
CFLAGS += -Wmissing-declarations

ASANFLAGS = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

all: mls

mls:
	@echo Compiling: $@
	@$(CC) $(ASANFLAGS) $(CFLAGS) mls.c -o mls
	@echo done! ✅

.PHONY: clean
clean:
	@echo byeeee 👋
	rm -rf mls mls.dSYM
