PROG=asciitennis

# add new sources here
SRCS=asciitennis.c utils.c

OBJS=${SRCS:.c=.o}

CLEAN=${OBJS} ${PROG}

CC=gcc

CFLAGS=-g -Wall

${PROG}: ${OBJS}
	${CC} ${CFLAGS} -o ${PROG} ${OBJS}

clean:
	rm -vf ${CLEAN}

# dependency lis
asciitennis.c: tools.h
utils.c: tools.h
