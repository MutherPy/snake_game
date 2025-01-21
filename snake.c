#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>


#define FIELD_SIZE_ROWS 10
#define FIELD_SIZE_COLS 30

#define PLUS '+'
#define MINUS '-'
#define STOP '0'

#define FILLER '='
#define PLAYER_BLOCK '#'


int x = (int)(FIELD_SIZE_COLS / 2);
int y = (int)(FIELD_SIZE_ROWS / 2) - 2;

char x_dir_sign = STOP;
char y_dir_sign = STOP;

char *p_x_dir_sign = &x_dir_sign;
char *p_y_dir_sign = &y_dir_sign;

void handle_snake_move(int x, int y, int (*snake)[2]){
    if(x == snake[0][0] && y == snake[0][1]){
        return;
    }
    for(int i = 3; i >= 0; i--){
        if(i > 0){
            snake[i][0] = snake[i-1][0];
            snake[i][1] = snake[i-1][1];
        } else {
            snake[0][0] = x;
            snake[0][1] = y;
        }
    }
}

void draw(char (*field)[FIELD_SIZE_COLS+1], int (*snake)[2]){
    printf("Start\n");
    for(int i = 0; i < FIELD_SIZE_ROWS; i++){
        for(int j = 0; j < FIELD_SIZE_COLS; j++){
            field[i][j] = FILLER;
            for(int k = 0; k < 4; k++){
                if(i == snake[k][1] && j == snake[k][0]){
                    field[i][j] = PLAYER_BLOCK;
                }
                if(j == FIELD_SIZE_COLS-1){
                    field[i][j+1] = '\0';
                    continue;
                }
            }
        }
        printf("%s\n", field[i]);
    }
}

void edge_control(int *x, int *y){
    if(*x > FIELD_SIZE_COLS-1){
        *x = FIELD_SIZE_COLS-1;
    }
    else if (*x < 0) {
        *x = 0;
    }
    if(*y > FIELD_SIZE_ROWS-1){
        *y = FIELD_SIZE_ROWS-1;
    }
    else if (*y < 0) {
        *y = 0;
    }
}

void position(int *x, int *y, char *x_dir_sign, char *y_dir_sign){
    switch (*x_dir_sign)
    {
    case MINUS:
        (*x)--;
        break;
    case PLUS:
        (*x)++;
        break;
    }
    switch (*y_dir_sign)
    {
    case MINUS:
        (*y)--;
        break;
    case PLUS:
        (*y)++;
        break;
    }
    edge_control(x, y);
}

void check_direction(char *current_direction_sign, char new_direction_sign, char *other_direction_sign){
    if ((new_direction_sign == *current_direction_sign) || *current_direction_sign == STOP) {
        *current_direction_sign = new_direction_sign;
        *other_direction_sign = STOP;
    }
    // description
//    if(new_direction_sign == '+' && (c_d_sign == '+' || c_d_sign == '0')){
//        *current_direction_sign = '+';
//        *other_direction_sign = '0';
//    }
//    if(new_direction_sign == '-' && (c_d_sign == '-' || c_d_sign == '0')){
//        *current_direction_sign = '-';
//        *other_direction_sign = '0';
//    }
}

// method for receiving keys from keyboard
_Noreturn void *keyboard_reader(void *vargp){
    static char key;
    struct termios info;
    tcgetattr(0, &info);          /* get current terminal attirbutes; 0 is the file descriptor for stdin */
    info.c_lflag &= ~ICANON;      /* disable canonical mode */
    info.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
    info.c_cc[VTIME] = 0;         /* no timeout */
    tcsetattr(0, TCSANOW, &info); /* set immediately */
    while (1)
    {
        key = (char)getchar();
        switch (key){
        case 'w':
            check_direction(p_y_dir_sign, MINUS, p_x_dir_sign);
            break;
        case 's':
            check_direction(p_y_dir_sign, PLUS, p_x_dir_sign);
            break;
        case 'd':
            check_direction(p_x_dir_sign, PLUS, p_y_dir_sign);
            break;
        case 'a':
            check_direction(p_x_dir_sign, MINUS, p_y_dir_sign);
            break;
        default:
            continue;
        }
    }
}

int main(){

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, keyboard_reader, NULL);
    // pthread_join(thread_id, NULL);

    char field[FIELD_SIZE_ROWS][FIELD_SIZE_COLS+1] = {{}, {}};

    int snake[4][2] = {{x,y}, {x, y+1}, {x, y+2}, {x, y+3}};

    while (1)
    {
        system("clear");
        position(&x, &y, p_x_dir_sign, p_y_dir_sign);
        handle_snake_move(x, y, snake);
        draw(field, snake);
        sleep(1);
    }
    return 0;
}
