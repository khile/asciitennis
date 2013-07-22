#include "tools.h"


/* write a char to board at x, y */
void draw(short int c, int x, int y)
{
    int index = y*WIDTH+x;
    BOARD->board[index] = c;
}

/* write all pieces to board */
void drawpieces(void)
{
    int i;
    int x, y;
    struct piece *p;

    for (i = PIECES-1; i > -1; i--) {
        p = PIECE_PTR[i];
        for (y = p->y; y < (p->y)+(p->h); y++) {
            for (x = p->x; x < (p->x)+(p->w); x++) {
                if (0 <= y && y <= HEIGHT-1 &&
                    0 <= x && x <= WIDTH-1)
                    draw((p->c), x, y);
            }
        }
    } 
}

/* ball deflect direction */
int getdirection(int d, int l, int r)
{
    /* assign direction to deflect based of the edge that has been hit */
    switch (d) {
        case -1:
            fprintf(stderr, "moveball: still piece\n");
            break;
        case 0:
            return 4;
        case 1:
            if (r)
                return 7;
            return 3;
        case 2:
            return 6;
        case 3:
            if (r)
                return 5;
            return 1;
        case 4:
            return 0;
        case 5:
            if (l)
                return 3;
            return 7;
        case 6:
            return 2;
        case 7:
            if (l)
                return 1;
            return 5;
        default:
            fprintf(stderr, "moveball: bad direction\n");
    }
    return -1;
}

/* check if ball is touching paddle */
int overlap(struct piece *ball, struct player *p)
{
    int b_min_y = ball->y;
    int b_max_y = b_min_y+ball->h;
    int p_min_y = p->p->y;
    int p_max_y = p_min_y+p->p->h;

    int b_min_x = ball->x;
    int b_max_x = b_min_x+ball->w;
    int p_min_x = p->p->x;
    /* int p_max_x = p_min_x+p->p->w; */

    if (p_min_x-b_max_x == -1) {
        if (p_max_y >= b_min_y && p_min_y <= b_max_y)
            return 1; /* true */
    }

    return 0; /* false */
}

/* move ball(s) and computer player */
void movepieces(void)
{
    int i;
    int left, right, top, bottom;
    int ahit, bhit;
    struct piece *b = PIECE_PTR[0];

    int ball_y = (int)((float)b->y+((float)b->h/(float)2));
    int comp_y = (int)((float)PLAYER_B->p->y +
                 ((float)PLAYER_B->p->h/(float)2));
    int *comp_py = &PLAYER_B->p->y;
    /* move balls based on previous direction */
    for (i = 0; i < 1; i++) {
        b = PIECE_PTR[i];

        /* move computer player */
        if (ball_y < comp_y)
            *comp_py -= 1;
        else if (ball_y > comp_y)
            *comp_py += 1;
        if ((*comp_py) < 0)
            *comp_py = 0;
        else if ((*comp_py)+PLAYER_B->p->h > HEIGHT)
            *comp_py = HEIGHT-PLAYER_B->p->h;

        /* move ball */
        switch (b->d) {
            case -1:
                break;
            case 0:
                b->y = (b->y)-1; 
                break;
            case 1:
                b->x = (b->x)+1;
                b->y = (b->y)-1;
                break;
            case 2:
                b->x = (b->x)+1;
                break;
            case 3:
                b->x = (b->x)+1;
                b->y = (b->y)+1;
                break;
            case 4:
                b->y = (b->y)+1;
                break;
            case 5:
                b->x = (b->x)-1;
                b->y = (b->y)+1;
                break;
            case 6:
                b->x = (b->x)-1;
                break;
            case 7:
                b->x = (b->x)-1;
                b->y = (b->y)-1;
                break;
            default:
                fprintf(stderr, "movepieces: bad piece direction\n");
        }

        /* if ball hits border then deflect in appropriate direction */
        left = 0; right = 0; top = 0; bottom = 0;
        if (b->x < 0) {
            /* temp behavior until player input is accepted */
            if (b->d < 5) {
                b->x -= 1;
                b->d += 4;
            } else {
                b->x += 1;
                b->d -= 4;
            }
            /* PLAYER_B->points += 1; */
        }
        if (b->x+b->w > WIDTH) {
            b->x = 10; b->y = 0;
            b->d = -1; b->c = 88;
            PLAYER_A->points += 1;
        }
        if (b->y < 0) {
            top = 1;
            b->y += 1;
        }
        if (b->y+b->h > HEIGHT) {
            bottom = 1;
            b->y -= 1;
        }
        if (left || right || top || bottom)
            b->d = getdirection(b->d, left, right);

        /* check for ball paddle contact */
        ahit = overlap(b, PLAYER_A);
        bhit = overlap(b, PLAYER_B);
        if (ahit) {
            b->x += 1;
            b->d -= 4;
        } else if (bhit) {
            b->x -= 1;
            b->d += 4;
        }
    }
}

char getinput(void)
{
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

/* re-intialize the board */
void clear(void)
{
    int i = 0;
    int size = WIDTH*HEIGHT;
    short int c = 32;

    while (i < size) {
        if (BOARD->board[i] != c)
            BOARD->board[i] = c; /* only clear pieces? */
        i++;
    }
}

/* progress the game by one state */
void playgame(int delay)
{
    if (FRAME_C % delay == 0)
        movepieces();
    drawpieces();
}

/* write current board to stdout */
void refresh(void)
{
    int i;
    int size = WIDTH*HEIGHT;

    putc(13, stdout); /* in case user input exists */
    /* draw board up to HUD */
    for (i = 0; i < size-45; i++) {
        putc(BOARD->board[i], stdout);
    }
    /* user HUD */
    printf("PlayerA:%5i | PlayerB:%5i | Frame:%7lli",
           PLAYER_A->points,
           PLAYER_B->points,
           FRAME_C);
    /* flush HUD */
    fflush(stdout); /* unbuffering this might be faster? */
    FRAME_C++;
}

