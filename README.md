# Gamma - console game in C

## Rules

The game is played on a rectangular board made of equal square fields. 

Adjacent spaces on the board are those that touch each other on their sides. We do not consider fields touching only with corners as adjacent. A space forms an area if each can be reached from one another by only passing through the adjacent spaces. A single field is also an area. 

One or more people may be playing. At the beginning of the game, the board is empty. Players take turns to occupy one field, placing their pawn on it. A player may occupy any unoccupied space, observing only the rule that a set of areas occupied by the same player may not, in any phase of the game, consist of more than the maximum number of areas, which is a parameter of the game. 
Each player can make a golden move once in the game, which consists in taking another player's pawn from the board and placing his pawn in its place, but this move must not violate the rule of the maximum number of occupied areas by any player. 
A player who is unable to make a move according to the above rule is out of the game, but may return to play after some golden move of another player. 
The game ends when neither player can move anymore. The player with the greatest number of fields wins.

## Description of use

### Build

```
mkdir release
cd release
cmake ..
make
```
#### Interactive mode
To start a new game in interactive mode, after running the program, type 
```
I width height players max_areas
```
then the game board will be displayed.  
![Screenshot from 2021-03-17 21-06-16](https://user-images.githubusercontent.com/80756697/111531698-2ceb5900-8765-11eb-9a4f-ad984125a919.png)  
`Arrows` - moves the cursor  
`Space` - move  
`G` - golden move  
`C` - abandonment of movements  
`Ctrl + D` - interrupt the game.  

After the end of the game the result will be displayed.  
![Screenshot from 2021-03-17 21-09-18](https://user-images.githubusercontent.com/80756697/111532022-8bb0d280-8765-11eb-9694-62a2a4515110.png)
