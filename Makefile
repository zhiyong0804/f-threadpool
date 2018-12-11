FILES=$(wildcard *.cc)

ifeq ($(with_eventfd), yes)
    SOURCES=$(filter-out async_cond_queue.cc, $(FILES))
else
    SOURCES=$(filter-out async_eventfd_queue.cc, $(FILES))
endif

OBJECTS=$(patsubst %.cc,%.o,$(SOURCES))
CC=gcc
USE_EVENT_PROGRAM=event_threadpool_test
USE_COND_PROGRAM=cond_threadpool_test
CFLAGS=-lpthread

$(USE_COND_PROGRAM) : $(OBJECTS)
	$(CC) -o $(USE_COND_PROGRAM) $(OBJECTS) $(CFLAGS)
$(USE_EVENT_PROGRAM) : $(OBJECTS)
	$(CC) -o $(USE_EVENT_PROGRAM) $(OBJECTS) $(CFLAGS) -DPOOL_WITH_EVENT_QUEUE
$(OBJECTS):$(SOURCES)

cond : $(USE_COND_PROGRAM) 
event: $(USE_EVENT_PROGRAM)

.PHONY : clean
clean :
	rm -rf $(OBJECTS) $(USE_EVENT_PROGRAM) $(USE_COND_PROGRAM)
