whoami: whoami.c
	$(CC) -o whoami whoami.c

yes: yes.c
	$(CC) -o yes yes.c

.PHONY: clean
clean:
	rm -f *.o yes whoami
