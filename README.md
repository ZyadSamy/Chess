# Chess
*Course project for CSE 121: Programming 1, Alexandria University Faculty of Engineering,  
Computer and Systems Engineering Dept .*

**Description** 

Chess console application made in C programming language. 
 It provides the essential blocks found in any chess game, which are: 

- A labelled chess board 
- Two players competing against each other (Black and White) 
- The special movement of each piece 
- The core mechanics of the game (Promotion, Stalemate, and Checkmate) 

In addition to the mentioned mechanics, some additional features were added to make the experience more flexible and enjoyable, such as: 

- The ability to save and load your concurrent game 
- The ability to undo and redo your actions 

---

**Team members**:  

|Name |
| - |
|Ramy Ahmed El Sayed|
|Zyad Samy Ramadan|




**Design overview** 

- Our design is mainly focused on our chess board. Our game functions in an infinite loop that breaks when one of the terminating game conditions occur (Checkmate or Stalemate). 
- It checks for the user input depending on the specified player turn (Player 1 or Player 2) and, checks whether the input is valid or not. 
- It then checks the piece picked and whether the target position is in the boundaries of the piece’s movements. 
- Continuously, checks for a Checkmate or a Stalemate. 
- Depending on the user input, various functions can occur, such as: 
- Saving 
- Loading 
- Undo 
- Redo 
- Once one of the terminating conditions occur, It breaks the loop and terminates the program. 

## **Design assumptions** 

It is assumed that the number of moves will not exceed 1000, since the lack of dynamic arrays in C, and the complexity of using other data structures, which would behave like a dynamic array (e.g., Hash tables or Linked lists). 

## **Used Data structures** 

Our program mainly utilizes one data structure, which is Arrays. 

We have several arrays that behave as main parameters for most of the functions, such as: 

1. Chess board (an 8x8 array that contains the pieces and the tiles) 
2. Moves (an array that records the moves done in each turn) 
3. Pre-undo array (records the moves that will be undone) 
4. Units removed (records the units removed) 
5. Promotion array 
6. Black and white king position arrays 

## **Important functions** 


**print\_board():** 

Our main function that prints our updated Chess board with the removed units and moved units. 

**is\_legalmove():** 

Checks if the move is valid to proceed to modify the main chess board array, and checks whether the moves will activate any special game states (Promotion/Stalemate/Checkmate). 

**modify():** 

Modifies our Chess board, according to the user input, after checking whether it is valid or not. It is also used when redoing a move. 

**is\_check():** 

Keeps checking on the king’s current position (Black or White depending on the current turn) and checks whether any unit of the opposing team can capture the king, if any piece can capture it; then the king is in check. 

**is\_checkmate():** 

If the **is\_check()** function shows that a king is in check,  then check if it can be denied by the threatened king or his pieces. If it cannot be denied, then a checkmate has been concluded and the game ends. 

**is\_stalemate():** 

If a stalemate is reached, the game terminates with a draw.

---

## **User manual**

When the user starts the .exe, he is prompted by this screen: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.005.jpeg)

The screen contains: 

- The chess board. 
- The lost units. 
- Instructions for other functions. 
- Player’s turn. 
- Move to be inserted. 

**User input:** 

The user is required to insert his move in this format, namely (A2A3/a2a3): 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.006.jpeg)

After inserting the input, the user is prompted with an updated chess board: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.007.jpeg)

**Undo/Redo:** 

If the user inserts ‘u’, the game undoes his last action. Example: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.008.jpeg)

Player 1 will eat Player 2’s pawn located in B5: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.009.jpeg)

The pawn is now added to Player 2’s lost units. If we want to undo the action, we insert ‘u’: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.010.jpeg)

Which will result in: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.011.jpeg)

And to redo what we just undid, the user can simply insert ‘d’: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.012.jpeg)

**Save/Load:** 

Supposing the player wants to save his chess match to continue later, for example: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.013.jpeg)

If the player inserts ‘s’ in the input, the grid board is saved in a text file:

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.014.jpeg)

Which will look like this: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.015.jpeg)

The text file contains the grid board, black king’s position, white king’s position, and the turn at which it was saved. 

If the player were to proceed and close the game and re-open, then type “l” as an input, the game will load and will proceed as usual: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.016.jpeg)

The loaded game: 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.017.jpeg)

## **Sample runs** 

1. 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.018.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.019.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.020.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.021.jpeg)

2. 

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.022.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.023.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.024.jpeg)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.025.png)

**3. Save/Load**

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.026.png)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.027.png)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.028.png)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.029.jpeg)

**4. Stalemate**

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.030.png)

![](Aspose.Words.380f6962-a359-4543-b28b-a41fe456e34b.031.png)
