CC = gcc
CFLAGS = -Wall -Iinclude -g
OBJDIR = build
TESTDIR = build/tests

TESTS = test_blocklist test_cache test_redirect test_resolver test_dns_server
SOURCES = tests/test_blocklist.c tests/test_cache.c tests/test_redirect.c tests/test_resolver.c tests/test_dns_server.c

COMMON_SRCS = src/dns_server.c src/blocklist.c src/cache.c src/redirect.c src/resolver.c src/utils.c
COMMON_OBJS = $(patsubst src/%.c, $(OBJDIR)/%.o, $(COMMON_SRCS))

SERVER_SRCS = src/main.c
SERVER_OBJS = $(patsubst src/%.c, $(OBJDIR)/%.o, $(SERVER_SRCS))

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TESTDIR):
	mkdir -p $(TESTDIR)

$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTDIR)/%: tests/%.c $(COMMON_OBJS) | $(TESTDIR)
	$(CC) $(CFLAGS) -o $@ $< $(COMMON_OBJS)

$(TESTS): %: $(TESTDIR)/%

SERVER_EXEC = dns_server

$(SERVER_EXEC): $(SERVER_OBJS) $(COMMON_OBJS) | $(OBJDIR)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS) $(COMMON_OBJS)

all: $(TESTS) $(SERVER_EXEC)

test: $(TESTS)
	@for test_exec in $(TESTS); do \
		echo "Running $$test_exec..."; \
		./$(TESTDIR)/$$test_exec; \
	done

clean:
	rm -f $(TESTDIR)/* $(OBJDIR)/*.o $(SERVER_EXEC)
