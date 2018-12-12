SOURCES=$(wildcard *.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))
CC=gcc
PROGRAM=threadpool_test
CFLAGS=-lpthread

$(PROGRAM) : $(OBJECTS)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS)
$(OBJECTS):$(SOURCES)

.PHONY : clean
clean :
	rm -rf $(OBJECTS) $(USE_EVENT_PROGRAM) $(USE_COND_PROGRAM)
