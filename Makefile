CC=gcc
CFLAGS=-I.

PROG1 = user
PROG1_OBJS = user.o

PROG2 = dispatch
PROG2_OBJS = dispatch.o

PROG3 = high
PROG3_OBJS = high.o

PROG4 = low
PROG4_OBJS = low.o

PROG5 = api
PROG5_OBJS = api.o

PROGS = $(PROG1) $(PROG2) $(PROG3) $(PROG4)

all: $(PROGS)

$(PROG1): $(PROG1_OBJS) $(PROG5_OBJS)
	$(CC) -o $(PROG1) $(PROG1_OBJS) $(PROG5_OBJS) $(CFLAGS)

$(PROG2): $(PROG2_OBJS) $(PROG5_OBJS)
	$(CC) -o $(PROG2) $(PROG2_OBJS) $(PROG5_OBJS) $(CFLAGS)

$(PROG3): $(PROG3_OBJS) $(PROG5_OBJS)
	$(CC) -o $(PROG3) $(PROG3_OBJS) $(PROG5_OBJS) $(CFLAGS)

$(PROG4): $(PROG4_OBJS) $(PROG5_OBJS)
	$(CC) -o $(PROG4) $(PROG4_OBJS) $(PROG5_OBJS) $(CFLAGS)

clean:
	rm -f *.o $(PROGS) 

