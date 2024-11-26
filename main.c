#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct NEdge {
  int id;
  int w;
  struct NEdge** edges;
  int num_edges;
} NEdge;
typedef struct NGraph {
  NEdge** sources;
  int num_sources;
} NGraph;
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
  int t;
  int* predecessors;
} HeapNode;
typedef struct Qnode {
  int id;
  int pred;
  int d;
  int t;
  struct Qnode* next;
  struct Qnode* prev;
} Qnode;
typedef struct {
  Qnode* head;
  Qnode* tail;
} Queue;

Graph *get_inputs(char **, int *, int *);
Node *create_node(int);
Edge *create_edge(int, int);
void print_graph(Graph *, int);
HeapNode* dijkstra(int, int, int, int *, Graph *);
void dequeue(HeapNode *, int, int *);
void update(HeapNode *, int, int *);
Queue* create_queue(int);
Qnode* dq(Queue*);
void nq(Queue*, Qnode*);
void print_path(HeapNode*, int, int, int, int);
NGraph* create_new_graph(Graph*, int, int);
void add_edges(NEdge*, Node*, Graph*, int, int);
void print_new_graph(NGraph*);

int main(int argc, char **argv) {
  int nv = 0; // num vertices
  int nw = 0; // num weights per edge
  HeapNode* heap = 0;

  Graph *g = get_inputs(argv, &nv, &nw);
  NGraph* ng = create_new_graph(g, nv, nw);
  print_new_graph(ng);
  exit(1);
  int *heap_index = (int *)malloc(sizeof(int) * nv);
  // print_graph(g, nw);
  int psource = -1, source = -2, dest = -1;
  while (1) {
    // printf("input source dest: ");
    if (scanf("%d %d", &source, &dest) != 2) {
      break;
      fprintf(stderr, "error, please enter source and destination in the format 'source dest':");
      exit(1);
    }
    if (source != psource) {
      if (heap) {
        for (int i = 0; i < nv; i++) {
          free(heap[i].predecessors);
        }
        free(heap);
      }
      heap = dijkstra(source, nv, nw, heap_index, g);
    }

    print_path(heap, source, dest, nw, heap[dest].t);
    printf("\n");
    psource = source;
  }

  // free everything

  free(heap_index);
  for (int i = 0; i < g->num_nodes; i++) {
    for (int j = 0; j < g->nodes[i]->num_edges; j++) {
      free(g->nodes[i]->edges[j]->weights);
      free(g->nodes[i]->edges[j]);
    }
    free(g->nodes[i]->edges);
    free(g->nodes[i]);
    free(heap[i].predecessors);
  }
  free(heap);
  free(g->nodes);
  free(g);
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
      free(g->nodes[t]->edges);
      free(g->nodes[t]);
      g->nodes[t] = create_node(t);
      g->num_nodes++;
    }
    // if node 2 not in graph, create it
    if (g->nodes[t1]->id == -1) {
      free(g->nodes[t1]->edges);
      free(g->nodes[t1]);
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

/*
HeapNode* dijkstra(int source, int nv, int nw, int *heap_index, Graph *g) {
  // taken from lecture slides. horrible idea
  HeapNode *heap = (HeapNode *)malloc(sizeof(HeapNode) * nv);
  int n = nv;
  int edge_num = 0;
  int a;
  x = nw - 1;

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
      if (heap[heap_index[v->id]].distance >
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
*/

HeapNode* dijkstra(int source, int nv, int nw, int *heap_index, Graph *g) {
  // set up heap and heap_index
  HeapNode *heap = (HeapNode *)malloc(sizeof(HeapNode) * nv);
  for (int i = 0; i < nv; i++) {
    heap[i].label = i;
    heap[i].distance = INT_MAX;
    heap[i].predecessors = (int*)malloc(sizeof(int) * nw);
    heap[i].t = -1;
    for (int j = 0; j < nw; j++) {
      heap[i].predecessors[j] = -1;
    }
    heap_index[i] = i;
  }

  Queue* q = create_queue(source);

  // nq all reachable nodes
  while (q->head != NULL) {
    for(int i = 0; i < g->nodes[q->head->id]->num_edges; i++) {
      Qnode* tqn = (Qnode*)malloc(sizeof(Qnode));
      tqn->id = g->nodes[q->head->id]->edges[i]->dest;
      tqn->t = q->head->t + 1;
      tqn->d = q->head->d + g->nodes[q->head->id]->edges[i]->weights[q->head->t % nw];
      tqn->pred = q->head->id;
      if (tqn->d < heap[tqn->id].distance) {
        heap[heap_index[tqn->id]].predecessors[tqn->t % nw] = q->head->id;
      }
      nq(q, tqn);
    }
    // dq
    Qnode* qn = dq(q);
    int temp = qn->pred;
    qn->pred = temp * 1;
    if (heap[heap_index[qn->id]].distance > qn->d) {
      heap[heap_index[qn->id]].distance = qn->d;
      // heap[heap_index[qn->id]].predecessors[qn->t % nw] = qn->pred;
      heap[heap_index[qn->id]].t = qn->t;
    }
    free(qn);
  }
  free(q);
  return heap;
}

/*
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
*/

/*
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
*/

/*
int node_in_graph(Graph* g, int n) {
  for (int i = 0; i < g->num_nodes; i++) {
    if (g->nodes[i]->id == n) {
      return 0;
    }
  }
  return 1;
}
*/

Queue* create_queue(int src) {
  Queue* q = (Queue*)malloc(sizeof(Queue));
  Qnode* qn = (Qnode*)malloc(sizeof(Qnode));
  qn->id = src;
  qn->next = NULL;
  qn->prev = NULL;
  qn->t = 0;
  qn->d = 0;
  q->head = qn;
  q->tail = qn;
  return q;
}

Qnode* dq (Queue* q) {
  if (q->tail == NULL) {
    fprintf(stderr, "dq from empty q");
    exit(1);
  }
  Qnode* temp = q->head;
  if (q->tail == q->head) {
    q->tail = NULL;
    q->head = NULL;
  }
  if (q->head != NULL) {
    q->head = q->head->next;
  }
  return temp;
}

void nq (Queue* q, Qnode* qn) {
  q->tail->next = qn;
  qn->prev = q->tail;
  q->tail = qn;
}

void print_path(HeapNode* heap, int source, int dest, int nw, int t) {
  if (dest == source) {
    printf("%d ", dest);
    return;
  }
  print_path(heap, source, heap[dest].predecessors[t % nw], nw, t - 1);
  printf("%d ", dest);
}

NGraph* create_new_graph(Graph* g, int nv, int nw) {
  // for each node in the graph we will start from 0 and travers
  NGraph* ng = (NGraph*)malloc(sizeof(NGraph));
  ng->num_sources = nv;
  ng->sources = (NEdge**)malloc(sizeof(NEdge) * nv);
  for (int i = 0; i < g->num_nodes; i++) {
    int t = 0;   
    ng->sources[i] = (NEdge*)malloc(sizeof(NEdge));
    ng->sources[i]->w = 0;
    ng->sources[i]->id = i;
    add_edges(ng->sources[i], g->nodes[i], g, t, nw);
  }
  return ng;
}

/*
void add_edges(NEdge* ne, Node* n, Graph* g, int t, int nw) {
  if (n->num_edges == 0) {
    ne->num_edges = 0;
    return;
  }
  ne->num_edges = n->num_edges;
  ne->edges = (NEdge**)malloc(sizeof(NEdge) * n->num_edges);
  for (int i = 0; i < n->num_edges; i++) {
    NEdge* new = (NEdge*)malloc(sizeof(NEdge));
    new->id = n->edges[i]->dest;
    new->w = n->edges[i]->weights[t];
    ne->edges[i] = new;
    if (g->nodes[n->edges[i]->dest]->num_edges > 0) {
      add_edges(ne->edges[i], g->nodes[n->edges[i]->dest], g, (t+1)%nw, nw);
    }
  }
}
*/

void add_edges(NEdge* ne, Node* n, Graph* g, int t, int nw) {
  if (n->num_edges == 0) {
    ne->num_edges = 0;
    return;
  }
  ne->num_edges = n->num_edges;
  ne->edges = (NEdge**)malloc(sizeof(NEdge) * n->num_edges);
  for (int i = 0; i < n->num_edges; i++) {
    NEdge* new = (NEdge*)malloc(sizeof(NEdge));
    new->id = n->edges[i]->dest;
    new->w = n->edges[i]->weights[t];
    ne->edges[i] = new;
    if (g->nodes[n->edges[i]->dest]->num_edges > 0) {
      add_edges(ne->edges[i], g->nodes[n->edges[i]->dest], g, (t+1)%nw, nw);
    }
  }
}

void print_new_graph(NGraph* ng) {
  printf("%d\n", ng->sources[0]->id);
}
