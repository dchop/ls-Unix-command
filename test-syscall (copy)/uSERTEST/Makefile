TARGET=cs300_testapp array_stats_test process_ancestors_test

CC_C = gcc

CFLAGS = -Wall -g -std=c99 -D _GNU_SOURCE -static -pthread

all: clean $(TARGET)

$(TARGET):
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f $(TARGET)

transfer:
	scp -P 10022 $(TARGET) root@localhost:~

killquemu:
	killall quemu-system-x86_64
