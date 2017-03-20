CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

GIT_HOOKS := .git/hooks/applied

EXEC = naive sse sse_prefetch

all: $(GIT_HOOKS) $(EXEC)  main.c

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

naive: main.c
	$(CC) $(CFLAGS) -D NAIVE -o naive main.c

sse: main.c
	$(CC) $(CFLAGS) -D SSE -o sse main.c

sse_prefetch: main.c
	$(CC) $(CFLAGS) -D SSE_PREFETCH -o sse_prefetch main.c

cache-test: $(EXEC)
	perf stat --repeat 50 \
	-e cache-misses,cache-references,L1-dcache-loads,L1-dcache-load-misses,r014c,r024c \
	./naive
	perf stat --repeat 50 \
	-e cache-misses,cache-references,L1-dcache-loads,L1-dcache-load-misses,r014c,r024c \
	./sse
	perf stat --repeat 50 \
	-e cache-misses,cache-references,L1-dcache-loads,L1-dcache-load-misses,r014c,r024c \
	./sse_prefetch
clean:
	$(RM) naive sse sse_prefetch
