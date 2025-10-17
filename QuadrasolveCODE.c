#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define ROWS 6
#define COLS 7

#include <stdbool.h>

#define SIZE 9
#define MAX_MOVES 100

int board[SIZE][SIZE];
int originalBoard[SIZE][SIZE];
int solution[SIZE][SIZE];

typedef struct {
    int row, col, prev, curr;
} Move;

Move moveHistory[MAX_MOVES];
int currentMoveIndex = -1;
int totalMoves = 0;

void fisherYatesShuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

bool isSafe(int row, int col, int n) {
    for (int i = 0; i < 9; i++)
        if (board[i][col] == n || board[row][i] == n)
            return false;

    for (int i = 0; i < SIZE; i++)
        if (board[3 * (row / 3) + (i / 3)][3 * (col / 3) + (i % 3)] == n)
            return false;

    return true;
}

bool solve(int arr[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (arr[i][j] == 0) {
                int nums[SIZE];
                for (int k = 0; k < SIZE; k++) nums[k] = k + 1;

                fisherYatesShuffle(nums, SIZE);

                for (int k = 0; k < SIZE; k++) {
                    int n = nums[k];
                    if (isSafe(i, j, n)) {
                        arr[i][j] = n;
                        if (solve(arr))
                            return true;
                        arr[i][j] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

int countSolutions(int row, int col, int *count) {
    if (row == SIZE - 1 && col == SIZE) {
        (*count)++;
        return *count > 1 ? 1 : 0;
    }
    if (col == SIZE) {
        row++;
        col = 0;
    }
    if (board[row][col] != 0)
        return countSolutions(row, col + 1, count);

    for (int num = 1; num <= SIZE; num++) {
        if (isSafe(row, col, num)) {
            board[row][col] = num;
            if (countSolutions(row, col + 1, count)) {
                board[row][col] = 0;
                return 1;
            }
            board[row][col] = 0;
        }
    }
    return 0;
}

void createPuzzle(int blanks) {
    int attempts = blanks;
    while (attempts > 0) {
        int row = rand() % SIZE, col = rand() % SIZE;
        while (board[row][col] == 0) {
            row = rand() % SIZE;
            col = rand() % SIZE;
        }

        int backup = board[row][col];
        board[row][col] = 0;

        int count = 0;
        countSolutions(0, 0, &count);

        if (count != 1)
            board[row][col] = backup;
        else
            attempts--;
    }
}

void printBoard1(int boardToPrint[SIZE][SIZE]) {
    printf("\n   ");
    for (int col = 0; col < SIZE; col++) {
        if (col % 3 == 0 && col != 0) printf(" | ");
        printf(" %d", col + 1);
    }
    printf("\n  +-------+-------+-------+\n");

    for (int row = 0; row < SIZE; row++) {
        printf("%2d |", row + 1);
        for (int col = 0; col < SIZE; col++) {
            if (boardToPrint[row][col] == 0)
                printf(" .");
            else
                printf(" %d", boardToPrint[row][col]);
            if ((col + 1) % 3 == 0 && col != SIZE - 1) printf(" |");
        }
        printf(" |\n");

        if ((row + 1) % 3 == 0 && row != SIZE - 1) printf("  +-------+-------+-------+\n");
    }

    printf("\n");
}

void saveGame1(const char *filename) {
    FILE *f = fopen(filename, "w");
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fprintf(f, "%d ", board[i][j]);
    fclose(f);
    printf("Game saved to %s\n", filename);
}

void loadGame1(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Failed to load game.\n"); return; }
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            fscanf(f, "%d", &board[i][j]);
    fclose(f);
    printf("Game loaded from %s\n", filename);
}

void makeMove(int row, int col, int prev, int curr) {
    if (currentMoveIndex < MAX_MOVES - 1) {
        currentMoveIndex++;
        moveHistory[currentMoveIndex] = (Move){row, col, prev, curr};
        totalMoves = currentMoveIndex + 1;
        board[row][col] = curr;
    }
}

void undoMove1() {
    if (currentMoveIndex >= 0) {
        Move m = moveHistory[currentMoveIndex];
        board[m.row][m.col] = m.prev;
        currentMoveIndex--;
        printf("Undo successful!\n");
    } else {
        printf("No moves to undo.\n");
    }
}

void redoMove() {
    if (currentMoveIndex < totalMoves - 1) {
        currentMoveIndex++;
        Move m = moveHistory[currentMoveIndex];
        board[m.row][m.col] = m.curr;
        printf("Redo successful!\n");
    } else {
        printf("No moves to redo.\n");
    }
}

void copyBoard1(int src[SIZE][SIZE], int dest[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            dest[i][j] = src[i][j];
}

void playGame1() {
    while (1) {
        printBoard1(board);
        printf("1. Enter move\n2. Undo\n3. Redo\n4. Save\n5. Load\n6. Exit\nChoice: ");
        int choice; scanf("%d", &choice);
        if (choice == 1) {
            int row, col, num;
            printf("Enter row, col, number (1-9): ");
            scanf("%d%d%d", &row, &col, &num);
            row--; col--;

            if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || num < 1 || num > 9) {
                printf("Invalid input range.\n");
                continue;
            }

            if (originalBoard[row][col] == 0) {
                if (solution[row][col] == num) {
                    makeMove(row, col, board[row][col], num);
                    printf("Move accepted!\n");

                    bool completed = true;
                    for (int i = 0; i < SIZE && completed; i++)
                        for (int j = 0; j < SIZE; j++)
                            if (board[i][j] == 0) {
                                completed = false;
                                break;
                            }

                    if (completed) {
                        printBoard1(board);
                        printf("\n  You win! You have completed the game successfully! \n");
                        break;
                    }
                } else {
                    printf("Incorrect move! Try again.\n");
                }
            } else {
                printf("Cannot change original puzzle values.\n");
            }
        } else if (choice == 2) undoMove1();
        else if (choice == 3) redoMove();
        else if (choice == 4) saveGame1("sudoku_save.txt");
        else if (choice == 5) loadGame1("sudoku_save.txt");
        else if (choice == 6) break;
        else printf("Invalid choice!\n");
    }
}


void initBoard(char board[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)  //loop for assigning '.'dots in all the squares in the board
            board[i][j] = '.';
}

void printBoard2(char board[ROWS][COLS]) {
    printf("\n  ");
    for (int j = 0; j < COLS; j++)
            printf (" %d", j);
       
    printf("\n");

    for (int i = 0; i < ROWS; i++) {
        printf(" %d", i);
        for (int j = 0; j < COLS; j++)
            printf(" %c", board[i][j]);
        printf("\n");
    }
    printf("\n");

}
int countConsecutive(char board[ROWS][COLS], int row, int col, int dRow, int dCol, char player, int count) {
    int newRow = row + dRow, newCol = col + dCol;
    if (newRow < 0 || newRow >= ROWS || newCol < 0 || newCol >= COLS)
        return count;
    if (board[newRow][newCol] != player)
        return count;
    return countConsecutive(board, newRow, newCol, dRow, dCol, player, count + 1);
}

int WINcheck(char board[ROWS][COLS], int row, int col, char player) {
    int count;
    // Horizontal
     count = 1;
     count += countConsecutive(board, row, col, 0, 1, player, 0);  
     count += countConsecutive(board, row, col, 0, -1, player, 0);  
     if (count >= 4) {
     return 1;
      }
     // Vertical
     count = 1;
     count += countConsecutive(board, row, col, 1, 0, player, 0);  
     count += countConsecutive(board, row, col, -1, 0, player, 0);
      if (count >= 4) {
      return 1;
       }
     // First Diagonal
     count = 1;
     count += countConsecutive(board, row, col, 1, 1, player, 0);  
     count += countConsecutive(board, row, col, -1, -1, player, 0);
     if (count >= 4) {
     return 1;
      }
     // Second Diagonal
     count = 1;
     count += countConsecutive(board, row, col, 1, -1, player, 0);  
     count += countConsecutive(board, row, col, -1, 1, player, 0);  
     if (count >= 4) {
     return 1;
      }

      return 0;
  }

int dropDisc(char board[ROWS][COLS], int col, char player) {
 if (col < 0 || col >= COLS){
  return -1;
 }
for (int row = ROWS - 1; row >= 0; row--) {
if (board[row][col] == '.') {
board[row][col] = player;
return row;
}
  }
  return -1;

}


void undoMove2(char currentBRD[ROWS][COLS], int row, int col) {
    //removes the latest move and replaces with '.' which was intially empty
    currentBRD[row][col] = '.';
    //end of loop
}

int isBoardFull(char boardfinal[ROWS][COLS]) {
    for (int column = 0; column < COLS; column++)
        if (boardfinal[0][column] == '.'){
        return 0;
        }   //checks if the board is filled ; reurns 0 if FULL else return 1;
    return 1;
}

//This function saves the game
void saveGame2(char board[ROWS][COLS], char currentPlayer, int moveHistory[][2], int moveCount) {
    FILE *fp = fopen("connect4_save.txt", "w");
    if (!fp) return;
    fprintf(fp, "%c\n%d\n", currentPlayer, moveCount);
    for (int i = 0; i < moveCount; i++)
        fprintf(fp, "%d %d\n", moveHistory[i][0], moveHistory[i][1]);
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            fputc(board[i][j], fp);
    fclose(fp);
    printf("Game saved!\n");
}

//loads the saved game
int loadGame2(char board[ROWS][COLS], char *currentPlayer, int moveHistory[][2], int *moveCount) {
    FILE *f = fopen("connect4_save.txt", "r");
    if (!f) return 0;
    fscanf(f, " %c\n%d\n", currentPlayer, moveCount);
    for (int i = 0; i < *moveCount; i++)
        fscanf(f, "%d %d\n", &moveHistory[i][0], &moveHistory[i][1]);
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            board[i][j] = fgetc(f);
    fclose(f);
    return 1;
}

void saveScore(const char *TitleHOLDER) {
    FILE *f = fopen("scores.txt", "a");
     //opening the file in append mode so that the latest scores get added to the existing file
    if (f==NULL) return;
    fprintf(f, "%s\n", TitleHOLDER);  //this will append the scores to the SCORES file
    fclose(f);
    //closes the file
}


 void copyBoard2(char destination[ROWS][COLS], char score[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++)  
        for (int j = 0; j < COLS; j++)  
            destination[i][j] = score[i][j]; //copying the pre-existing board before printing the Board (with new move)
}

int smartMove(char board[ROWS][COLS], char Computer, char human) {
    //loop to make the Computer win
    for (int col = 0; col < COLS; col++) {
        char temp[ROWS][COLS];
       //calling copyboard function
      copyBoard2(temp, board);
      //return the column number if entered move is valid && wincheck function detects the wins for Computer
        int row = dropDisc(temp, col, Computer);
        if (row != -1 && WINcheck(temp, row, col, Computer))
            return col;
    }
    // loop to block the user from winning
    for (int col = 0; col < COLS; col++) {
        char temp[ROWS][COLS];
         copyBoard2(temp, board);
        int row = dropDisc(temp, col, human);
     //return the column number if entered move is valid && wincheck function detects the wins for Human
        if (row != -1 && WINcheck(temp, row, col, human))
            return col;
    }
    //loop to place random move when both above conditions are not met
    int options[COLS];
    int count = 0;
    for (int col = 0; col < COLS; col++)
        if (board[0][col] == '.') options[count++] = col;
     
    if (count == 0) return -1;
    return options[rand() % count];
}
void playGame2(int mode, char board[ROWS][COLS], char currentPlayer, int moveHistory[][2], int moveCount, int isLoaded) {
    char winner[100];

    if (!isLoaded)
        initBoard(board);

    while (1) {
        printBoard2(board);
        int col;

        if (mode == 2 && currentPlayer == 'O') {
            col = smartMove(board, 'O', 'X');
            printf("Computer chooses column %d\n", col);
        } else {
            printf("Player %c, enter column (0-6), -1 to undo, -2 to save and quit: ", currentPlayer);
            scanf("%d", &col);

            if (col == -2) {
                saveGame2(board, currentPlayer, moveHistory, moveCount);
                break;
            }

            if (col == -1 && moveCount > 0) {
                if (mode == 2 && currentPlayer == 'X' && moveCount >= 2) {
                    int r1 = moveHistory[--moveCount][0], c1 = moveHistory[moveCount][1];
                    undoMove2(board, r1, c1);
                    int r2 = moveHistory[--moveCount][0], c2 = moveHistory[moveCount][1];
                    undoMove2(board, r2, c2);
                } else {
                    int r = moveHistory[--moveCount][0], c = moveHistory[moveCount][1];
                    undoMove2(board, r, c);
                }
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                continue;
            }
        }

        int row = dropDisc(board, col, currentPlayer);
        if (row == -1) {
            printf("Invalid move.\n");
            continue;
        }

        moveHistory[moveCount][0] = row;
        moveHistory[moveCount][1] = col;
        moveCount++;

        if (WINcheck(board, row, col, currentPlayer)) {
            printBoard2(board);
            printf("Player %c wins!\n", currentPlayer);
            sprintf(winner, "Player %c", currentPlayer);
            saveScore(winner);
            break;
        }

        if (isBoardFull(board)) {
            printBoard2(board);
            printf("Draw!\n");
            saveScore("Draw");
            break;
        }

        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
}

int SUDOKU(void){
    srand(time(NULL));
    solve(board);
    copyBoard1(board, solution);

    printf("Select difficulty (1-Easy, 2-Medium, 3-Hard): ");
    int diff; scanf("%d", &diff);
    int blanks = (diff == 1) ? 30 : (diff == 2) ? 40 : 50;
    createPuzzle(blanks);
    copyBoard1(board, originalBoard);

    playGame1();
    return 0;
}

int CONNECT4(void){
    srand(time(NULL));
    int choice;
    printf("CONNECT 4\n1. Player vs Player\n2. Player vs Computer\n3. Load Saved Game\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1 || choice==2) {
        char board[ROWS][COLS];
        int moveHistory[ROWS * COLS][2];
        playGame2(choice, board, 'X', moveHistory, 0, 0);  // new game
    } else if (choice == 3) {
        char board[ROWS][COLS], currentPlayer;
        int moveHistory[ROWS * COLS][2], moveCount;
        if (loadGame2(board, &currentPlayer, moveHistory, &moveCount)) {
            printf("Game loaded!\n");
            playGame2(choice,board,currentPlayer,moveHistory,moveCount,1);
        } else {
            printf("No saved game.\n");
        }

}

return 0;
}

int main()
{
    int select;
    printf("\n--- WELCOME TO QUADRASOLVE! ---\n");

        printf("1. Play Sudoku\n");
        printf("2. Play Connect 4\n");
        
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &select);
    


    if(select==1){
        SUDOKU();
    }
    if(select==2){
        CONNECT4();
    }
    else{
        exit ;
    }
}