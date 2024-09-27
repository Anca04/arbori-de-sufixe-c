#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// am adaugat o noua variabila de tip boolean pentru a ma ajuta sa calculez nr
// de sufixe
typedef struct node {
  struct node *children[27];
  bool isEndOfSuffix[100];
} Node, *Tree;

// aloca spatiu pentru nodurile din arbore, pentru copii
Node *createNode(void) {
  int i;
  Node *new_node = (Node *)malloc(sizeof(Node));

  if (new_node == NULL) {
    printf("Memory can't be allocated for the new_node.");
    exit(-1);
  }

  for (i = 0; i < 27; i++) new_node->children[i] = NULL;
  return new_node;
}

// extrag sufixele din cuvant, apoi parcurg sufixul litera cu litera si testez
// daca a fost inserat sau nu, adica verific daca a fost deja alocat spatiu
// pentru el de asemenea, aloc spatiu pentru $
// tot aici calculez si nr de sufixe de dimensiunea K cu ajutorul variabilei de
// tip boolean
void insert(Node *tree, char *s, int K, int *nr_suffix) {
  Node *current = tree;
  int i, j, index;
  int lenght = strlen(s);
  int lenght_suffix;
  char *suffix;

  for (i = 0; i < lenght; i++) {
    // extrag sufixele
    suffix = s + lenght - i - 1;
    lenght_suffix = strlen(suffix);

    current = tree;
    // parcurg sufixul
    for (j = 0; j < lenght_suffix; j++) {
      index = suffix[j] - 'a' + 1;

      // testez daca litera a fost inserata sau nu
      if (current->children[index] == NULL) {
        current->children[index] = createNode();

      } else if (current->children[index] != NULL) {
        // calculez urmatorul index
        while (current->children[index] != NULL) {
          current = current->children[index];
          j++;
          index = suffix[j] - 'a' + 1;
        }

        if (suffix[j] == '\0') {
          break;
        } else {
          current->children[index] = createNode();
        }
      }
      current = current->children[index];
    }

    // calculez nr de sufixe de dim K
    if (j == K && !current->isEndOfSuffix[K]) {
      current->isEndOfSuffix[K] = true;
      (*nr_suffix)++;
    }

    // aloc spatiu pt $
    current->children[0] = createNode();
  }
}

// verific daca un sufix exista sau nu in arbore, parcurg sufixul si testez daca
// se afla sau nu in arbore, cu ajutorul lui ok afisez daca sufixul se afla in
// arbore sau nu
void search_suffix(Node *tree, char *suffix, FILE *outfile) {
  Node *tmp = tree;
  int length_suffix = strlen(suffix);
  int i, index, ok = 1;

  for (i = 0; i < length_suffix; i++) {
    index = suffix[i] - 'a' + 1;

    if (tmp->children[index] == NULL) {
      ok = 0;
      break;
    } else {
      tmp = tmp->children[index];
    }
  }

  if (tmp->children[0] == NULL) {
    ok = 0;
  }

  if (ok == 1)
    fprintf(outfile, "1\n");
  else
    fprintf(outfile, "0\n");
}

// afisarea in latime cu ajutorul unei cozi
void bfs(Node *tree, FILE *outfile) {
  Node *q[1000], *current;
  int head = 0;
  int tail = 0;
  int level_nodes, i;

  if (tree == NULL) return;

  // adaug in coada radacina
  q[tail++] = tree;

  while (head < tail) {
    level_nodes = tail - head;

    // testez pe nivele
    while (level_nodes > 0) {
      current = q[head++];

      // adaug in coada copii
      for ( i = 0; i < 27; i++) {
        if (current->children[i] != NULL) {
          q[tail++] = current->children[i];

          // printez in functie de index valoarea literei sau $
          if (i == 0) {
            fprintf(outfile, "$ ");
          } else
            fprintf(outfile, "%c ", 'a' + i - 1);
        }
      }
      level_nodes--;
    }
    if (head < tail) fprintf(outfile, "\n");
  }
}

// calculez nr de noduri frunza, folosind din nou o coada, in loc de afisare
// cresc nr de noduri frunza
void numberLeaf(Node *tree, int *number_leaf) {
  Node *q[1000], *current;
  int head = 0;
  int tail = 0;
  int level_nodes, i;
  if (tree == NULL) return;

  q[tail++] = tree;

  while (head < tail) {
    level_nodes = tail - head;

    while (level_nodes > 0) {
      current = q[head++];

      for (i = 0; i < 27; i++) {
        if (current->children[i] != NULL) {
          q[tail++] = current->children[i];

          if (i == 0) (*number_leaf)++;
        }
      }
      level_nodes--;
    }
  }
}

// calculez nr maxim de descendenti directi
void maxNrDesecendents(Node *tree, int *max_nr_descendents) {
  int i;
  for (i = 0; i < 27; i++) {
    if (tree->children[i] != NULL) (*max_nr_descendents)++;
  }
}

// eliberare memorie arbore
void freeNode(Node *tree) {
  int i;
  if (tree == NULL) {
    return;
  }

  for (i = 0; i < 27; i++) {
    freeNode(tree->children[i]);
  }
  free(tree);
}

// am implementat atstfel incat sa pot rula cu -c1, -c2 etc, initializez
// arborele si primul nod copil $ care se afla in aceeasi pozitie mereu, apoi
// apelez functiile aferente pentru fiecare cerinta
int main(int argc, char const *argv[]) {
  FILE *infile, *outfile;
  Node *tree = createNode();

  int i, n, K, number_leaf = 0, N, M, j, nr_suffix = 0;
  int max_nr_descendents = 0;
  char s[100], aux[100], string[100], suffix[100];
  K = 0;

  (void)argc;

  // aloc spatiu pt primul copil care este mereu $
  tree->children[0] = createNode();

  strcpy(aux, argv[1]);

  // pentru fiecare cerinta am cate un caz
  if (strcmp(aux, "-c1") == 0) {
    infile = fopen(argv[2], "r");
    if (infile == NULL) {
      printf("infile can't be opened.");
      return 1;
    }

    outfile = fopen(argv[3], "w");
    if (outfile == NULL) {
      printf("outfile can't be opened.");
      return 1;
    }

    // n = nr de cuvinte din input
    fscanf(infile, "%d", &n);
    fgetc(infile);

    // citesc cuvintele din fisier
    for (i = 0; i < n; i++) {
      fscanf(infile, "%s", s);
      insert(tree, s, K, &nr_suffix);
    }

    bfs(tree, outfile);
  } else if (strcmp(aux, "-c2") == 0) {
    K = atoi(argv[2]);

    infile = fopen(argv[3], "r");
    if (infile == NULL) {
      printf("infile can't be opened.");
      return 1;
    }

    outfile = fopen(argv[4], "w");
    if (outfile == NULL) {
      printf("outfile can't be opened.");
      return 1;
    }

    // n = nr de cuvinte din input
    fscanf(infile, "%d", &n);
    fgetc(infile);

    // citesc cuvintele din fisier
    for (i = 0; i < n; i++) {
      fscanf(infile, "%s", s);
      insert(tree, s, K, &nr_suffix);
    }

    numberLeaf(tree, &number_leaf);
    maxNrDesecendents(tree, &max_nr_descendents);

    fprintf(outfile, "%d\n", number_leaf);
    fprintf(outfile, "%d\n", nr_suffix);
    fprintf(outfile, "%d\n", max_nr_descendents);

  } else if (strcmp(aux, "-c3") == 0) {
    infile = fopen(argv[2], "r");
    if (infile == NULL) {
      printf("infile can't be opened.");
      return 1;
    }

    outfile = fopen(argv[3], "w");
    if (outfile == NULL) {
      printf("outfile can't be opened.");
      return 1;
    }

    // citesc N din fisier
    fscanf(infile, "%d", &N);

    // citesc M din fisier
    fscanf(infile, "%d", &M);

    // citesc cuvintele
    for (i = 0; i < N; i++) {
      fscanf(infile, "%s", string);
      insert(tree, string, K, &nr_suffix);
    }

    // citesc sufixele
    for (j = 0; j < M; j++) {
      fscanf(infile, "%s", suffix);
      search_suffix(tree, suffix, outfile);
    }
  } else if (strcmp(aux, "-c4") == 0) {
    infile = fopen(argv[2], "r");
    if (infile == NULL) {
      printf("infile can't be opened.");
      return 1;
    }

    outfile = fopen(argv[3], "w");
    if (outfile == NULL) {
      printf("outfile can't be opened.");
      return 1;
    }

    // n = nr de cuvinte din input
    fscanf(infile, "%d", &n);
    fgetc(infile);

    // citesc cuvintele din fisier
    for (i = 0; i < n; i++) {
      fscanf(infile, "%s", s);
      insert(tree, s, K, &nr_suffix);
    }
  }

  freeNode(tree);

  fclose(infile);
  fclose(outfile);

  return 0;
}
