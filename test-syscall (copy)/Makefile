# Makefile for building application targeting QEMU system
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET=test_ancestor array_stats_test

CC_C = gcc

# NOTE: Do not statically link because of a bug in sem_unlink:
# See bug: https://stackoverflow.com/a/47914897/3475174
CFLAGS = -Werror -Wall -g -std=c99 -D _GNU_SOURCE -pthread

all: clean $(TARGET)

$(TARGET):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGET)

transfer: all
	scp -P 10022 $(TARGET) root@localhost:~
	# sshpass -p "root" scp -P 10022 $(TARGET) root@localhost:~

copy_core:
	scp -P 10022 root@localhost:~/core .

ssh:
	ssh -p 10022 root@localhost

killqemu:
	killall qemu-system-x86_64

