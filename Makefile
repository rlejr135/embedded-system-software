CC = arm-none-linux-gnueabi-gcc -pthread -static
#CC = gcc

CFLAGS = -c

OBJS = 20141602.o input.o main_process.o output.o program_mode/clock.o program_mode/counter.o program_mode/text.o program_mode/draw.o program_mode/user.o

SRCS = 20141602.c input.c main_process.c output.c program_mode/clock.c program_mode/counter.c program_mode/text.c program_mode/draw.c program_mode/user.c

TARGETS = 20141602



$(TARGETS) : $(OBJS)
			   $(CC) -o $(TARGETS) $(OBJS)



clean:
	rm $(OBJS) $(TARGETS)
