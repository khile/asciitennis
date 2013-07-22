#   $Id: Makefile,v 1.6 2012-04-10 19:14:39 whoru Exp $

PROG=pong

# add new sources here
SRCS=pong.c utils.c

OBJS=${SRCS:.c=.o}

CLEAN=${OBJS} ${PROG}

CC=gcc

CFLAGS=-g -Wall

${PROG}: ${OBJS}
	${CC} ${CFLAGS} -o ${PROG} ${OBJS}

clean:
	rm -vf ${CLEAN}

# dependency lis
pong.c: tools.h
utils.c: tools.h
kbhit.c: kbhit.h

