#include "tools.h"

int
main(int argc, char **argv)
{
    int delay = 5;
    int status;
    int cpid;
    int pfd[2];

    /* convert argv[1] to integer FPS */
    int speed = 30;
    if (argc == 2) {
        speed = atoi(argv[1]);
        if (speed < 1) {
            fprintf(stderr, "args: invalid FPS argument\n");
            return -1;
        }
    } else if (argc > 2) {
        fprintf(stderr, "args: incorrect number of arguments\n");
        fprintf(stdout,
                "    $ pong [fps]\n fps - A positive integer.\n%s%s",
                "    The game pong. If fps is not set then it run at ~30FPS.\n",
                "Lower this value if things run too slowly.");
    }

    /* intialize board */
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    struct board *b = malloc(
        sizeof(struct board)+(sizeof(short int)*w.ws_row*w.ws_col));
    if (b == NULL) {
        fprintf(stderr, "malloc: Could not allocate memory for board.\n");
        return -1;
    }
    b->w = w.ws_col;
    b->h = w.ws_row;
    /* initialize pieces */
    struct piece playerA = {0, 0, 2, 6, -1, 35};
    struct piece playerB = {((b->w)-2), 0, 2, 6, -1, 35}; 
    struct piece ball = {11, 12, 1, 1, 5, 48};
    struct piece median = {b->w/2, 0, 1, b->h, -1, 124};
    /* intialize players */
    struct player pa = {0, &playerA};
    struct player pb = {0, &playerB};

    /* set globals */
    WIDTH = b->w;
    HEIGHT = b->h;
    BOARD = b;
    PIECE_PTR[0] = &ball;
    PIECE_PTR[1] = &playerA;
    PIECE_PTR[2] = &playerB;
    PIECE_PTR[3] = &median;
    PLAYER_A = &pa;
    PLAYER_B = &pb;
    FRAME_C = 0;

    if (pipe2(pfd, O_NONBLOCK)) {
        perror("pipe2");
        return -1;
    }

    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        exit(127);
    }

    /* convert fps to ~microseconds */
    speed = (int)((float)1000000/(float)speed);

    /* check if we are child */
    if (cpid == 0) {
        char buf[2];

        /* child writes to pipe so close read */
        buf[1] = 0;
        close(pfd[0]);
        for (;;) {
            buf[0] = getinput();
            write(pfd[1], &buf, 2);
            if (buf[0] == 'q') {
                return 0;
            }
            usleep(speed);
        }
        return -1;
    }
    /* close write end in parent */
    close(pfd[1]);

    /* run game in parent */
    char input[2];

    input[1] = 0;
    for(;;) {
        clear();
        playgame(delay);
        refresh();
        input[0] = 0;
        read(pfd[0], &input, 2);
        switch (input[0]) {
            case 'w':
                PLAYER_A->p->y -= 1;
                break;
            case 's':
                PLAYER_A->p->y += 1;
                break;
            case 'a':
                PLAYER_A->p->x -= 1;
                break;
            case 'd':
                PLAYER_A->p->x += 1;
                break;
            case 'q':
                waitpid(-1, &status, 0);
                return 0;
        }
        usleep(speed);
    }
    return 0;
}

