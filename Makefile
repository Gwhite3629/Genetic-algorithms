CC=g++
LINK=g++
TARGET=gen.exe
OBJS=main.o creature.o file.o trait.o world.o
CFLAGS= -g -Wall -Wextra -O0
LFLAGS= -g -O0

all: ${TARGET}

${TARGET}: ${OBJS}
	${CC} ${LFLAGS} -o $@ $^

main.o: main.cpp
creature.o: creature.cpp
file.o: file.cpp
trait.o: trait.cpp
world.o: world.cpp

.PHONY : clean

clean:
	del -f ${TARGET} core*
	del -f *.o core*