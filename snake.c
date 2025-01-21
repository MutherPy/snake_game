#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>


#define FIELD_SIZE_ROWS 12
#define FIELD_SIZE_COLS 33

#define PLAY_FIELD_SIZE_ROWS 10
#define PLAY_FIELD_SIZE_COLS 30

#define PLUS '+'
#define MINUS '-'
#define STOP '0'

#define FILLER ' '
#define PLAYER_BLOCK '#'
#define FRUIT '@'
#define BORDER_LR '|'
#define BORDER_U '_'
#define BORDER_D '-'


int x = (int)(FIELD_SIZE_COLS / 2);
int y = (int)(FIELD_SIZE_ROWS / 2);

char x_dir_sign = STOP;
char y_dir_sign = MINUS;

char *p_x_dir_sign = &x_dir_sign;
char *p_y_dir_sign = &y_dir_sign;

int snake_size = 4;

int fruit_x = 0;
int fruit_y = 0;

bool fruit_eaten = true;


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


bool check_fruit_to_snake_coordinates(const int (*snake)[2], int possible_fr_x, int possible_fr_y){
    int good_count = 0;
    for(int i = 0;  i < snake_size; i++) {
        if((snake[i][0] != possible_fr_x) && (snake[i][1] != possible_fr_y)){
            good_count++;
        } else return false;
    }
    if(good_count == snake_size) return true;
    return false;
}

void handle_fruit_appearance(int *fr_x, int *fr_y, const int (*snake)[2]){
    if(!fruit_eaten) return;
    bool good = false;
    int r_x;
    int r_y;
    do {
        r_x = rand() % PLAY_FIELD_SIZE_COLS;
        if (r_x == 0 ) r_x++;
        r_y = rand() % PLAY_FIELD_SIZE_ROWS;
        if (r_y == 0) r_y++;
        good = check_fruit_to_snake_coordinates(snake, r_x, r_y);
    } while (!good);

    *fr_x = r_x;
    *fr_y = r_y;
    fruit_eaten = false;
}


void _draw_field_edge(int x, int y, char (*field)[FIELD_SIZE_COLS]){
    if(x == FIELD_SIZE_COLS-1){
        field[y][x+1] = '\0';
    }
}

bool _draw_borders(int x, int y, char (*field)[FIELD_SIZE_COLS]){
    bool result = false;
    if(y == 0){
        for(int i=0; i<FIELD_SIZE_COLS; i++) {
            field[y][i] = BORDER_U;
        }
        result = true;
    } else if (y == FIELD_SIZE_ROWS-1){
        for(int i=0; i<FIELD_SIZE_COLS; i++) {
            field[y][i] = BORDER_D;
        }
        result = true;
    }
    else if (x == 0 || x == FIELD_SIZE_COLS-1){
        field[y][x] = BORDER_LR;
        result = true;
    }
    _draw_field_edge(x, y, field);
    return result;
}

void _draw_filler(int x, int y, char (*field)[FIELD_SIZE_COLS]){
    field[y][x] = FILLER;
}

void _draw_snake(int x, int y, char (*field)[FIELD_SIZE_COLS], int (*snake)[2]){
    for(int k = 0; k < snake_size; k++){
        if(x == snake[k][0] && y == snake[k][1]){
            field[y][x] = PLAYER_BLOCK;
        }
    }
}

void _draw_fruit(int x, int y, int fr_x, int fr_y, char (*field)[FIELD_SIZE_COLS]){
    if (x == fr_x && y == fr_y){
        field[y][x] = FRUIT;
    }
}

void draw(char (*field)[FIELD_SIZE_COLS], int (*snake)[2], int fr_x, int fr_y){
    printf("Start\n");
    bool stop_draw = false;
    for(int i = 0; i < FIELD_SIZE_ROWS; i++){
        for(int j = 0; j < FIELD_SIZE_COLS; j++){
            stop_draw = _draw_borders(j, i, field);
            if (stop_draw) continue;
            _draw_filler(j, i, field);
            _draw_snake(j, i, field, snake);
            _draw_fruit(j, i, fr_x, fr_y, field);
            _draw_field_edge(j, i, field);
        }
        printf("%s\n", field[i]);
    }
}



void eaten_fruit_control(int x, int y, int fr_x, int fr_y, bool *fruit_eaten){
    if(x==fr_x && y == fr_y){
        *fruit_eaten = true;
    }
}

void edge_control(int *x, int *y){
    if(*x > PLAY_FIELD_SIZE_COLS+1){
        *x = PLAY_FIELD_SIZE_COLS+1;
    }
    else if (*x < 1) {
        *x = 1;
    }
    if(*y > PLAY_FIELD_SIZE_ROWS){
        *y = PLAY_FIELD_SIZE_ROWS;
    }
    else if (*y < 1) {
        *y = 1;
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
    eaten_fruit_control(*x, *y, fruit_x, fruit_y, &fruit_eaten);
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

    srand(time(NULL));

    char field[FIELD_SIZE_ROWS][FIELD_SIZE_COLS] = {{}, {}};

    int snake[4][2] = {{x,y}, {x, y+1}, {x, y+2}, {x, y+3}};

    while (1)
    {
        system("clear");
        position(&x, &y, p_x_dir_sign, p_y_dir_sign);
        handle_snake_move(x, y, snake);
        handle_fruit_appearance(&fruit_x, &fruit_y, snake);
        draw(field, snake, fruit_x, fruit_y);
        usleep(500000);
    }
    return 0;
}
