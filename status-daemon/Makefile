status_daemon: status_daemon.c status.c
	gcc -Wall -O2 -o status_daemon status_daemon.c status.c

clean:
	rm -f status_daemon

run:
	./status_daemon

.PHONY: clean run
