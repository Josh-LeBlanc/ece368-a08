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
  int max_edges;
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
HeapNode* dijkstra(int, int, int, int *, Graph *);
void dequeue(HeapNode *, int, int *);
void update(HeapNode *, int, int *);
int get_edge_num();

int x = 0;

int main(int argc, char **argv) {
  int nv = 0; // num vertices
  int nw = 0; // num weights per edge

  Graph *g = get_inputs(argv, &nv, &nw);
  int *heap_index = (int *)malloc(sizeof(int) * nv);
  // print_graph(g, nw);
  int source, dest;
  printf("input source dest: ");
  if (scanf("%d %d", &source, &dest) != 2) {
    fprintf(stderr, "error, please enter source and destination in the format 'source dest':");
    exit(1);
  }
  HeapNode* heap = dijkstra(source, nv, nw, heap_index, g);

  printf("print distances:\n\n");
  for (int i = 0; i < nv; i++) {
    printf("%d\n", heap[i].distance);
  }

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
  for (int i = 0; i < *nv; i++) {
    g->nodes[i] = create_node(-1);
  }

  int t, t1, t2;
  Edge *e;
  // read id
  while (fscanf(f, "%d %d", &t, &t1) != EOF) {
    // if node not in graph, create it
    if (g->nodes[t]->id == -1) {
      g->nodes[t] = create_node(t);
      g->num_nodes++;
    }
    // if node 2 not in graph, create it
    if (g->nodes[t1]->id == -1) {
      g->nodes[t1] = create_node(t1);
      g->num_nodes++;
    }
    // create edge on node 1
    if (g->nodes[t]->num_edges == g->nodes[t]->max_edges) {
      g->nodes[t]->max_edges++;
      Edge **temp =
          realloc(g->nodes[t]->edges,
                  sizeof(Edge *) * g->nodes[t]->max_edges);
      if (temp == NULL) {
        fprintf(stderr, "realloc edge failed");
        free(g);
        exit(1);
      }
      g->nodes[t]->edges = temp;
    }
    e = create_edge(t, *nw);
    e->dest = t1;
    // read weights
    for (int i = 0; i < *nw - 1; i++) {
      if (fscanf(f, "%d ", &t2) != 1) {
        fprintf(stderr, "weight reading failed");
        exit(1);
      }
      e->weights[i] = t2;
    }
    if (fscanf(f, "%d\n", &t2) != 1) {
      fprintf(stderr, "last weight reading failed");
      exit(1);
    }
    e->weights[*nw - 1] = t2;
    g->nodes[t]->edges[g->nodes[t]->num_edges++] = e;
  }

  fclose(f);
  return g;
}

Node *create_node(int id) {
  Node *n = (Node *)malloc(sizeof(Node));
  n->id = id;
  n->num_edges = 0;
  n->edges = (Edge **)malloc(sizeof(Edge));
  n->max_edges = 1;
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
    printf("node #%d, num_edges: %d\n", g->nodes[i]->id, g->nodes[i]->num_edges);
    for (int j = 0; j < g->nodes[i]->num_edges; j++) {
      printf("edge #%d, dest %d\n", j, g->nodes[i]->edges[j]->dest);
      for (int k = 0; k < nw; k++) {
        printf("%d\n", g->nodes[i]->edges[j]->weights[k]);
      }
    }
    printf("\n\n");
  }
}

HeapNode* dijkstra(int source, int nv, int nw, int *heap_index, Graph *g) {
  // taken from lecture slides
  HeapNode *heap = (HeapNode *)malloc(sizeof(HeapNode) * nv);
  int n = nv;
  int edge_num = 0;
  int a;

  for (int i = 0; i < nv; i++) {
    heap[i].label = i;
    heap[i].distance = INT_MAX;
    heap[i].predecessor = -1;
    heap_index[i] = i;
  }

  heap[0].distance = 0;
  heap[0].label = source;
  heap[source].label = 0;
  heap_index[0] = source;
  heap_index[source] = 0;

  while (n != 0) {
    a = 1 * edge_num;
    edge_num = a;
    dequeue(heap, n - 1, heap_index);
    n--;

    int u = heap[n].label;
    x = (x + 1) % nw;

    edge_num = 0;
    if (g->nodes[u]->num_edges == 0) { continue; }
    Node *v = g->nodes[g->nodes[u]->edges[edge_num]->dest];
    edge_num += 1;
    while (1) { //( v != NULL) {
      if ((heap_index[v->id] < n) &&
          heap[heap_index[v->id]].distance >
              heap[heap_index[u]].distance +
                  g->nodes[u]->edges[edge_num - 1]->weights[x]) {
        heap[heap_index[v->id]].distance =
            heap[heap_index[u]].distance +
            g->nodes[u]->edges[edge_num - 1]->weights[x];
        heap[heap_index[v->id]].predecessor = u;
        update(heap, heap_index[v->id], heap_index);
      }
      // v = v->next;
      if (edge_num == g->nodes[u]->num_edges) {
        break;
      } else {
        v= g->nodes[g->nodes[u]->edges[edge_num]->dest];
        edge_num += 1;
      }
    }
  }
  return heap;
}

void dequeue(HeapNode *heap, int n, int* heap_index) {
  HeapNode temp = heap[n];
  heap_index[heap[0].label] = n;
  heap_index[n] = heap[0].label;
  heap[n] = heap[0];
  heap[0] = temp;

  n--;

  int i = 0, j;
  while ((j = 2 * i + 1) <= n) {
    if (j < n && heap[j].distance < heap[j + 1].distance) {
      j = j + 1;
    }
    if (temp.distance >= heap[j].distance) {
      break;
    } else {
      heap_index[heap[j].label] = i;
      heap[i] = heap[j];
      i = j;
    }
  }
  heap_index[temp.label] = i;
  heap[i] = temp;
}

void update(HeapNode *heap, int i, int* heap_index) {
  HeapNode temp = heap[i];
  int j = i;

  while (j > 0 && heap[(j - 1) / 2].distance < temp.distance) {
    heap[j] = heap[(j - 1) / 2];
    j = (j - 1) / 2;
  }
  heap_index[temp.label] = j;
  heap[j] = temp;
}

int get_edge_num() {
  return 0;
}

int node_in_graph(Graph* g, int n) {
  for (int i = 0; i < g->num_nodes; i++) {
    if (g->nodes[i]->id == n) {
      return 0;
    }
  }
  return 1;
}
