#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void modify(char board[8][8], char intialP[], char targetP[]);
void print_board(char board[8][8], char removed[], int turn);
void ask_for_input(int playerNo);
char piece_type(char position[], char board[8][8]);
char piece_team(char piece);
int get_input(char board[8][8], char moves[1000][4], int turn, char intialP[], char targetP[]);
int is_legalmove(char piece, char intialP[], char targetP[], char board[8][8], int first);
int is_check(char board[8][8], char kingPosition[], char team, char checkPieceP[]);
int is_checkmate(char board[8][8], int checks, char kingPosition[], char checkPosition[]);
int is_stalemate(char board[8][8], char team);
void undo(char board[8][8], char moves[1000][4], int turn, char removed[]);

int main()
{
    char moves[1000][4];
    char unitsRemoved[1000] = {0};
    char undone[1000][4];

    int playerNo = 1;
    int turn = 0;
    int kingChecked = 0;
    int undoCounter = 0;
    int redoCounter = 0;

    char checkPieceP[2];
    char intialP[2];
    char targetP[2];
    char whiteKingPosition[3] = {'E', '1', '\0'};
    char blackKingPosition[3] = {'E', '8', '\0'};

    char promotion[] = {'b','n','q','r'};
    FILE *fp;

    char board[8][8] = {{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'},
                        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                        {'-', '.', '-', '.', '-', '.', '-', '.'},
                        {'.', '-', '.', '-', '.', '-', '.', '-'},
                        {'-', '.', '-', '.', '-', '.', '-', '.'},
                        {'.', '-', '.', '-', '.', '-', '.', '-'},
                        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}};
    print_board(board, unitsRemoved, turn);


    //Our fixed update loop
    while (1)
    {
        ask_for_input(playerNo); // Asks the user for input and prints the available commands

        enum inputStates{Failed, Move, Save, Load, Undo, Redo} input;
        input = get_input(board, moves, turn, intialP, targetP); // Get the user input

        if(input == Failed) // If its invalid , ask for input again
        {
            continue;
        }
        else if (input == Save)
        {
            fp = fopen("save.txt", "w");
            fprintf(fp, "%.64s %.2s %.2s %d %.1000s", &board, whiteKingPosition, blackKingPosition, turn, &unitsRemoved);
            fclose(fp);
            printf("File has been saved!");
            continue;
        }
        else if (input == Load)
        {
            fp = fopen("save.txt", "r");
            fscanf(fp,"%s %s %s %d %s",board ,&whiteKingPosition, &blackKingPosition, &turn, unitsRemoved);
            fclose(fp);
            print_board(board, unitsRemoved, turn);
           
            playerNo = turn % 2 + 1;
            continue;
        }
        else if(input == Undo)
        {
            if (turn > 0)
            {
                turn -= 1;
                for (int i = 0; i < 4; i++)
                {
                    undone[undoCounter][i] = moves[turn][i];
                }

                undoCounter += 1;
                playerNo = turn % 2 + 1;
                
                undo(board, moves, turn, unitsRemoved);
                print_board(board, unitsRemoved, turn - 1);
                continue;
            }
            else
            {
                printf("No moves were made to undo");
                continue;
            }
        }
        else if (input == Redo && redoCounter < undoCounter)
        { 
            redoCounter += 1;
            char todoP1[2], todoP2[2];

            strncpy(todoP1, undone[undoCounter - redoCounter], 2);
            strncpy(todoP2, undone[undoCounter - redoCounter] + 2, 2);
            strncpy(moves[turn], undone[undoCounter - redoCounter], 4);

            for (int i = 0; i < 2; i++)
            {
                todoP1[i] = toupper(todoP1[i]);
                todoP2[i] = toupper(todoP2[i]);
            }

            turn += 1;
            playerNo = turn % 2 + 1;
            modify(board, todoP1, todoP2);
            print_board(board, unitsRemoved, turn);
            continue;
        }
        else if (input == Move) // if input is a move
        {
            // Check the validity of the move depending on each piece
            enum MoveState{illegal, legal, Promotion} move;
            move = is_legalmove(piece_type(intialP, board), intialP, targetP, board, 1);

            if (move == legal)
            {
                if (piece_team(piece_type(intialP, board)) == 'w') // if its white's turn
                {
                    // if the king is the piece being moved update the king variable
                    if (piece_type(intialP,board) == 'k')
                    {
                        whiteKingPosition[0] = targetP[0];
                        whiteKingPosition[1] = targetP[1];
                    }

                    //If it is valid, adds the target to the removedUnits array
                    unitsRemoved[turn] = board['8' - targetP[1]][targetP[0] - 'A'];

                    modify(board, intialP, targetP);
                    print_board(board, unitsRemoved, turn);

                    kingChecked = is_check(board, blackKingPosition, 'w', checkPieceP);
                    // if a check happens
                    // check if the game is over
                    if (kingChecked > 0)
                    {
                        if (is_checkmate(board, kingChecked, blackKingPosition, checkPieceP) == 1)
                        {
                            printf("\nCheckmate!");
                            printf("\nPlayer %i won\n", playerNo);
                            system("pause");
                            break;
                        }
                        printf("\nThe king is in under threat!");
                    }
                    // a stalemate happens when a player has no legal moves but the king is not in check
                    else     // if the king is not in check
                    { 
                        if (is_stalemate(board, 'b') == 1)
                        {
                            printf("\nStalemate!\n");
                            system("pause");
                            break;
                        }
                    }
                    

                }
                else // black's turn
                {
                    if (piece_type(intialP,board) == 'K')
                    {
                        blackKingPosition[0] = targetP[0];
                        blackKingPosition[1] = targetP[1];
                    }

                    unitsRemoved[turn] = board['8' - targetP[1]][targetP[0] - 'A'];
                    modify(board, intialP, targetP);
                    
                    print_board(board, unitsRemoved, turn);
                    kingChecked = is_check(board, whiteKingPosition, 'b', checkPieceP);

                    if (kingChecked > 0)
                    {
                        if (is_checkmate(board, kingChecked, whiteKingPosition, checkPieceP) == 1) 
                        {
                            printf("\nCheckmate!");
                            printf("\nPlayer %i won\n", playerNo);
                            system("pause");
                            break;
                        }
                        printf("\nThe king is in under threat!");
                    }
                    else
                    {
                        if (is_stalemate(board, 'b') == 1)
                        {
                            printf("\nStalemate!\n");
                            system("pause");
                            break;
                        }
                    }
                }
                
            }
            else if (move == Promotion)
            {
                while (1)
                {
                    modify(board, intialP, targetP);
                    printf("Insert the promotion!\n");
                    scanf("%c", &board['8' - targetP[1]][targetP[0] - 'A']);
                    for (int i = 0; i < 4; i++)
                    {
                        if (board['8' - targetP[1]][targetP[0] - 'A'] == promotion[i])
                        {
                            goto BREAK;
                        }
                    }
                    printf("Please insert a valid input\n");
                    fflush(stdin);
                    continue;
                    BREAK:print_board(board, unitsRemoved, turn); 
                    break;            
                }     
            }
            //If it is invalid, ask the user for another input
            else if(move == illegal)
            {
                printf("\nPlease insert a valid position");
                fflush(stdin);
                continue;
            }
        }
    
        turn++;
        playerNo = turn % 2 + 1; 
        fflush(stdin);
    }
}

void print_board(char board[8][8], char removed[], int turn)
{
    system("cls");
    for (int i = 0; i < 8; i++)
    {
        printf("%d       ", 8 - i);
        for (int j = 0; j < 8; j++)
        {
            printf("%c    ", board[i][j]);
        }
        printf("\n\n");
    }

    printf("\n        A    B    C    D    E    F    G    H");
    printf("\n\nLost units (1): ");
    for (int i = 1; i <= turn || removed[i] != '\0'; i++)
    {
        if (isalpha(removed[i]) != 0)
        {
            if (islower(removed[i]) != 0)
            {
                printf("%c  ", removed[i]);
            }
        }   
    }

    printf("\nLost units (2): ");
    for (int i = 1; i <= turn || removed[i] != '\0'; i++)
    {
        if (isalpha(removed[i]) != 0)
        {
            if (isupper(removed[i]) != 0)
            {
                printf("%c  ", removed[i]);
            }
        }     
    }
    printf("\n");
}

void modify(char board[8][8], char intialP[], char targetP[])
{
    char temp = board['8' - intialP[1]][intialP[0] - 'A'];
    board['8' - targetP[1]][targetP[0] - 'A'] = temp;
    if(('8' - intialP[1]) % 2 == 0)
    {
        if((intialP[0] - 'A') % 2 == 0)
        {
            board['8' - intialP[1]][intialP[0] - 'A'] = '-';
        }
        else
        {
            board['8' - intialP[1]][intialP[0] - 'A'] = '.';
        }
    }
    else if(('8' - intialP[1]) % 2 == 1)
    {
        if((intialP[0] - 'A') % 2 == 0)
        {
            board['8' - intialP[1]][intialP[0] - 'A'] = '.';
        }
        else
        {
            board['8' - intialP[1]][intialP[0] - 'A'] = '-';
        }
    }
}

void ask_for_input(int playerNo)
{
    // Prints available commands
    printf("\nOther Commands:\n");
    printf("\"undo\", \"redo\", \"save\", \"load\"\n");

    // Prints the player number
    printf("\nPlayer %d:\n", playerNo);
}

void lower_string(char string[])
{
    // make the string letters all lowercase
    for (int i = 0; string[i] != ' '; i++)
    {
        string[i] = tolower(string[i]); 
    }
}

int get_input(char board[8][8], char moves[1000][4], int turn, char intialP[], char targetP[])
{
    // Get the input
    if (fgets(moves[turn], 6, stdin) == NULL)  // Checks if the input is within the boundry 
    {
        printf("\nPlease insert a valid position");
        fflush(stdin);
        return 0;
    }
    
    lower_string(moves[turn]); // Make it not case sensitive

    // Check if the input is a command not a move
    if (strncmp(moves[turn], "save", 4) == 0)
    {
        return 2;
    }
    else if(strncmp(moves[turn], "load", 4) == 0)
    {
        return 3;
    }
    else if(strncmp(moves[turn], "undo", 4) == 0)
    {
        return 4;
    }
    else if(strncmp(moves[turn], "redo", 4) == 0)
    {
        return 5;
    }

    // Separate the input
    strncpy(intialP, moves[turn], 2);
    strncpy(targetP, moves[turn] + 2, 2);

    // Turns the moves into upper case before using them as parameters
    for (int i = 0; i < 2; i++)
    {
        intialP[i] = toupper(intialP[i]);
        targetP[i] = toupper(targetP[i]);
    }

    // Make sure that the input is valid
    if (intialP[0] < 'A' || intialP[0] > 'H' || 
        intialP[1] < '1' || intialP[1] > '8' || 
        targetP[0] < 'A' || targetP[0] > 'H' || 
        targetP[1] < '1' || targetP[1] > '8' )
    {
        printf("\nPlease insert a valid position");
        fflush(stdin);
        return 0;
    }

    //Make sure that the selected position is not empty
    if (piece_type(intialP, board) == 'e')
    {
        printf("\nThe selected tile is empty");
        fflush(stdin);
        return 0;
    }

    //Makes sure that every player uses his own pieces
    if (turn % 2 == 0 && islower(board['8' - intialP[1]][toupper(intialP[0]) - 'A']) == 0)
    {
        printf("\nPlease pick a piece from your side");
        fflush(stdin);
        return 0;
    }
    else if(turn % 2 == 1 && isupper(board['8' - intialP[1]][toupper(intialP[0]) - 'A']) == 0)
    {
        printf("\nPlease pick a piece from your side");
        fflush(stdin);
        return 0;
    }


    return 1;
}

char piece_type(char position[], char board[8][8])
{
    //check if its empty
    if (board['8' - position[1]][position[0] - 'A'] == '-' || board['8' - position[1]][position[0] - 'A'] == '.')
    {
        return 'e';
    }
    //otherwise return the piece
    return board['8' - position[1]][position[0] - 'A'];
}

char piece_team(char piece)
{
    if (piece == 'e')
    {
        return 'e';
    }
    
    // if the piece is small letter >> white team
    if (piece >= 'a' && piece <= 'z')
    {
        return 'w';
    }
    else
    {
        return 'b';
    }
}

int positive(int num)
{
    if (num < 0)
    {
        return num*-1;
    }
    else
    {
        return num;
    }
}

int sign(int x)
{
    if(x > 0) return 1;
    if(x < 0) return -1;
    return 0;
}

int is_legalmove(char piece, char intialP[], char targetP[], char board[8][8], int testKingCheck)
{
    // check if the move will result in the player's king to be checked 
    if (testKingCheck == 1 || testKingCheck == 2)
    {
        char checkPieceP[2];
        char kingP[2];
        
        char tmpgrid[8][8] ;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                tmpgrid[i][j] = board[i][j];
            }
        }
        modify(tmpgrid, intialP, targetP); //tmpgrid is a copy of what the board would look like if the move was made
        
        if (piece_team(piece) == 'w')
        {
            // get the position of the king
            for (int i = 'A'; i <= 'H'; i++)
            {
                for (int j = '1'; j <= '8'; j++)
                {   
                    char curPosition[2]; curPosition[0] = i; curPosition[1] = j; 
                    if (piece_type(curPosition, tmpgrid) == 'k')
                    {
                        kingP[0] = i; 
                        kingP[1] = j;
                        break;
                    }
                }
            }

            if (is_check(tmpgrid, kingP, 'b', checkPieceP) > 0)
            {
                if(testKingCheck != 2)
                {
                    printf("\nKing will be threatened by: %c%c", checkPieceP[0], checkPieceP[1]);
                }
                return 0;
            }
            
        }
        else if (piece_team(piece) == 'b')
        {
            // get the position of the king
            for (int i = 'A'; i <= 'H'; i++)
            {
                for (int j = '1'; j <= '8'; j++)
                {   
                    char curPosition[2]; curPosition[0] = i; curPosition[1] = j; 
                    if (piece_type(curPosition, tmpgrid) == 'K')
                    {
                        kingP[0] = i; 
                        kingP[1] = j;
                        break;
                    }
                }
            }

            if (is_check(tmpgrid, kingP, 'w', checkPieceP) > 0)
            {
                if(testKingCheck != 2)
                {
                    printf("\nKing will be threatened by: %c%c", checkPieceP[0], checkPieceP[1]);
                }
                return 0;
            }
        }
    }
    
    switch (piece)
    {
        case 'r':
        case 'R':
        {
            if (intialP[0] == targetP[0]) // vertical move >> num changes
            {
                //check if its blocked
                //make sure every tile is empty till we get to the final position
                for (int i = 1; i < positive(intialP[1] - targetP[1]); i++) //skip the first and last one
                {
                    char tempPosition[] = {intialP[0], intialP[1] + (i* -1 * sign(intialP[1]-targetP[1]) )};
                    if (piece_type(tempPosition, board) != 'e')
                    {
                        return 0;
                    }
                }
                //check if the last one is not empty 
                //and whether its the oppents piece or not
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                return 1;
            }
            else if(intialP[1] == targetP[1]) // horizontal move >> letter changes
            {
                for (int i = 1; i < positive(intialP[0] - targetP[0]); i++)
                {
                    char tempPosition[] = {intialP[0] + (i* -1 * sign(intialP[0]-targetP[0])), intialP[1]};
                    if (piece_type(tempPosition, board) != 'e')
                    {
                        return 0;
                    }
                }
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                return 1;
            }
            return 0;
        }

        case 'n':
        case 'N':
        {
            /* a knight can move 
            1 vertical, 2 horizontal
            2 vertical, 1 horizontal
            so the sum should always = 3
            */
            int sum = positive(intialP[0] - targetP[0]) + positive(intialP[1] - targetP[1]);
            if (sum == 3 && intialP[0] != targetP[0] && intialP[1] != targetP[1])
            {
                // check if the target position has a piece and whether its on the same team or not
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                // otherwise if its empty return 1
                return 1;
            }
            return 0;
        }

        case 'b':
        case 'B':
        {
            int charDiff = intialP[0] - targetP[0];
            int numDiff = intialP[1] - targetP[1];

            // check if they difference is the same
            if (positive(numDiff) == positive(charDiff))
            {
                // check every diagonal tile til the target
                for (int i = 1; i < positive(numDiff); i++)
                {
                    // check if any tile on the way is not empty
                    char currentTile[] = {intialP[0] + i * (-1 * sign(charDiff)) , intialP[1] + i * (-1 * sign(numDiff))};
                    if (piece_type(currentTile, board) != 'e')
                    {
                        return 0;
                    }
                }
                // check the last tile (target)
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                return 1;
            }
            return 0;
        }
        
        case 'q':
        case 'Q':
        {
            // check if its a diagonal move
            int charDiff = intialP[0] - targetP[0];
            int numDiff = intialP[1] - targetP[1];
            if (positive(numDiff) == positive(charDiff))
            {
                for (int i = 1; i < positive(numDiff); i++)
                {
                    char currentTile[] = {intialP[0] + i * (-1 * sign(charDiff)) , intialP[1] + i * (-1 * sign(numDiff))};
                    if (piece_type(currentTile, board) != 'e')
                    {
                        return 0;
                    }
                }
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                return 1;
            }
            else if (intialP[0] == targetP[0] || intialP[1] == targetP[1])
            {
                if (intialP[0] == targetP[0]) // vertical move >> num changes
                {
                    //check if its blocked
                    //make sure every tile is empty till we get to the final position
                    for (int i = 1; i < positive(intialP[1] - targetP[1]); i++) //skip the first and last one
                    {
                        char tempPosition[] = {intialP[0],intialP[1] + (i* -1 * sign(intialP[1]-targetP[1])) };
                        if (piece_type(tempPosition, board) != 'e')
                        {
                            return 0;
                        }
                    }
                    //check if the last one is not empty 
                    //and whether its the oppents piece or not
                    if (piece_type(targetP, board) != 'e')
                    {
                        if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                        {
                            return 0;
                        }
                    }
                    return 1;
                }
                else if(intialP[1] == targetP[1]) // horizontal move >> letter changes
                {
                    for (int i = 1; i < positive(intialP[0] - targetP[0]); i++)
                    {
                        char tempPosition[] = {intialP[0] + (i* -1 * sign(intialP[0]-targetP[0])), intialP[1]};
                        if (piece_type(tempPosition, board) != 'e')
                        {
                            return 0;
                        }
                    }
                    if (piece_type(targetP, board) != 'e')
                    {
                        if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                        {
                            return 0;
                        }
                    }
                    return 1;
                }
            }
            else
            {
                return 0;
            }
        }

        case 'k':
        case 'K':
        {
            int sum = positive(intialP[0] - targetP[0]) + positive(intialP[1] - targetP[1]);
            if (sum == 1)
            {
                if (piece_type(targetP, board) != 'e')
                {
                    if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                    {
                        return 0;
                    }
                }
                return 1;
            }
            else if(sum == 2) // 1 diagonal move
            {
                // check if its 2 moves on the same line
                if (intialP[0] == targetP[0] || intialP[1] == targetP[1])
                {
                    return 0;
                }
                else // 1 diagonal
                {
                    if (piece_type(targetP, board) != 'e')
                    {
                        if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                        {
                            return 0;
                        }
                    }
                    return 1;
                }
            }
            else
            {
                return 0;
            }
        }

        /*
            pawns can only move forwad once or twice if its the pawn's first move
            pawns cant move forward if there is a piece blocking it (whether its 1 or 2 moves)
            pawns can take a piece 1 forward diagonal move only 
        */

        case 'p':
        {
            //check if the move is horizontal
            if (intialP[0] != targetP[0])
            {
                //check if its only 1 move diagonal
                if (targetP[1] - intialP[1] == 1 && positive(intialP[0] - targetP[0]) == 1)
                {
                    // if its, check if takes a piece (check if the tile is not empty)
                    if (piece_type(targetP, board) != 'e')
                    {
                        if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                        {
                            return 0;
                        }
                        return 1;
                    }
                }
                return 0;
            }
            else // the move is only vertical
            {
                // check if its 1 move
                if (targetP[1] - intialP[1] == 1)
                {
                    // check if its not blocked
                    if (piece_type(targetP, board) == 'e')
                    {
                        if (targetP[1] == '8')
                        {
                            return 3;
                        }
                        return 1;
                    }
                    return 0;
                }
                // check if its 2 and its the pawn's first move
                else if (targetP[1] - intialP[1] == 2 && intialP[1] == '2')
                {
                    //then 2 moves are allowed if its not blocked
                    char firstTile[] = {targetP[0], '3'};
                    char secondTile[] = {targetP[0], '4'};

                    if (piece_type(firstTile, board) == 'e' && piece_type(secondTile, board) == 'e')
                    {
                        return 1;
                    }
                    return 0;
                }
                // more than 1 or 2 moves
                return 0;
            }
            break;
        }

        case 'P':
        {
            //check if the move is horizontal
            if (intialP[0] != targetP[0])
            {
                //check if its only 1 move diagonal
                if (intialP[1] - targetP[1] == 1 && positive(intialP[0] - targetP[0]) == 1)
                {
                    // if its, check if takes a piece (check if the tile is not empty)
                    if (piece_type(targetP, board) != 'e')
                    {
                        if (piece_team(piece_type(targetP, board)) == piece_team(piece_type(intialP, board)) )
                        {
                            return 0;
                        }
                        return 1;
                    }
                }
                return 0;
            }
            else // the move is only vertical
            {
                // check if its 1 move
                if (intialP[1] - targetP[1] == 1 )
                {
                    // check if its not blocked
                    if (piece_type(targetP, board) == 'e')
                    {
                        if (targetP[1] == '1')
                        {
                            return 3;
                        }
                        return 1;
                    }
                    return 0;
                }
                // check if its 2 and its the pawn's first move
                else if (intialP[1] - targetP[1] == 2 && intialP[1] == '7')
                {
                    //then 2 moves are allowed if its not blocked
                    char firstTile[] = {targetP[0], '6'};
                    char secondTile[] = {targetP[0], '5'};

                    if (piece_type(firstTile, board) == 'e' && piece_type(secondTile, board) == 'e')
                    {
                        return 1;
                    }
                    return 0;
                }
                // more than 1 or 2 moves
                return 0;
            }
            break;
        }
    }
}

int is_check(char board[8][8], char kingPosition[], char attackingTeam, char checkPieceP[])
{
    int checks = 0;
    // check if there is any check on the king
    for (int i = 'A'; i <= 'H'; i++)
    {
        for (int j = '1'; j <= '8'; j++)
        {
            char curPosition[3]; curPosition[0] = i; curPosition[1] = j;
            char piece = piece_type(curPosition, board);

            if (piece_team(piece) == attackingTeam)
            {
                if (is_legalmove(piece, curPosition, kingPosition, board,0) == 1)
                {
                    checkPieceP[0] = curPosition[0]; 
                    checkPieceP[1] = curPosition[1]; 
                    checks++;
                }
            }

        }
    }
    return checks;   
}

void get_check_path(char board[8][8], int checkPathCounter, char kingPosition[], char checkPosition[], char checkPath[8][2])
{
    switch (piece_type(checkPosition,board))
    {
        case 'n':
        case 'N':
        case 'p':
        case 'P': // knights and pawns dont have a path
        {
            checkPathCounter = 0;
            break;
        }

        case 'r':
        case 'R':
        {
            if (checkPosition[0] == kingPosition[0]) // vertical move >> num changes
            {
                for (int i = 1; i < positive(kingPosition[1]-checkPosition[1]); i++)
                {
                    checkPath[checkPathCounter][0] = checkPosition[0];
                    checkPath[checkPathCounter][1] = checkPosition[1] + (i * sign(kingPosition[1]-checkPosition[1]));
                    checkPathCounter++;
                }
                break;
            }
            else if(checkPosition[1] == kingPosition[1]) // horizontal move >> letter changes
            {
                for (int i = 1; i < positive(kingPosition[1]-checkPosition[1]); i++)
                {
                    checkPath[checkPathCounter][1] = checkPosition[1];
                    checkPath[checkPathCounter][0] = checkPosition[0] + (i * sign(kingPosition[0]-checkPosition[0]));
                    checkPathCounter++;
                }
                break;
            }
        }

        case 'b':
        case 'B':
        {
            for (int i = 1; i < positive(checkPosition[0]-kingPosition[0]); i++)
            {
                char currentTile[] = {checkPosition[0] + i * sign(kingPosition[0]-checkPosition[0]) , checkPosition[1] + i * sign(kingPosition[1]-checkPosition[1])};
                checkPath[checkPathCounter][0] = currentTile[0];
                checkPath[checkPathCounter][1] = currentTile[1];
                checkPathCounter++;
            }
            break;
        }

        case 'q':
        case 'Q':
        {
            if (checkPosition[0] == kingPosition[0]) // works as a rook - vertical move 
            {
                for (int i = 1; i < positive(kingPosition[1]-checkPosition[1]); i++)
                {
                    checkPath[checkPathCounter][0] = checkPosition[0];
                    checkPath[checkPathCounter][1] = checkPosition[1] + (i * sign(kingPosition[1]-checkPosition[1]));
                    checkPathCounter++;
                }
                break;
            }
            else if(checkPosition[1] == kingPosition[1]) // works as a rook - horizontal move 
            {
                for (int i = 1; i < positive(kingPosition[1]-checkPosition[1]); i++)
                {
                    checkPath[checkPathCounter][1] = checkPosition[1];
                    checkPath[checkPathCounter][0] = checkPosition[0] + (i * sign(kingPosition[0]-checkPosition[0]));
                    checkPathCounter++;
                }
                break;
            }
            else // works as a bishop
            {
                for (int i = 1; i < positive(checkPosition[0]-kingPosition[0]); i++)
                {
                    char currentTile[] = {checkPosition[0] + i * sign(kingPosition[0]-checkPosition[0]) , checkPosition[1] + i * sign(kingPosition[1]-checkPosition[1])};
                    checkPath[checkPathCounter][0] = currentTile[0];
                    checkPath[checkPathCounter][1] = currentTile[1];
                    checkPathCounter++;
                }
                break;
            }
        }
}
}

int is_checkmate(char board[8][8], int checks, char kingPosition[], char checkPosition[])
{
    // check if the king can change his position
    for (int i = 'A'; i <= 'H'; i++)
    {
        for (int j = '1'; j <= '8'; j++)
        {   
            // check if the king can move to that position
            char curPosition[2]; curPosition[0] = i; curPosition[1] = j;            
            if (is_legalmove(piece_type(kingPosition, board), kingPosition, curPosition, board,0) == 1)
            {
                // if he can then its not checkmate
                return 0;
            }
        }
    }

    // check if the threatening piece can be taken or blocked
    if (checks != 2) // exception for double check
    {
        // check if the threatening piece can be taken
        // loop over the board
        for (int i = 'A'; i <= 'H'; i++)
        {
            for (int j = '1'; j <= '8'; j++)
            {    
                char curPosition[2]; curPosition[0] = i; curPosition[1] = j;
                char piece = piece_type(curPosition, board);
                // check if any piece can take the piece threatening the king
                if (is_legalmove(piece, curPosition, checkPosition, board,0) == 1)
                {
                    // if it can then its not checkmate
                    return 0;
                }
            }
        }

        // block the way to the king   
        // get the array of tiles til we get to the king
        char checkPath[8][2];
        int checkPathCounter = 0;
        get_check_path(board, checkPathCounter, kingPosition, checkPosition, checkPath);
        // loop over each tile in the path
        for (int c = 0; c < checkPathCounter; c++)
        {
            // loop over each piece
            for (int i = 'A'; i <= 'H'; i++)
            {
                for (int j = '1'; j <= '8'; j++)
                {   
                    // check if any piece can block the tile
                    char curPosition[2]; curPosition[0] = i; curPosition[1] = j;
                    char piece = piece_type(curPosition, board);
                    
                    if (is_legalmove(piece, curPosition, checkPath[c], board,0) == 1)
                    {
                        // if it can then its not checkmate
                        return 0;
                    }
                }
            }
        }
    }
    
    return 1;
    
}

int is_stalemate(char board[8][8], char team)
{
    for (int i = 'A'; i <= 'H'; i++)
    {
        for (int j = '1'; j <= '8'; j++)
        {
            char pieceP[2]; pieceP[0] = i; pieceP[1] = j;
            char piece = piece_type(pieceP, board);

            // skip the other team pieces
            if (piece_team(piece) != team)
            {
                continue;
            }

            // check the positions the piece can get to
            for (int r = 'A'; r <= 'H'; r++)
            {
                for (int c = '1'; c <= '8'; c++)
                {
                    char curTile[2]; curTile[0] = r; curTile[1] = c;

                    // skip it's self
                    if (curTile[0] == pieceP[0] && curTile[1] == pieceP[1])
                    {
                        continue;
                    }
                    
                    // if any move can be made by any piece on the team then its not stalemate
                    enum MoveState{illegal, legal, Promotion} move;
                    move = is_legalmove(piece, pieceP, curTile, board, 2);

                    if ( move == legal || move == Promotion)
                    {
                        return 0;
                    }
                    
                }
            }

        }
    }
    return 1;

}

void undo(char board[8][8], char moves[1000][4], int turn, char removed[])
{
    board['8' - moves[turn][1]][toupper(moves[turn][0]) - 'A'] = board['8' - moves[turn][3]][toupper(moves[turn][2]) - 'A'];
    board['8' - moves[turn][3]][toupper(moves[turn][2]) - 'A'] = removed[turn];
}

