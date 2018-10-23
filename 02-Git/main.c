#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    en_rock = 0,
    en_paper = 1,
    en_scissors = 2,
    en_max_state = 3
} en_state_t;


const char*  win_lut[en_max_state][en_max_state] = {
    /* player    |computer    rock,                                                      paper,                                                                            scissors   */
    /* rock     */ "> You choose rock, I choose rock too\n> Draw: rock and rock",         "> You choose rock, I choose paper \n> I win: paper beats rock",                 "> You choose rock, I choose scissors \n> You win: rock beats scissors",
    /* paper    */ "> You choose paper, I choose rock\n> You win: paper beats rock",      "> You choose paper, I choose paper too \n> Draw: paper and paper",              "> You choose paper, I choose scissors \n> I win: scissors beats paper",
    /* scissors */ "> You choose scissors, I choose rock\n> I win: rock beats scissors",  "> You choose scissors, I choose paper \n> You win: scissors beats paper",       "> You choose scissors, I choose scissors too \n> Draw: scissors and scissors"
};

int main(void)
{
    srand(time(NULL));

    do {
        int pc_choose = rand() % (int)en_max_state;

        printf("> Please choose: rock (r) - paper (p) - scissors (s)\n");

        char user_choose;
        scanf(" %c", &user_choose);

        switch(user_choose) {
        case'q':
            return 0;

        case'r':
            printf("%s\n", win_lut[en_rock][pc_choose]);
            continue;

        case'p':
            printf("%s\n", win_lut[en_paper][pc_choose]);
            continue;

        case's':
            printf("%s\n", win_lut[en_scissors][pc_choose]);
            continue;

        default:
            continue;
        }

    } while(1);
}
