CC=gcc
LINK=gcc
TARGET=gen.exe
OBJS=main.o creature.o file.o trait.o world.o
CFLAGS= -g -Wall -Wextra
LFLAGS= -g

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LFLAGS} -o $@ $^

main.o: main.c
creature.o: creature.c
file.o: file.c
trait.o: trait.c
world.o: world.c

.PHONY : clean

clean:
	del -f ${TARGET} core*
	del -f *.o core*