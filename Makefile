CC=gcc
CFLAGS=-Wall -g -Wextra

# define targets
TARGETS=tema2

build: $(TARGETS)

tema2: tema2.c
	$(CC) $(CFLAGS) tema2.c -o tema2

run: $(TARGETS)
	./$(TARGETS)

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
