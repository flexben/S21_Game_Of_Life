#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>

#define UPBORDER 0
#define D_BORDER 25
#define L_BORDER 0
#define R_BORDER 80

int* behavior(const int *oldGen);
void displayField(int *fieldData, int genCount);
int validate_input(int* speed, int max, int min);
void clear();
void cellStatus(int sumOfCells, int oldGen, int* newGen);
void genCountOutout(int genCount);
int getch();
int kbhit(void);

int main() {
    printf("Выберите карту для игры:\n");
    printf("1: chaos_field\n");
    printf("2: oscillator\n");
    printf("3: spaceship\n");
    printf("4: still_life\n");
    printf("5: train\n");
    printf("6: glider\n");
    int m = 25, n = 80;
    int **fieldData;
    int input;
    FILE * file;
    scanf("%d", &input);
    if (input == 1) {
        file = fopen("../datasets/chaos_field.txt", "rt");
    } else if (input == 2) {
        file = fopen("../datasets/oscillator.txt", "rt");
    } else if (input == 3) {
        file = fopen("../datasets/spaceship.txt", "rt");
    } else if (input == 4) {
        file = fopen("../datasets/still_life.txt", "rt");
    } else if (input == 5) {
        file = fopen("../datasets/train.txt", "rt");
    } else if (input == 6) {
        file = fopen("../datasets/glider.txt", "rt");
    }
    fieldData = malloc(m * sizeof(int*));
    for (int i = 0; i < m; i++) {
        fieldData[i] = malloc(n * sizeof(int));
    }
    if (input) {
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
            fscanf(file, "%d", &fieldData[i][j]);
            }
        }
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", fieldData[i][j]);
        }
        printf("\n");
    }
    int genCount = 0;
    int speed = 460000;
    int max = 1060000;
    int min = 10000;
    while (1) {
        if (validate_input(&speed, max, min) != 0) {
            int *nextData = behavior(&fieldData[0][0]);
            for (int i = 0; i < D_BORDER; i++) {
                for (int j = 0; j < R_BORDER; j++) {
                    fieldData[i][j] = nextData[i*R_BORDER + j];
                }
            }
            displayField(&fieldData[0][0], genCount);
            genCount++;
            usleep(speed);
        } else {
            for (int i = 0; i < m; i++) {
                free(fieldData[i]);
            }
            free(fieldData);
            clear();
            printf("Game stopped!");
            break;
        }
    }
    return 0;
}

int validate_input(int* speed, int max, int min) {
    if (kbhit() == 1) {
        char c;
        c = getch();
        if (c == 'a' && *speed > min) {
            *speed = *speed - 50000;
        } else if (c == 'z' && *speed < max) {
            *speed = *speed + 50000;
        } else if (c == 'q') {
            return 0;
        }
    }
    return 1;
}

void genCountOutout(int genCount) {
    char *gen = "Поколение №";
    printf("%20s%d", gen, genCount);
}

void aliveCells(const int *fieldData) {
    int aliveCells = 0;
    char* cells = "Живых клеток: ";
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            aliveCells+=fieldData[i*80 + j];
        }
    }
    printf("%40s%d\n", cells, aliveCells);
}

void displayField(int *fieldData, int genCount) {
    clear();
    printf("\t\t\t\tИГРА \"ЖИЗНЬ\"\n");
    long cell = 'o';
    char space = '.';
    genCountOutout(genCount);
    aliveCells(fieldData);
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            if (fieldData[i*80 + j] == 0) {
                printf("%c", space);
            } else if (fieldData[i*80 + j] == 1) {
                printf("%c", (char)cell);
            }
        }
        printf("\n");
    }
}

void clear() {
    printf("\033[0d\033[2J");
}

int* behavior(const int *oldGen1) {
    int** newGen = malloc(D_BORDER * R_BORDER * sizeof(int) + D_BORDER * sizeof(int));
    int* ptr = (int*)(newGen + D_BORDER);
    for (int i = 0; i < D_BORDER; i++) {
        newGen[i] = ptr + R_BORDER * i;
    }
    int oldGen[D_BORDER][R_BORDER];
    for (int i = 0; i < D_BORDER; i++) {
        for (int j = 0; j < R_BORDER; j++) {
            oldGen[i][j] = oldGen1[i*R_BORDER + j];
        }
    }
    for (int i = 0; i < D_BORDER; i++) {
        int sumOfCells = 0;
        for (int j = 0; j < R_BORDER; j++) {
            if (i == 0 && j == 0) {
                sumOfCells = oldGen[D_BORDER - 1][j] + oldGen[D_BORDER - 1][j+1] + oldGen[i][R_BORDER - 1]
                + oldGen[i][j+1] + oldGen[i+1][R_BORDER - 1] + oldGen[i+1][j] + oldGen[i+1][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (i == D_BORDER - 1 && j == 0) {
                sumOfCells = oldGen[i-1][R_BORDER - 1] + oldGen[i-1][j] + oldGen[i-1][j+1] +
                oldGen[i][R_BORDER - 1] + oldGen[i][j+1] + oldGen[UPBORDER][j] + oldGen[UPBORDER][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (i == 0 && j == R_BORDER - 1) {
                sumOfCells = oldGen[D_BORDER - 1][j-1] + oldGen[D_BORDER-1][j] +
                oldGen[i][j-1] + oldGen[i][L_BORDER] +
                    oldGen[i+1][j-1] + oldGen[i+1][j] + oldGen[i+1][L_BORDER];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (i == D_BORDER - 1 && j == R_BORDER - 1) {
                sumOfCells = oldGen[i-1][j-1] + oldGen[i-1][j] + oldGen[i-1][L_BORDER] +
                oldGen[i][j-1] + oldGen[i][L_BORDER] + oldGen[UPBORDER][j-1] + oldGen[UPBORDER][j];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (i == 0 && j != 0 && j != R_BORDER - 1) {
                sumOfCells = oldGen[D_BORDER - 1][j-1] + oldGen[D_BORDER - 1][j] + oldGen[D_BORDER- 1][j+1] +
                oldGen[i][j-1] + oldGen[i][j+1] + oldGen[i+1][j-1] + oldGen[i+1][j] + oldGen[i+1][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (i == D_BORDER - 1 && j != 0 && j != R_BORDER - 1) {
                sumOfCells = oldGen[i-1][j-1] + oldGen[i-1][j] + oldGen[i-1][j+1] +
                oldGen[i][j-1] + oldGen[i][j+1] +
                    oldGen[UPBORDER][j-1] + oldGen[UPBORDER][j] + oldGen[UPBORDER][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (j == 0 && i != 0 && i != D_BORDER - 1) {
                sumOfCells = oldGen[i-1][R_BORDER - 1] + oldGen[i-1][j] + oldGen[i-1][j+1] +
                oldGen[i][R_BORDER - 1] + oldGen[i][j+1] +
                    oldGen[i+1][R_BORDER - 1] + oldGen[i+1][j] + oldGen[i+1][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (j == R_BORDER - 1 && i != 0 && i != D_BORDER - 1) {
                sumOfCells = oldGen[i-1][j-1] + oldGen[i-1][j] + oldGen[i-1][L_BORDER] +
                oldGen[i][j-1] + oldGen[i][L_BORDER] +
                    oldGen[i+1][j-1] + oldGen[i+1][j] + oldGen[i+1][L_BORDER];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            } else if (j > 0 && i > 0 && i < 24 && j < 79) {
                sumOfCells = oldGen[i-1][j-1] + oldGen[i-1][j] + oldGen[i-1][j+1] +
                oldGen[i][j-1] + oldGen[i][j+1] + oldGen[i+1][j-1] + oldGen[i+1][j] + oldGen[i+1][j+1];
                cellStatus(sumOfCells, oldGen[i][j],  &newGen[i][j]);
            }
        }
    }
    free(newGen);
    return &newGen[0][0];
}


void cellStatus(int sumOfCells, int oldGen, int* newGen) {
    if (sumOfCells == 3) {
        *newGen = 1;
    } else if (sumOfCells > 3 || sumOfCells < 2) {
        *newGen = 0;
    } else {
        *newGen = oldGen;
    }
}

int kbhit(void) {
  struct termios oldt, newt;
  int ch;
  int oldf;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
      ungetc(ch, stdin);
      return 1;
  }
  return 0;
}

int getch() {
    int ch;
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
