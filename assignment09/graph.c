#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for an EDGE
typedef struct edge {
	struct vertex *dest; // pointer to destination vertex
	struct edge *next;
} EDGE;

// Structure for a Vertex
typedef struct vertex {
	int id;
	int visited; // visited flag for graph traversal
	EDGE *edges; // head of adjacency list
} VERTEX;

// Structure for a Graph
typedef struct {
	int numVertices; // number of vertices in graph
	VERTEX *vertices; // array to store vertices
} GRAPH;

// Stack structure for DFS
typedef struct {
	int *data; // array to store elements
	int top; // index of the top element
	int capacity;
} Stack;

// Queue structure for BFS
typedef struct {
	int *data; // array to store elements
	int front; // index of the front element
	int rear; // index of the rear element
	int capacity;
	int size; // current size of the queue
} Queue;

////////////////////////////////////////////////////////////////////////////////
// Function to create a stack
Stack *createStack(int capacity) {
	Stack *stack = (Stack*)malloc(sizeof(Stack));
	stack->data = (int*)malloc(capacity  *sizeof(int));
	stack->top = -1;
	stack->capacity = capacity;
	return stack;
}

////////////////////////////////////////////////////////////////////////////////
// Push an element onto the stack
void push(Stack *stack, int value) {
	if (stack->top == stack->capacity - 1) {
		printf("Stack overflow\n");
		return;
	}
	stack->data[++stack->top] = value;
}

////////////////////////////////////////////////////////////////////////////////
// Pop an element from the stack
int pop(Stack *stack) {
	if (stack->top == -1) {
		printf("Stack underflow\n");
		return -1;
	}
	return stack->data[stack->top--];
}

////////////////////////////////////////////////////////////////////////////////
// Check if the stack is empty
int isEmptyStack(Stack *stack) {
	return stack->top == -1;
}

////////////////////////////////////////////////////////////////////////////////
// Free the stack
void freeStack(Stack *stack) {
	free(stack->data);
	free(stack);
}

////////////////////////////////////////////////////////////////////////////////
// Function to create a queue
Queue *createQueue(int capacity) {
	Queue *queue = (Queue *)malloc(sizeof(Queue));
	queue->data = (int *)malloc(capacity * sizeof(int));
	queue->front = 0;
	queue->rear = -1;
	queue->capacity = capacity;
	queue->size = 0;
	return queue;
}

////////////////////////////////////////////////////////////////////////////////
// Function to check if the queue is empty
int isEmptyQueue(Queue *queue) {
	return queue->size == 0;
}

////////////////////////////////////////////////////////////////////////////////
// Function to check if the queue is full
int isFullQueue(Queue *queue) {
	return queue->size == queue->capacity;
}

////////////////////////////////////////////////////////////////////////////////
// Function to add an element to the queue (Enqueue)
void enqueue(Queue *queue, int value) {
	if (isFullQueue(queue)) {
		printf("Queue overflow\n");
		return;
	}
	queue->rear = (queue->rear + 1) % queue->capacity; // Circular increment
	queue->data[queue->rear] = value;
	queue->size++;
}

////////////////////////////////////////////////////////////////////////////////
// Function to remove an element from the queue (Dequeue)
int dequeue(Queue *queue) {
	if (isEmptyQueue(queue)) {
		printf("Queue underflow\n");
		return -1;
	}
	int value = queue->data[queue->front];
	queue->front = (queue->front + 1) % queue->capacity; // Circular increment
	queue->size--;
	return value;
}

////////////////////////////////////////////////////////////////////////////////
// Free the stack
void freeQueue(Queue *queue) {
	free(queue->data);
	free(queue);
}

////////////////////////////////////////////////////////////////////////////////
void printGraph(GRAPH *graph) {

	for (int i = 0; i < graph->numVertices; i++) {
		
		printf("vertex %d:", i+1);

		EDGE *edge = graph->vertices[i].edges;
		
		while(edge) {
			printf("->%d", edge->dest->id);
			edge = edge->next;
		}
		printf("\n");
	}
}


// Function to parse the .net file and build the graph
GRAPH *parseNetFile(const char *fileName);

// Function to create a graph with disjoint vertices
GRAPH *createGraph(int numVertices);

// Function to add an edge to the graph (insertion in an ordered list) 
void addEdge(GRAPH *graph, int src, int dest);

void destroyGraph(GRAPH *graph);

// Iterative DFS using a stack
void DFS(GRAPH *graph);

// BFS using a queue
void BFS(GRAPH *graph);

void resetVisitedFlag(GRAPH *graph);

////////////////////////////////////////////////////////////////////////////////
// Main function
int main(int argc, char **argv) {
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s .net_file\n", argv[0]);
		return 1;
	}
	
	GRAPH *graph = parseNetFile(argv[1]);

	printGraph(graph);

	// Perform DFS starting from the first vertex
	DFS(graph);

	// Perform BFS starting from the first vertex
	BFS(graph);
	
	destroyGraph(graph);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

// Function to parse the .net file and build the graph
GRAPH *parseNetFile(const char *fileName){
	FILE *fp;
	fp = fopen( fileName, "rt");
	if (fp == NULL) return NULL;
	
	char str[30];
	int N, a, b;
	fscanf(fp, "%s %d",str,&N);
	fscanf(fp, "%s", str);
	
	GRAPH *graph = createGraph(N);	

	while (fscanf( fp, "%d %d", &a, &b) != EOF){
		addEdge(graph, a-1, b-1);
		addEdge(graph, b-1, a-1);
	}
	fclose(fp);
	return graph;
}

// Function to create a graph with disjoint vertices
GRAPH *createGraph(int numVertices){
	GRAPH *graph = (GRAPH*)malloc(sizeof(GRAPH));
	if(graph == NULL) return NULL;
	
	graph->numVertices = numVertices;
	graph->vertices = (VERTEX*)malloc(sizeof(VERTEX) * numVertices);

	for(int i = 0; i < numVertices; i++){
		graph->vertices[i] = (VERTEX){i + 1, 0, NULL};
	}
	return graph;
}

// Function to add an edge to the graph (insertion in an ordered list) 
void addEdge(GRAPH *graph, int src, int dest){
	EDGE *pLoc = graph->vertices[src].edges;
	EDGE *pPre = NULL;
	EDGE *edge = (EDGE*)malloc(sizeof(EDGE));
	if (edge == NULL) return NULL;

	edge->dest = &graph->vertices[dest];
	edge->next = NULL;

	while(pLoc && pLoc->dest->id < dest+1){
		pPre = pLoc, pLoc = pLoc->next;
	}
	if(pPre == NULL){
		edge->next = graph->vertices[src].edges;
		graph->vertices[src].edges = edge;
	}
	else{
		edge->next = pPre->next, pPre->next = edge ;
	}
}

void destroyGraph(GRAPH *graph){
	for(int i = 0; i < graph->numVertices; i++){
		EDGE *ptr = graph->vertices[i].edges;
		EDGE *ret = NULL;

		while(ptr != NULL){
			ret = ptr->next;
			free(ptr);
			ptr = ret;
		}
	}
	free(graph->vertices);
	free(graph);
}

// Iterative DFS using a stack
void DFS(GRAPH *graph){
	Stack *stack = createStack(graph->numVertices);
	for(int i = 0; i < graph->numVertices; i++){
		if(graph->vertices[i].visited++) 
			continue;
		printf("Depth-First Traversal (Stack) starting from vertex %d:\n", i+1);
		push(stack, i);
		graph->vertices[i].visited = 1;
		while(!isEmptyStack(stack)){
			int top = pop(stack);
			printf("%d ",top+1);
			EDGE *edge = graph->vertices[top].edges;
			while(edge){
				int n = edge->dest->id;
				if(!graph->vertices[n-1].visited++){
					push(stack, n-1);
				}
				edge = edge->next;
			}
		}
		printf("\n");
	}
	freeStack(stack);
	resetVisitedFlag(graph);
}

// BFS using a queue
void BFS(GRAPH *graph){
	Queue *queue = createQueue(graph->numVertices);
	for(int i = 0; i < graph->numVertices; i++){
		if(graph->vertices[i].visited++) 
			continue;
		printf("Breadth-First Traversal starting from vertex %d:\n", i+1);
		enqueue(queue, i);
		graph->vertices[i].visited = 1;
		while(!isEmptyQueue(queue)){
			int front = dequeue(queue);
			printf("%d ",front+1);
			EDGE *edge = graph->vertices[front].edges;
			while(edge){
				int n = edge->dest->id;
				if(!graph->vertices[n-1].visited++) 
					enqueue(queue, n-1);
				edge = edge->next;
			}
		}
		printf("\n");
	}
	freeQueue(queue);
	resetVisitedFlag(graph);
}

void resetVisitedFlag(GRAPH *graph){
	int N = graph->numVertices;
	for(int i = 0; i < N; i++){
		graph->vertices[i].visited = 0;
	}
}