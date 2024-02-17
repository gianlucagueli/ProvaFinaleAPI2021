#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct struct_graphNode {
  long minimumPath;
  int graphIndex;
  struct struct_graphNode *next;
  struct struct_graphNode *prev;
} graphNode;

typedef graphNode *NODE;

/* 
* Concatenate from NODE a to NODE b and vice-versa.
*  a -> next = b
*  b -> prev = a
*/
void concatenate(NODE a, NODE b);

/*
* Records a bi-dimensional array from stdin. dim value represents the size of the array.
* The format of each line has to be: "%d,%d,.....,%d" with as many %d as dim.
*/
void addGraph(int *matrix, int dim);

/*
* Returns the sum of all path from NODE 0 (matrix[0][0]) to all NODEs.
*/
long dijkstra(int *matrix, bool *visited, long *minimumPath, int dim);

/* 
* Feats for dijkstra.
* Returns the index of the minimum value not visited yet among minimumPath.
* Returns -1 if there isn't any value in minimumPath that satisfies the requirements.
*/
int extractMin(long *minimumPath, bool *visited, int dim);

/* 
* Feats for dijkstra.
* Modifies vector2Use replacing the values with the index of the NODES connected to.
*/
void getAdiacents(int *matrix, int currIndex, int *vector2Use, int dim);

/*
* Builds a NODE with the passed specs and the links pointing to NULL.
*/
NODE buildsNode(long path, int index);

/*
* Removes the tail and concatenate the penultimate NODE with the head.
* Returns the pointer to the old tail.
*/
NODE removeTail(NODE head);

/*
* Inserts the NODE node2insert in ascending order in the linkedList.
*/
void insertNode(NODE node2insert, NODE *head);

/*
* Prints the graphIndex value of each node of the list.
*/
void printList(NODE *head);

/*  
* Returns the current value of the tail or -1 if any error occour.
*/
long getTailValue(NODE *head);

/*
* Free up memory by deleting the list.
*/
void delete_list(NODE *head);


int main(int argc, char const *argv[]) {
  char command[14];
  int currIndex = 0;
  int currK = 0;
  int k = 0;
  int dim = 0;

  long *minimumPath;
  bool *visited;
  int *matrix;

  NODE head = NULL;

  if(scanf("%d%*c%d", &dim, &k ) == 2) {
    visited = (bool *) malloc(dim * sizeof(bool));
    minimumPath = (long *) malloc(dim * sizeof(long));
    matrix = (int *) malloc(dim * dim * sizeof(int));

    //sets utility for dijkstra algorithm
    for(int i = 0; i < dim; i++) {
      visited[i] = false;
      minimumPath[i] = -1;
    }

    while(scanf("%s", command) != EOF) {
      if(strcmp(command, "TopK") == 0) {
        printList(&head);
      }
      else if (strcmp(command, "AggiungiGrafo") == 0) {
        addGraph(matrix, dim);

        long num = dijkstra(matrix, visited, minimumPath, dim);

        if(currK < k) {
          NODE node2insert = buildsNode(num, currIndex);
          insertNode(node2insert, &head);
          currK++;
        }
        else if(k == 1 && (head -> minimumPath > num)) {
          head -> minimumPath = num;
          head -> graphIndex = currIndex;
        }
        else if(k > 1 && getTailValue(&head) != -1 && getTailValue(&head) > num) {
          NODE oldTail = removeTail(head);
          oldTail -> minimumPath = num;
          oldTail -> graphIndex = currIndex;
          insertNode(oldTail, &head);
        }

        currIndex++;

      }
      else {
        printf("ERROR: Not supported operation!\n");
        exit(1);
      }
    }

    free(minimumPath);
    free(visited);
    free(matrix);
    delete_list(&head);

    return 0;
  }

  return -1;
}

void addGraph(int *matrix, int dim) {
  int length = 256;
  char line[length];
  char *ptr;

  for(int i = 0; i < dim; i++) {
      int currIndex = 0;
      while (currIndex < dim) {
        //reads array of length-size each time and parse for int
        if(fgets(line, sizeof line, stdin) != NULL) {
          ptr = line;
          //if line contains '\n' this means that line (variable) is the last array
          //read of the current line (input as stdin)
          if(strchr(line, '\n') != NULL) {
            while(strlen(ptr) != 1 ) {
              //skips every ',' char
              while(ptr[0] == ',' ) {
                ptr = ptr + 1;
              }

              //equals to matrix[i][currIndex]
              *(matrix + i * dim + currIndex) = strtol(ptr, &ptr, 10);
              currIndex++;
            }
          }
          else {
            //keeps parse till ptr (which is the mobile pointer to parse the string) has ','
            while(strchr(ptr, ',') != NULL) {
              while(ptr[0] == ',' ) {
                ptr = ptr + 1;
              }
              *(matrix + i * dim + currIndex) = strtol(ptr, &ptr, 10);
              currIndex++;
            }
            //if tmp is a digit, keeps reading and updating the right cell of the matrix
            //till tmp isn't a digit anymore
            char tmp = getchar();
            while(tmp >= 48 && tmp <= 57) {
              *(matrix + i * dim + currIndex - 1) =  10*(*(matrix + i * dim + currIndex - 1)) + (tmp - '0');
              tmp = getchar();
            }
          }
        }
    }
  }
}

int extractMin(long *minimumPath, bool *visited, int dim) {
  int currMin = -1;
  int currIndex = -1;

  for(int i = 0; i < dim; i++) {
    if(!visited[i] && ((currMin == -1 && minimumPath[i] != -1) || (currMin > minimumPath[i] && minimumPath[i] != -1))) {
      currIndex = i;
      currMin = minimumPath[i];
    }
  }
  return currIndex;
}


void getAdiacents(int *matrix, int currIndex, int *vector2Use, int dim) {
  int  currSize = 0;
  for(int i = 0; i < dim; i++) {
    if(*(matrix + currIndex * dim + i) != 0) {
        vector2Use[currSize] = i;
        currSize++;
    }
  }
}


long dijkstra(int *matrix, bool *visited, long *minimumPath, int dim) {
  long sum = 0;
  int index;
  int currCost;
  int *adj = (int *) calloc(dim, sizeof(int));

  minimumPath[0] = 0;
  for(int i = 0; i < dim; i++) {
      index = extractMin(minimumPath, visited, dim);
      if(index == -1) {
        break;
      }
      visited[index] = true;

      getAdiacents(matrix, index, adj, dim);

      int countZero = 0;
      for(int j = 0; j < dim && countZero <= 1; j++) {
        if(adj[j] == 0) {
          countZero ++;
        }
        else {
          currCost = *(matrix + index * dim + adj[j]);

          //updates minimumPath if finds a new path
          if(currCost != 0 && (minimumPath[adj[j]] == -1 ||
              minimumPath[adj[j]] > minimumPath[index] + currCost)) {

            minimumPath[adj[j]] = minimumPath[index] + currCost;
          }
          //set adj to 0 so that it is ready for the next getAdjacent call
          adj[j] = 0;
        }
      }
  }
  free(adj);

  for(int i = 0; i < dim; i++) {
    if(minimumPath[i] > -1) {
      sum +=  minimumPath[i];
      minimumPath[i] = -1;
    }
    visited[i] = false;
  }
  return sum;
}

NODE buildsNode(long path, int index) {
  NODE node =  malloc(sizeof(graphNode));
  node -> minimumPath = path;
  node -> graphIndex = index;
  node -> next = NULL;
  node -> prev = NULL;

  return node;
}

void insertNode(NODE node2insert, NODE *head) {
  if (*head == NULL) {
    *head = node2insert;
  }
  else if ((*head) -> next == NULL && (*head) -> prev == NULL) {
    concatenate(*head, node2insert);
    concatenate(node2insert, *head);
    if((*head) -> minimumPath > node2insert -> minimumPath) {
      *head = node2insert;
    }
  }
  else {
    NODE tmp = *head;
    while(node2insert -> minimumPath > tmp -> minimumPath && tmp -> next != *head) {
      tmp = tmp -> next;
    }
    if(tmp == *head) {
      *head = node2insert;
    }
    if(tmp -> next == *head && node2insert -> minimumPath > tmp -> minimumPath) {
      concatenate(node2insert, tmp->next);
      concatenate(tmp, node2insert);
    } else {
      concatenate(tmp -> prev, node2insert);
      concatenate(node2insert, tmp);
    }
  }
}


NODE removeTail(NODE head) {
  NODE tmp = head->prev;
  concatenate(tmp->prev, tmp->next);

  return tmp;
}

void printList(NODE *head) {
  NODE tmp = *head;

  while(tmp != NULL && tmp -> next != *head && tmp -> next != NULL) {
    printf("%d ", tmp -> graphIndex);
    tmp = tmp->next;
  }

  if(tmp != NULL) {
    printf("%d\n", tmp -> graphIndex);
  } else {
    printf("\n");
  }
}


long getTailValue(NODE *head) {
  NODE tmp = *head;

  if(tmp->prev != NULL) {
    long num = tmp -> prev -> minimumPath;
    return num;
  }

  return -1;
}

void concatenate(NODE a, NODE b)  {
  a->next = b;
  b->prev = a;
}

void delete_list(NODE *head) {
  NODE tmp = (*head) -> next;
  
  while(tmp != NULL && tmp -> next != (*head)) {
    tmp = tmp -> next;
    free(tmp -> prev);
  }
  free(tmp);
  free(*head);
}
