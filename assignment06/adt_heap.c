#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
   for heap_Insert function
*/
static void _reheapUp( HEAP *heap, int index)
{
    if (index == 0) return; // Root node, no need to reheap up

    int parent = (index - 1) / 2;
    if (heap->compare(heap->heapArr[index], heap->heapArr[parent]) > 0) {
        // Swap parent and child
        void *temp = heap->heapArr[parent];
        heap->heapArr[parent] = heap->heapArr[index];
        heap->heapArr[index] = temp;

        _reheapUp(heap, parent); // Recursive call for parent
    }
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
   for heap_Delete function
*/
static void _reheapDown( HEAP *heap, int index)
{
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;
    int largest = index;

    if (leftChild < heap->last && heap->compare(heap->heapArr[leftChild], heap->heapArr[largest]) > 0) {
        largest = leftChild;
    }
    if (rightChild < heap->last && heap->compare(heap->heapArr[rightChild], heap->heapArr[largest]) > 0) {
        largest = rightChild;
    }
    if (largest != index) {
        // Swap current node with the largest child
        void *temp = heap->heapArr[index];
        heap->heapArr[index] = heap->heapArr[largest];
        heap->heapArr[largest] = temp;

        _reheapDown(heap, largest); // Recursive call for the child
    }
}


HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2))
{
	HEAP *newheap = (HEAP*)malloc(sizeof(HEAP));
	if (newheap == NULL) return NULL;

	newheap->capacity = 10;
    newheap->compare = compare;
    newheap->last = 0;
    newheap->heapArr = (void **)malloc(sizeof(void *) * newheap->capacity);
    
    if (newheap->heapArr == NULL) {
        free(newheap);
        return NULL;
    }

    return newheap;
}

void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr))
{
	for (int i = 0; i < heap->last; i++){
      if (heap->heapArr[i] != NULL) {
         remove_data(heap->heapArr[i]);
      }
	}
	free(heap->heapArr);
	free(heap);
}

int heap_Insert( HEAP *heap, void *dataPtr)
{
    if (heap->last >= heap->capacity) {
        // Resize heap array
        heap->capacity *= 2;
        void **newHeapArr = realloc(heap->heapArr, sizeof(void*) * heap->capacity);
        if (newHeapArr == NULL) return 0; // Memory allocation failed
        heap->heapArr = newHeapArr;
    }

    heap->heapArr[heap->last] = dataPtr;
    _reheapUp(heap, heap->last);
    heap->last++;

    return 1; // Successful insertion
}

int heap_Delete( HEAP *heap, void **dataOutPtr)
{
   if (heap->last == 0) {
      *dataOutPtr = NULL; // Explicitly set to NULL
      return 0; // Heap is empty
   }

   *dataOutPtr = heap->heapArr[0];
   heap->heapArr[0] = heap->heapArr[heap->last - 1];
   heap->heapArr[heap->last - 1] = *dataOutPtr;
   heap->last--;

   _reheapDown(heap, 0);

   return 1; // Successful deletion
}

int heap_Empty(  HEAP *heap)
{
	if (heap->last == 0) return 1;
	else return 0;
}

void heap_Print( HEAP *heap, void (*print_func) (const void *data))
{
   for(int i = 0; i < heap->last; i++){
      print_func(heap->heapArr[i]);
   }
   printf("\n");
}

