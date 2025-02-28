/**
* The program lets user play a game of Sokoban
*
* The program reads map from input, then simulates
* the player's moves as they input them.
*
* The player's moves refer to the boxes on the board.
* The program checks if the move is legal and allows to undo them.
* Process ends when a '.' is inputted.
*
* Author: Oskar Rowicki
* Date: 21.12.2024
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Difference between ASCII codes of lower and upper case letters,
// adding and subtracting allows to change case.
#define CHANGECASE 32

#define UP '8'
#define DOWN '2'
#define LEFT '4'
#define RIGHT '6'

#define UNCHECKED 0
#define UNCHECKEDAROUND 1
#define REACHABLE 2
#define UNREACHABLE 3

// Moves list structure used for undo implementation
typedef struct move {
  int playerPos;
  int boxChar;
  int direction;
  struct move *next;
} move_t;

// Reads game map from input, stops reading when it reads
// two '\n' in a row.
// The map is represented as an array.
void ReadMap(char **map, int *n) {
  bool end_of_input = false, last_newline = false;
  char *array = NULL;
  int i = 0, size = 0;
  int c;
  while (!end_of_input && (c = getchar()) != EOF) {
    if (c == '\n') {
      if (last_newline) {
        end_of_input = true;
      } else {
        last_newline = true;
      }
    } else {
      last_newline = false;
    }
    if (!end_of_input) {
      if (i == size) {
        size = size * 2 + 1;
        array = realloc(array, (size_t) size * sizeof *array);
      }
      array[i] = (char) c;
      i++;
    }
  }
  *map = array;
  *n = i;
}

void PrintMap(char map[], int n) {
  for (int i = 0; i < n; i++) {
    printf("%c", map[i]);
  }
}

// Returns index of first element in the array, that's the same as specified.
int FindSymbol(char map[], int n, int symbol) {
  int i = 0;
  while (i < n && map[i] != symbol) {
    i++;
  }
  return i;
}

// Returns index of the player character in the array.
int FindPlayer(char map[], int n) {
  if (FindSymbol(map, n, '@') < n) {
    return FindSymbol(map, n, '@');
  }
  return FindSymbol(map, n , '*');
}

// Returns index of the next '\n' in the array.
int FindNextNewline(char map[], int n, int origin) {
  while (origin < n && map[origin] != '\n') {
    origin++;
  }
  return origin;
}

int FindPrevNewline(char map[], int origin) {
  while (origin > -1 && map[origin] != '\n') {
    origin--;
  }
  return origin;
}

// Finds the element underneath the given square on the map.
// Returns its index or -1 if it doesn't exist.
int BottomElement(char map[], int n, int origin) {
  int column = origin - FindPrevNewline(map, origin);
  int newline_1 = FindNextNewline(map, n, origin);
  int newline_2 = FindNextNewline(map, n, newline_1 + 1);
  if (newline_1 + column < newline_2) {
    return newline_1 + column;
  }
  return -1;
}

int TopElement(char map[], int origin) {
  int newline_2 = FindPrevNewline(map, origin);
  int newline_1 = FindPrevNewline(map, newline_2 - 1);
  int column = origin - newline_2;
  if (newline_1 >= -1 && newline_1 + column < newline_2) {
    return newline_1 + column;
  }
  return -1;
}

int RightElement(char map[], int n, int origin) {
  if (origin + 1 < n && map[origin + 1] != '\n') {
    return origin + 1;
  }
  return -1;
}

int LeftElement(char map[], int origin) {
  if (origin - 1 > -1 && map[origin - 1] != '\n') {
    return origin - 1;
  }
  return -1;
}

int FindOppositePos(char map[], int n, int origin, int direction) {
  switch (direction) {
    case UP:
      return BottomElement(map, n, origin);
    case RIGHT:
      return LeftElement(map, origin);
    case LEFT:
      return RightElement(map, n, origin);
    case DOWN:
      return TopElement(map, origin);
    default:
      return 0;
  }
}

int FindDestination(char map[], int n, int origin, int direction) {
  switch (direction) {
    case UP:
      return TopElement(map, origin);
    case RIGHT:
      return RightElement(map, n, origin);
    case LEFT:
      return LeftElement(map, origin);
    case DOWN:
      return BottomElement(map, n, origin);
    default:
      return 0;
  }
}

// Moves box from specified origin to destination, together with the player.
void MoveBox(char map[], int n, int origin, int destination) {
  if (map[FindPlayer(map, n)] == '@') {
    map[FindPlayer(map, n)] = '-';
  } else map[FindPlayer(map, n)] = '+';
  char goal = map[destination];
  map[destination] = map[origin];
  if (map[origin] >= 'A' && map[origin] <= 'Z') {
    map[origin] = '*';
    if (goal == '-' || goal == '@') {
      map[destination] += CHANGECASE;
    }
  } else {
    map[origin] = '@';
    if (goal == '+' || goal == '*') {
      map[destination] -= CHANGECASE;
    }
  }
}

// Adds move to the list for undoing.
move_t *AddMoveToStack(move_t *head, int player_pos, int box_char, int direction) {
  move_t *new_move = malloc(sizeof(move_t));
  new_move->playerPos = player_pos;
  new_move->boxChar = box_char;
  new_move->direction = direction;
  new_move->next = head;
  return new_move;
}

// Undoes last move.
move_t *UndoMove(move_t *head, char map[], int n) {
  int symbol = head->boxChar;
  if (FindSymbol(map, n, symbol) >= n) {
    symbol -= CHANGECASE;
  }
  int origin = FindSymbol(map, n, symbol);
  int destination = FindOppositePos(map, n, origin, head->direction);
  MoveBox(map, n, origin, destination);
  if (map[FindPlayer(map, n)] == '@') {
    map[FindPlayer(map, n)] = '-';
  } else {
    map[FindPlayer(map, n)] = '+';
  }
  if (map[head->playerPos] == '-') {
    map[head->playerPos] = '@';
  } else {
    map[head->playerPos] = '*';
  }
  move_t *new_head = head->next;
  free(head);
  return new_head;
}

// Checks if box can be moved to a square represented by the specified symbol.
bool IsSymbolValid(int symbol) {
  if (symbol == '-' || symbol == '+' || symbol == '@' || symbol == '*') {
    return true;
  }
  return false;
}

// Checks if it's possible for the player to move from the origin to the destination.
bool CheckPathExists(char map[], int n, int origin, int destination) {
  bool checked_all = false, reachable = false;
  int *path = calloc((size_t) n, (size_t) sizeof(int));
  path[origin] = UNCHECKEDAROUND;
  while (!checked_all && !reachable) {
    checked_all = true;
    if (path[destination] == REACHABLE) {
      reachable = true;
    }
    for (int i = 0; i < n; i++) {
      if (path[i] == UNCHECKEDAROUND) {
        checked_all = false;
        int Adjacent[4];
        Adjacent[0] = TopElement(map, i);
        Adjacent[1] = RightElement(map, n, i);
        Adjacent[2] = BottomElement(map, n, i);
        Adjacent[3] = LeftElement(map, i);
        for (int j = 0; j < 4; j++) {
          if (Adjacent[j] != -1 && path[Adjacent[j]] == 0) {
            if (IsSymbolValid(map[Adjacent[j]])) {
              path[Adjacent[j]] = UNCHECKEDAROUND;
            } else {
              path[Adjacent[j]] = UNREACHABLE;
            }
          }
        }
        path[i] = REACHABLE;
      }
    }
  }
  free(path);
  return reachable;
}

// Reads moves from input, performs actions accordingly. Stops after '.' .
bool ReadMove(char map[], int n, move_t **head) {
  int c = getchar();
  if (c == '.' || c == EOF) {
    return true;
  }
  if (c == '\n') {
    PrintMap(map, n);
  } else if (c == '0') {
    if (*head != NULL) {
      *head = UndoMove(*head, map, n);
    }
  } else {
    int symbol = c;
    if (FindSymbol(map, n, symbol) >= n) {
      symbol -= CHANGECASE;
    }
    int direction = getchar();
    int origin = FindSymbol(map, n, symbol);
    int player_destin = FindOppositePos(map, n, origin, direction);
    int destin = FindDestination(map, n, origin, direction);
    if (destin != -1 && player_destin != -1 &&
        CheckPathExists(map, n, FindPlayer(map, n), player_destin) && IsSymbolValid(map[destin])) {
      *head = AddMoveToStack(*head, FindPlayer(map, n), c, direction);
      MoveBox(map, n, origin, destin);
    }
  }
  return false;
}

// Reads map, then reads and performs player moves until '.' .
int main(void) {
  char *map;
  move_t *head = NULL;
  int n;
  bool end_of_input = false;
  ReadMap(&map, &n);
  PrintMap(map, n);
  while (!end_of_input) {
    end_of_input = ReadMove(map, n, &head);
  }
  free(map);
  while (head) {
    move_t *new_head = head->next;
    free(head);
    head = new_head;
  }
}