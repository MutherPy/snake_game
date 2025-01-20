#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <termios.h>


#define FIELD_SIZE_ROWS 10
#define FIELD_SIZE_COLS 30

void draw(int x, int y){
    // TODO full-fill field
    char field[FIELD_SIZE_ROWS][FIELD_SIZE_COLS+1] = {{}, {}};

    // mixer();
    printf("Start\n");
    for(int i = 0; i < FIELD_SIZE_ROWS; i++){
        for(int j = 0; j < FIELD_SIZE_COLS; j++){
            field[i][j] = '=';
            if(i == y && j == x){
                field[i][j] = '#';
            }
            if(j == FIELD_SIZE_COLS-1){
                field[i][j+1] = '\0';
                continue;
            }
        }
    }
    printf("\n");
    for(int i = 0; i < FIELD_SIZE_ROWS; i++){
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

void position(int *x, int *y, char *x_dir, char *y_dir){
    switch (*x_dir)
    {
    case '-':
        (*x)--;
        break;
    case '+':
        (*x)++;
        break;
    }
    printf("first %i ", *y);
    switch (*y_dir)
    {
    case '-':
        (*y)--;
        printf(" inner %i ", *y);
        break;
    case '+':
        (*y)++;
        break;
    }
    printf("out %i ", *y);
    edge_control(x, y);

    printf("x - %i %c \n", *x+1, *x_dir);
    printf("y - %i %c \n", *y+1, *y_dir);
}


int x = (int)(FIELD_SIZE_COLS / 2);
int y = (int)(FIELD_SIZE_ROWS / 2);

char x_dir = '0';
char y_dir = '0';

void check_direction(char *current_direction_processing, char new_dir, char *other){
    char d = *current_direction_processing;
    if(new_dir == '+' && (d == '+' || d == '0')){
        *current_direction_processing = '+';
        *other = '0';
    }
    if(new_dir == '-' && (d == '-' || d == '0')){
        *current_direction_processing = '-';
        *other = '0';
    }
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
            check_direction(&y_dir, '-', &x_dir);
            break;
        case 's':
            check_direction(&y_dir, '+', &x_dir);
            break;
        case 'd':
            check_direction(&x_dir, '+', &y_dir);
            break;
        case 'a':
            check_direction(&x_dir, '-', &y_dir);
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

    while (1)
    {
        system("clear");
        position(&x, &y, &x_dir, &y_dir);
        draw(x, y);
        sleep(1);
    }
    return 0;
}
