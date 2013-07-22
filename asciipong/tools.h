#ifndef TOOLS_H
#define TOOLS_H

#define _GNU_SOURCE

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define PIECES 4

/* global types */
struct board {
    int w;
    int h;
    short int board[];
};
struct piece {
    int x;
    int y;
    int w;
    int h;
    short int d;
    short int c;
};
struct player {
    int points;
    struct piece *p;
};

/* global variables */
int WIDTH;
int HEIGHT;
long long int FRAME_C;
struct piece *PIECE_PTR[PIECES];
struct player *PLAYER_A, *PLAYER_B;
struct board *BOARD;

/* function prototypes */
void clear(void);
void playgame(int delay);
void refresh(void);
char getinput(void);

#endif /* TOOLS_H */

