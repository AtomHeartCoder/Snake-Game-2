#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

/**************************
* Here below are the data *
**************************/

#define  MAX_HEIGHT  50
#define  MAX_WIDTH   80

typedef struct POSITION {
    int x;
    int y;
} POSITION;

POSITION snake_head, snake_tail, snake_front, food, award_food;
FILE *fmap = NULL;
char map[60][150];
int map_width, map_height, margin;
int lose, level = 3, score, award, award_score;
char control, direction;

/*******************************
* Here below are the functions *
*******************************/

void gotoxy(int x, int y);
void goprint(int x, int y, const char *c);
int map_print(FILE *f);
int snake_create(void);
void snake_control(void);
void snake_move(void);
void food_create(void);
void show_tips(void);
void show_score(void);
void progress_bar(void);
void quit(void);

// Show the map array's data to debug
void test(void)
{
    int i, j;
    for(i = 0; i < map_height; i++)
        for(j = 0; j < map_width; j++){
            gotoxy(margin + j, 18 + i);
            printf("%c ", map[i][j]);
        }
}

/*************************
* Here below is the body *
*************************/

// Function of setting console cursor position
void gotoxy(int x, int y)
{
    COORD pos = {2 * (x + 1), y + 1};                                /* Double the x coordinate to make the console characters have the same width as height */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}

// Function of setting console cursor position and print a string or a character
void goprint(int x, int y, const char *c)
{
    gotoxy(x, y);
    printf("%s", c);
}

// Show the menu and choose items
void welcome(void)
{
    lose = 0, score = 0, award = 0, award_food.x = -1, fmap = NULL;
    char option = '0', c;
    char map_choice;
    char map_path[80];
    while(1){
        goprint(4, 4, "Welcome to the Snake by Lst");
        goprint(6, 6, "Start by number:\n\n");
        printf("1. Quick game\n2. Map game\n3. Revise level\n4. Exit\n\n*** Please play in English Input Method ***\n");
        option = _getch();
        switch(option){
            case '1':
                fmap = fopen("map_0.txt", "r");
                break;
            case '2':
                system("cls");
                goprint(4, 4, "Welcome to the Snake by Lst");
                goprint(6, 6, "Choose by number:\n\n");
                printf("1. Cave\n2. Hell\n3. <EMPTY>\n4. Your own map\n5. Back");
                map_choice = _getch();
                switch(map_choice){
                    case '1':
                        fmap = fopen("map_1.txt", "r");
                        break;
                    case '2':
                        fmap = fopen("map_2.txt", "r");
                        break;
                    case '3':
                        fmap = fopen("map_3.txt", "r");
                        break;
                    case '4':
                        printf("\n\nInput your map's path:\n");
                        gets(map_path);
                        fmap = fopen(map_path, "r");
                        if(fmap == NULL){
                            system("cls");
                            goprint(10, 5, "Open fail! Or the map file doesn't exist!");
                            _getch();
                            system("cls");
                            option = '0';
                        }
                        break;
                    default:
                        option = '0';
                        break;
                }
                break;
            case '3':
                system("cls");
                goprint(4, 4, "Welcome to the Snake by Lst");
                goprint(6, 6, "Start by number:\n\n");
                printf("Present level: %d\n\nChoose level from 1 to 9 or... (Press Enter):", level);
                scanf("%d", &level);
                while((c = getchar()) != EOF && c != '\n');
                option = '0';
                break;
            case '4':
                exit(0);
            default:
                option = '0';
        }
        system("cls");
        if(option != '0'){
            if(map_print(fmap) != 0)
                option = '0';
            if(option != '0')
                break;
        }
    }
    fclose(fmap);
    margin = map_width + 5;
}

// This is to print the map
int map_print(FILE *fp)
{
    if(fp == NULL){
        goprint(10, 5, "Open fail! Or the map file doesn't exist!");
        _getch();
        system("cls");
        return 1;
    }
    char c;
    int i = 0, j = 0, max_j = 0;
    // Here below is to print blocks
    for(i = 0; (c = fgetc(fp)) != EOF; i++){
        ungetc(c, fp);
        // In case that the map is too big for the screen
        if(i >= MAX_HEIGHT){
            gotoxy(5, 5);
            printf("The map size exceeds the limit(%dw*%dh)!", MAX_WIDTH, MAX_HEIGHT);
            _getch();
            system("cls");
            return 2;
        }
        for(j = 0; (c = fgetc(fp)) != '\n' && c != EOF; j++){
            if(c == '1')
                map[i][j] = c;
            else
                map[i][j] = '0';
            if(j >= MAX_WIDTH){
                gotoxy(5, 5);
                printf("The map size exceeds the limit(%dw*%dh)!", MAX_WIDTH, MAX_HEIGHT);
                _getch();
                system("cls");
                return 2;
            }
        }
        if(j > max_j)
            max_j = j;
        if(c == EOF){
            i++;
            break;
        }
    }
    map_height = i, map_width = max_j;
    for(i = 0; i < map_height; i++){                       /* Flexibly recognize the map */
        for(j = 0; j < map_width; j++)
            if(map[i][j] != '0' && map[i][j] != '1')
                map[i][j] = '0';
        map[i][map_width] = '\0';
    }
    for(i = 0; i < map_height; i++)
        for(j = 0; j < map_width; j++)
            if(map[i][j] == '1')
                goprint(j, i, "¨€");
    // Here below is to print the boundary
    for(i = 0; i < map_height; i++){
        goprint(map_width, i, "©ª");
        goprint(-1, i, "©ª");
    }
    for(j = 0; j < map_width; j++){
        goprint(j, map_height, "©¨");
        goprint(j, -1, "©¨");
    }
    goprint(-1, -1, "©°");
    goprint(map_width, -1, "©´");
    goprint(-1, map_height, "©¸");
    goprint(map_width, map_height, "©¼");
    return 0;
}

// This is to echo the tips when playing the game
void show_tips(void)
{
    gotoxy(margin, 3);
    printf("Your score: %d", score);
    gotoxy(margin, 5);
    printf("Level: %d", level);
    gotoxy(margin, 7);
    printf("Map size: %d * %d", map_width, map_height);
    goprint(margin, 9, "Play with \"w/a/s/d\"");
    goprint(margin, 11, "Click to pause / \"p\" to quit");
}

// The function of creating food
void food_create(void)
{
    int sign = 0;
    do{
        if(sign)
            sign = 0;
        srand(rand());
        food.x = rand() % map_width;
        food.y = rand() % map_height;
        if(map[food.y][food.x] != '0'){                    /* Make sure the position is available */
            sign = 1;
            continue;
        }
        if(award == 5){                                    /* Create award food */
            award_food.x = rand() % map_width;
            award_food.y = rand() % map_height;
            if(map[award_food.y][award_food.x] != '0'){
                sign = 1;
                continue;
            }
        }
        if(award_food.x == food.x && award_food.y == food.y){
            sign = 1;
            continue;
        }
    }while(sign);
    goprint(food.x, food.y, "¡ñ");
    map[food.y][food.x] = '2';
    if(award == 5){
        goprint(award_food.x, award_food.y, "¡ï");
        map[award_food.y][award_food.x] = '3';
    }
}

// This is to create the snake at the beginning
int snake_create(void)
{
    int sign = 0, count = 0;
    int i, j, x, y, m = 0, n = 0;
    do{                                                    /* Avoid creating the snake on the blocks */
        count++;
        if(sign)
            sign = 0;
        srand(time(NULL));
        snake_head.x = rand() % map_width;
        snake_head.y = rand() % map_height;
        if(count >= 1000000){                              /* After 1000000 times of try randomly */
            while(map[n][m] != '0'){                       /* Directly look for available position */
                m++;
                if(m >= map_width){
                    m = 0;
                    n++;
                }
                if(n >= map_height){
                    goprint(map_width / 2 - 10, map_height / 2, "Your map isn't suitable to create the snake!");
                    _getch();
                    system("cls");
                    return 0;
                }
                snake_head.x = m, snake_head.y = n;
            }
        }
        if(map[snake_head.y][snake_head.x] != '0'){
            sign = 1;
            continue;
        }
        for(i = 0; i < 4; i++)                             /* Try every direction */
        {
            switch(i){
                case 0: direction = 'w', x = 0, y = 1; break;
                case 1: direction = 'd', x = -1, y = 0; break;
                case 2: direction = 'a', x = 1, y = 0; break;
                case 3: direction = 's', x = 0, y = -1; break;
            }
            // In case of exceeding the map limit array's limit
            if(snake_head.y + 5 * y < 0 || snake_head.x + 5 * x < 0 || snake_head.y + 5 * y > map_height || snake_head.x + 5 * x > map_width)
                sign = 1;
            else if(map[snake_head.y + y][snake_head.x + x] != '0' || map[snake_head.y + 2 * y][snake_head.x + 2 * x] != '0')
                sign = 1;
            else{
                if(sign)
                    sign = 0;
                for(j = 1; j < 5; j++)
                    if(map[snake_head.y - j * y][snake_head.x - j * x] != '0'){
                        sign = 1;
                        break;
                    }
                if(sign == 0)
                    map[snake_head.y][snake_head.x] = map[snake_head.y + y][snake_head.x + x] = map[snake_head.y + 2 * y][snake_head.x + 2 * x] = direction;
                break;
            }
        }
    }while(sign);
    snake_tail.x = snake_head.x + 2 * x;
    snake_tail.y = snake_head.y + 2 * y;
    for(i = 0; i < 6; i++){                                /* Let the snake flash to catch player's attention */
        goprint(snake_tail.x, snake_tail.y, "¡ö");
        goprint(snake_head.x + x, snake_head.y + y, "¡ö");
        goprint(snake_head.x, snake_head.y, "¡ö");
        if(i == 5)
            break;
        Sleep(80);
        goprint(snake_tail.x, snake_tail.y, "  ");
        goprint(snake_head.x + x, snake_head.y + y, "  ");
        goprint(snake_head.x, snake_head.y, "  ");
        Sleep(80);
    }
    return 1;
}

// This is to control the snake
void snake_control(void)
{
    while(!lose){
        Sleep(400 / level);
        if(_kbhit()){
			control = _getch();
			if(isupper(control))
                control = (char)tolower(control);
			if(control == 'p')
                break;
			if(control == 'w' || control == 's' || control == 'a' || control == 'd'){
                if(direction == 'w' && control == 's') ;   /* Avoid moving backward */
                else if(direction == 's' && control == 'w') ;
                else if(direction == 'a' && control == 'd') ;
                else if(direction == 'd' && control == 'a') ;
                else direction = control;
			}
        }
        snake_move();
    }
    if(control != 'p')
        goprint(snake_head.x, snake_head.y, "¡Á");
}

// This function let the snake move
void snake_move(void)
{
    int x = 0, y = 0;
    switch(direction){
        case 'w': x = 0, y = -1; break;
        case 's': x = 0, y = 1; break;
        case 'a': x = -1, y = 0; break;
        case 'd': x = 1, y = 0; break;
    }
    snake_front.x = snake_head.x + x;
    snake_front.y = snake_head.y + y;
    if(snake_front.x == map_width)                         /* Here we allow the snake to penetrate the boundary */
        snake_front.x = 0;
    else if(snake_front.x == -1)
        snake_front.x = map_width - 1;
    else if(snake_front.y == map_height)
        snake_front.y = 0;
    else if(snake_front.y == -1)
        snake_front.y = map_height - 1;
    switch(map[snake_front.y][snake_front.x]){
        case 'w':
            ;
        case 'a':
            ;
        case 's':
            ;
        case 'd':
            // This is the case that the front position is the tail, the tail and the head move in the sometime, the snake won't die
            if(snake_front.x == snake_tail.x && snake_front.y == snake_tail.y)
                ;
            else{
                lose = 1;
                return;
            }
        case '0':
            switch(map[snake_tail.y][snake_tail.x]){
                case 'w': x = 0, y = -1; break;
                case 's': x = 0, y = 1; break;
                case 'a': x = -1, y = 0; break;
                case 'd': x = 1, y = 0; break;
            }
            goprint(snake_tail.x, snake_tail.y, "  ");
            map[snake_tail.y][snake_tail.x] = '0';
            snake_tail.x += x, snake_tail.y += y;
            if(snake_tail.x == map_width)                  /* Here we allow the snake to penetrate the boundary */
                snake_tail.x = 0;
            else if(snake_tail.x == -1)
                snake_tail.x = map_width - 1;
            else if(snake_tail.y == map_height)
                snake_tail.y = 0;
            else if(snake_tail.y == -1)
                snake_tail.y = map_height - 1;
            break;
        case '2':
            award++;
            score += level;
            food_create();
            show_score();
            break;
        case '3':
            score += award_score / 2 * level;
            award_food.y = -1;
            show_score();
            break;
        default:
            lose = 1;
            return;
    }
    map[snake_head.y][snake_head.x] = direction;
    snake_head = snake_front;
    goprint(snake_head.x, snake_head.y, "¡ö");
    if(award_food.x != -1)
            progress_bar();
}

// Print the score after eating food
void show_score(void)
{
    gotoxy(margin, 3);
    printf("Your score: %d", score);
}

// Show and control the progress bar of the award food
void progress_bar(void)
{
    if(award == 5){
        goprint(margin, 13, "Award Food:");
        goprint(margin, 14, "¨€¨€¨€¨€¨€¨€¨€¨€¨€¨€");
        award_score = 100;
        award = 0;
    }
    if(award_food.y == -1){
        goprint(margin, 13, "           ");
        goprint(margin, 14, "                    ");
        award_food.x = -1;
    }
    goprint(margin + 1 + award_score-- / 10, 14, " ");
    if(award_score == 0){
        goprint(margin, 13, "           ");
        goprint(margin, 14, "                    ");
        goprint(award_food.x, award_food.y, "  ");
        map[award_food.y][award_food.x] = '0';
        award_food.x = -1, award_food.y = -1;
    }
}

// This will be called after ending the game
void quit(void)
{
    char name[30];
    int i, j;
    for(i = 0; i < 7; i++)
        for(j = 0; j < 14; j++)
            if(i == 0 || i == 6)
                goprint(map_width / 2 - 7 + j, map_height / 2 - 3 + i, "©¨");
            else if(j == 0 || j == 13)
                goprint(map_width / 2 - 7 + j, map_height / 2 - 3 + i, "©ª");
            else
                goprint(map_width / 2 - 7 + j, map_height / 2 - 3 + i, "  ");
    goprint(map_width / 2 - 7, map_height / 2 - 3, "©°");
    goprint(map_width / 2 + 6, map_height / 2 - 3, "©´");
    goprint(map_width / 2 - 7, map_height / 2 + 3, "©¸");
    goprint(map_width / 2 + 6, map_height / 2 + 3, "©¼");
    if(lose){
        system("color 01");
        Sleep(100);
        system("color 0F");
        goprint(map_width / 2 - 2, map_height / 2 - 2, "You lose!");
    }
    else
        goprint(map_width / 2 - 2, map_height / 2 - 2, "You quit!");
    gotoxy(map_width / 2 - 4, map_height / 2);
    printf("Your score: %5d", score);
    goprint(map_width / 2 - 6, map_height / 2 + 2, "Your name: ");
    gets(name);
    system("cls");
}

int main(void)
{
    while(1){
        welcome();
        show_tips();
        if(!snake_create())
            continue;
        food_create();
        control = _getch();
        if(isupper(control))
            control = (char)tolower(control);
        if(control == 'p'){
            quit();
            continue;
        }
        direction = map[snake_head.y][snake_head.x];
        if(control == 'w' || control == 's' || control == 'a' || control == 'd'){
            if(direction == 'w' && control == 's') ;
            else if(direction == 's' && control == 'w') ;
            else if(direction == 'a' && control == 'd') ;
            else if(direction == 'd' && control == 'a') ;
            else direction = control;
        }
        snake_control();
        quit();
    }
    return 0;
}
