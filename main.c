#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int dest;
  int *weights;
} Edge;
typedef struct {
  int id;
  Edge **edges;
  int num_edges;
} Node;
typedef struct {
  Node **nodes;
  int num_nodes;
} Graph;
typedef struct {
  int label;
  int distance;
  int predecessor;
} HeapNode;

Graph *get_inputs(char **, int *, int *);
Node *create_node(int);
Edge *create_edge(int, int);
void print_graph(Graph *, int);
void dijkstra(int, int, int *);

int main(int argc, char **argv) {
  int nv = 0; // num vertices
  int nw = 0; // num weights per edge

  Graph *g = get_inputs(argv, &nv, &nw);
  int heap_index[nv];
  print_graph(g, nw);
  assert(g == NULL); // free the graph and all nodes
  return 0;
}

Graph *get_inputs(char **argv, int *nv, int *nw) {
  // open file
  FILE *f = fopen(argv[1], "r");
  if (!f) {
    fprintf(stderr, "fopen in get_inputs failed");
    exit(1);
  }
  // read num vertex and edge
  if (fscanf(f, "%d %d\n", nv, nw) != 2) {
    fprintf(stderr, "scanning first line of file failed");
    exit(1);
  }
  // create graph
  Graph *g = (Graph *)malloc(sizeof(Graph));
  g->nodes = (Node **)malloc(sizeof(Node) * *nv);
  g->num_nodes = 0;
  // first node
  int t;
  Node *n;
  Edge *e;
  // read id
  if (fscanf(f, "%d ", &t) != 1) {
    fprintf(stderr, "first node id read failed");
    exit(1);
  }
  n = create_node(t);
  n->num_edges = 1;
  // read edge
  if (fscanf(f, "%d ", &t) != 1) {
    fprintf(stderr, "first edge dest read failed");
    exit(1);
  }
  e = create_edge(t, *nw);
  // read weights
  for (int i = 0; i < *nw - 1; i++) {
    if (fscanf(f, "%d ", &t) != 1) {
      fprintf(stderr, "weight reading failed");
      exit(1);
    }
    e->weights[i] = t;
  }
  if (fscanf(f, "%d\n", &t) != 1) {
    fprintf(stderr, "last weight reading failed");
    exit(1);
  }
  e->weights[*nw - 1] = t;
  n->edges[n->num_edges - 1] = e;
  g->nodes[g->num_nodes++] = n;

  // read rest of nodes
  while (1) {
    // check for EOF
    if (feof(f)) {
      break;
    }
    // id
    if (fscanf(f, "%d ", &t) != 1) {
      fprintf(stderr, "rest node id read failed");
      exit(1);
    }
    // check if same node
    if (t != g->nodes[g->num_nodes - 1]->id) {
      // create new node
      n = create_node(t);
      n->num_edges = 1;
      g->num_nodes++;
      Node **temp = realloc(g->nodes, sizeof(Node *) * g->num_nodes);
      if (temp == NULL) {
        fprintf(stderr, "realloc node failed");
        free(g);
        exit(1);
      }
      g->nodes = temp;
      g->nodes[g->num_nodes - 1] = n;
    } else {
      g->nodes[g->num_nodes - 1]->num_edges++;
      Edge **temp =
          realloc(g->nodes[g->num_nodes - 1]->edges,
                  sizeof(Edge *) * g->nodes[g->num_nodes - 1]->num_edges);
      if (temp == NULL) {
        fprintf(stderr, "realloc edge failed");
        free(g);
        exit(1);
      }
      g->nodes[g->num_nodes - 1]->edges = temp;
    }
    if (fscanf(f, "%d ", &t) != 1) {
      fprintf(stderr, "additional edge breaking");
      exit(1);
    }
    e = create_edge(t, *nw);
    for (int j = 0; j < *nw - 1; j++) {
      if (fscanf(f, "%d ", &t) != 1) {
        fprintf(stderr, "weight reading failed");
        exit(1);
      }
      e->weights[j] = t;
    }
    if (fscanf(f, "%d\n", &t) != 1) {
      fprintf(stderr, "last weight reading failed");
      exit(1);
    }
    e->weights[*nw - 1] = t;
    g->nodes[g->num_nodes - 1]
        ->edges[g->nodes[g->num_nodes - 1]->num_edges - 1] = e;
  }

  fclose(f);
  return g;
}

Node *create_node(int id) {
  Node *n = (Node *)malloc(sizeof(Node));
  n->id = id;
  n->num_edges = 0;
  n->edges = (Edge **)malloc(sizeof(Edge));
  return n;
}

Edge *create_edge(int dest, int nw) {
  Edge *e = (Edge *)malloc(sizeof(Edge));
  e->dest = dest;
  e->weights = (int *)malloc(sizeof(int) * nw);
  return e;
}

void print_graph(Graph *g, int nw) {
  for (int i = 0; i < g->num_nodes; i++) {
    printf("node #%d\n", g->nodes[i]->id);
    for (int j = 0; j < g->nodes[i]->num_edges; j++) {
      printf("edge #%d, dest %d\n", j, g->nodes[i]->edges[j]->dest);
      for (int k = 0; k < nw; k++) {
        printf("%d\n", g->nodes[i]->edges[j]->weights[k]);
      }
    }
    printf("\n\n");
  }
}

void dijkstra(int source, int nv, int *heap_index) {
  // taken from lecture slides
  HeapNode heap[nv];
  int n = nv;

  for (int i = 0; i < nv; i++) {
    heap[i].label = i;
    heap[i].distance = INT_MAX;
    heap[i].predecessor = -1;
    heap_index[i] = i;
  }
  heap[0].distance = 0;
}
