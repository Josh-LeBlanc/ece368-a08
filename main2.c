#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct edge {
  int w;
  unsigned short d;
} edge;
typedef struct gnode {
  unsigned short id;
  int t;
  int num_edges;
  edge** edges;
} gnode;
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
typedef struct hnode {
  unsigned short label;
  long long distance;
  short predecessor;
} hnode;

Graph* get_inputs(char**, int*, int*);
Node *create_node(int);
Edge *create_edge(int, int);
gnode** create_new_graph(Graph*, int, int);
void printng(gnode**, int, int);
void print_graph(Graph*, int);
void dijkstra(gnode**, int, int);
void dequeue(hnode*, int, int*);
void enqueue(hnode*, int, int*, int);
void print_heap(hnode*, int);
void print_path(hnode*, int*, int, int, int);
void print_heap_index(int*, int);
void free_nng(gnode**, int);

int main(int argc, char** argv) {
  int nv = 0, nw = 0;

  Graph* g = get_inputs(argv, &nv, &nw);
  // print_graph(g, nw);
  gnode** nng = create_new_graph(g, nv, nw);
  // printng(nng, nv, nw);

  dijkstra(nng, nv, nw);

  free_nng(nng, nv * nw);
  for (int i = 0; i < g->num_nodes; i++) {
    for (int j = 0; j < g->nodes[i]->num_edges; j++) {
      free(g->nodes[i]->edges[j]->weights);
      free(g->nodes[i]->edges[j]);
    }
    free(g->nodes[i]->edges);
    free(g->nodes[i]);
  }
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

gnode** create_new_graph(Graph* g, int nv, int nw) {
  gnode** nng = (gnode**)malloc(sizeof(gnode) * nv * nw);

  for (int i = 0; i < nv; i++) {
    for (int j = 0; j < nw; j++) {
      nng[nw*i+j] = (gnode*)malloc(sizeof(gnode));
      nng[nw*i+j]->id = i;
      nng[nw*i+j]->t = j;
      nng[nw*i+j]->num_edges = g->nodes[i]->num_edges;
      nng[nw*i+j]->edges = (edge**)malloc(sizeof(edge) * g->nodes[i]->num_edges);
      for (int k = 0; k < g->nodes[i]->num_edges; k++) {
        nng[nw*i+j]->edges[k] = (edge*)malloc(sizeof(edge));
        nng[nw*i+j]->edges[k]->d = g->nodes[i]->edges[k]->dest * nw + ((j + 1) % nw);
        nng[nw*i+j]->edges[k]->w = g->nodes[i]->edges[k]->weights[j];
      }
    }
  }

  return nng;
}

void printng(gnode** nng, int nv, int nw) {
  for(int i = 0; i < nv; i++) {
    for(int j = 0; j < nw; j++) {
      printf("node: %d, time: %d, num_edges: %d\n", nng[nw*i+j]->id, nng[nw*i+j]->t, nng[nw*i+j]->num_edges);
      for (int k = 0; k < nng[nw*i+j]->num_edges; k++) {
        printf("edge: %d, dest: %d, weight: %d\n", k, nng[nw*i+j]->edges[k]->d, nng[nw*i+j]->edges[k]->w);
      }
      printf("\n\n");
    }
  }
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

void dijkstra(gnode** nng, int nv, int nw) {
  int source = 0, dest = 0;

  while(scanf("%d %d", &source, &dest) == 2) {
    int* heap_index = (int*)malloc(sizeof(int) * nv * nw);
    hnode* heap = (hnode*)malloc(sizeof(hnode) * nv * nw);
    int n = nw * nv;
    for (int i = 0; i < nw * nv; i++) {
      heap[i].label = i;
      heap[i].distance = INT_MAX;
      heap[i].predecessor = -1;
      heap_index[i] = i;
    }

    heap[0].distance = 0;
    heap[0].label = source * nw;
    heap[source * nw].label = 0;
    heap_index[0] = source * nw;
    heap_index[source * nw] = 0;
    // print_heap_index(heap_index, nv * nw);
    // print_heap(heap, nv * nw);

    while (n != 0) {
      int num_edge = 0;
      dequeue(heap, n-1, heap_index);
      // print_heap_index(heap_index, nv * nw);
      // print_heap(heap, nv * nw);
      n--;

      int u = heap[n].label;

      if (nng[u]->num_edges == 0 || heap[n].distance == INT_MAX) { continue; }
      edge* v = nng[u]->edges[num_edge++];

      while (1) {
        if (heap_index[v->d] < n && heap[heap_index[v->d]].distance > heap[heap_index[u]].distance + v->w) {
        // if (heap[heap_index[v->d]].distance > heap[heap_index[u]].distance + v->w) {
          heap[heap_index[v->d]].distance = heap[heap_index[u]].distance + v->w;
          heap[heap_index[v->d]].predecessor = u;
          enqueue(heap, heap_index[v->d], heap_index, n);
          // print_heap_index(heap_index, nv * nw);
          // print_heap(heap, nv * nw);
        }
        if (num_edge == nng[u]->num_edges) { break; }
        else { v = nng[u]->edges[num_edge++]; }
      }
    }
    // print_heap(heap, nv * nw);
    int min_dist = INT_MAX;
    hnode path = {0};
    for (int i = 0; i < nw; i++) {
      if (heap[heap_index[dest*nw+i]].distance < min_dist) {
        min_dist = heap[heap_index[dest*nw+i]].distance;
        path = heap[heap_index[dest*nw+i]];
      }
    }
    // print_heap_index(heap_index, nv * nw);
    // print_heap(heap, nv * nw);
    print_path(heap, heap_index, heap_index[path.label], path.label, nw);
    free(heap_index);
    free(heap);
  }

}

void dequeue(hnode* arr, int n, int* heap_index) { //n is the last index
  hnode temp = arr[n]; //exchange the root and the last node
  int a = temp.label * 1;
  temp.label = a;
  heap_index[arr[0].label] = n;
  heap_index[temp.label] = 0;
  arr[n] = arr[0];
  arr[0] = temp;
  n--; //decrease the heap size
  
  int i = 0;
  while (arr[i].distance > arr[i+1].distance) {
    temp = arr[i];
    heap_index[arr[i+1].label] = i;
    heap_index[temp.label] = i+1;
    arr[i] = arr[i+1];
    arr[i+1] = temp;
    i++;
  }

  /*
  int i = 0, j;
  while ((j = 2*i+1) <= n) { //left child exists?
    if (j < n && arr[j].distance < arr[j+1].distance) {// binary search
      j = j+1; //pick right child
    }
    if (temp.distance >= arr[j].distance) { break; }
    else {
      heap_index[arr[j].label] = i;
      arr[i] = arr[j];
      i = j;
    }
  }
  heap_index[temp.label] = i;
  arr[i] = temp;
  */

}

void enqueue(hnode* arr, int i, int* heap_index, int n) { // upward heapify
  hnode temp; // new element at index i
  while (i < n && arr[i].distance > arr[i+1].distance) {
    temp = arr[i];
    heap_index[arr[i+1].label] = i;
    heap_index[arr[i].label] = i+1;
    arr[i] = arr[i+1];
    arr[i+1] = temp;
    i++;
  }
  while (i > 0 && arr[i].distance < arr[i-1].distance) {
    temp = arr[i];
    heap_index[arr[i-1].label] = i;
    heap_index[arr[i].label] = i-1;
    arr[i] = arr[i-1];
    arr[i-1] = temp;
    i--;
  }
}

void print_heap(hnode* heap, int size) {
  printf("print heap:\n");
  for (int i = 0; i < size; i++) {
    printf("label: %d, distance: %lld, predecessor: %d\n", heap[i].label, heap[i].distance, heap[i].predecessor);
  }
  printf("\n\n");
}

void print_heap_index(int* hi, int size) {
  printf("print index:\n");
  for (int i = 0; i < size; i++) {
    printf("label: %d, index: %d\n", i, hi[i]);
  }
}

void print_path(hnode* heap, int* heap_index, int p, int dest, int nw) {
  if (heap[p].predecessor == -1) {
    printf("%d ", heap[p].label/nw);
    return;
  }
  print_path(heap, heap_index, heap_index[heap[p].predecessor], dest, nw);
  if ((heap[p].label) == dest) {
    printf("%d\n", heap[p].label/nw);
  } else {
    printf("%d ", heap[p].label/nw);
  }
}

void free_nng(gnode** nng, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < nng[i]->num_edges; j++) {
      free(nng[i]->edges[j]);
    }
    free(nng[i]->edges);
    free(nng[i]);
  }
  free(nng);
}
