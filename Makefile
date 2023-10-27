CC = gcc
CFLAGS = -Wall -Wconversion -O3

LIBS = -lm
EXEC = filter
TEST = test
OBJS = filters.o mtxio.o
INCS = filter.h mtxio.h

all: $(EXEC) $(TEST)

debug: CFLAGS += -DDEBUG -g
debug: all

$(EXEC): main.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

$(TEST): unittests.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.c $(INCS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean
clean:
	rm -f main.o unittests.o $(OBJS) $(EXEC) $(TEST) *.~ *.mtx *.dSYM
