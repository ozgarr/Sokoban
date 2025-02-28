# Sokoban
A simplified version of puzzle game Sokoban.
## Game explanation
Sokoban is a japanese puzzle video game, where the player pushes boxes around a warehouse trying to get them to the goal squares.
In the original game the player controls the player and walks into a box to push it; in this version the player specifies which box they want to push
and in which direction and the program does the rest (checks if move is legal and if so moves the box and the player accordingly).
## Map legend

The map consists of empty squares, walls, boxes, goal squares and the player.  
'-' empty square  
'#' wall  
'a'-'z' boxes  
'+' goal square  
'@' the player  
If a box is on a goal square. the letter gets capitalized; if the player is on a goal square, '@' changes to '*'.

User can create any map as long as it has exactly one player and follows the legend above.
For example:  
@-------  
----b++-  
--####--  
--#--#--  
--#-a#--  
--#--#--  
--####--  
\--------  
\--------  

The map can be in various shapes:  
\+  
\--  
\-a-  
\----  
\-----  
\------  
\-c#####  
\------  
\-----  
\----  
\-b-  
\--  
\*  

# Controls
After inputting the map, the user has to enter a newline twice to confirm the layout and start solving the puzzle.
The user performs moves by typing a letter representing the box they want to move, followed by a number symbolising the direction;  
8 Up  
2 Down  
4 Left  
6 Down  
(like arrows on the numpad)  
The user can also input a '0' to undo the last legal move (there is no limit to undoes) and a newline to print the current state of the puzzle.  
Example string of commands:  
b6b6b6  
b6  
b8  
b8  
b4  
a2  
0  
0  
0  
0  
0  
b4  
.
