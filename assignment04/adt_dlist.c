#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr)
{
	NODE *newnode = (NODE*)malloc(sizeof(NODE));
	if (newnode == NULL) return 0;

	newnode->dataPtr = dataInPtr;
	newnode->rlink = NULL;
	newnode->llink = NULL;
	pList->count++;

	if(pPre == NULL){
		newnode->rlink = pList->head;

		if (pList->head != NULL) pList->head->llink = newnode;
	
		pList->head = newnode;

		if (pList->rear == NULL) pList->rear = newnode;
	}
	else{
		newnode->rlink = pPre->rlink;
		newnode->llink = pPre;

		if (pPre->rlink != NULL) pPre->rlink->llink = newnode;

        pPre->rlink = newnode;

        if (pPre == pList->rear) pList->rear = newnode;
	}

	return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr)
{
	pList->count--;
	*dataOutPtr = pLoc->dataPtr;

	if (pLoc->llink == NULL){
		pList->head = pLoc->rlink;

        if (pLoc->rlink != NULL)
            pLoc->rlink->llink = NULL;
        else
            pList->rear = NULL;
	}
	else if (pLoc->rlink == NULL){
		pPre->rlink = NULL;
        pList->rear = pPre;
	}
	else{
		pLoc->llink->rlink = pLoc->rlink;
        pLoc->rlink->llink = pLoc->llink;
	}

	free(pLoc);
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu)
{
    *pPre = NULL;
    *pLoc = pList->head;

    while (*pLoc != NULL && pList->compare(pArgu, (void *)(*pLoc)->dataPtr) > 0)
    {
        *pPre = *pLoc;
        *pLoc = (*pLoc)->rlink;
    }

    if (*pLoc != NULL && pList->compare(pArgu, (void *)(*pLoc)->dataPtr) == 0)
        return 1;
    else
        return 0;
}


////////////////////////////////////////////////////////////////////////////////
// function declarations

LIST *createList( int (*compare)(const void *, const void *))
{
	LIST *newlist = (LIST*)malloc(sizeof(LIST));
	if (newlist == NULL) return NULL;

	newlist->count = 0;
	newlist->head = NULL;
	newlist->rear = NULL;
	newlist->compare = compare;

	return newlist;
}

void destroyList( LIST *pList, void (*callback)(void *))
{
	NODE *pNode = pList->head;
	NODE *temp;

	while (pNode != NULL){
		temp = pNode;
		pNode = pNode->rlink;
        callback(temp->dataPtr);
        free(temp);
	}

	free(pList);
}

int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *))
{
    NODE *pPre = NULL;
    NODE *pLoc = NULL;

    if (_search(pList, &pPre, &pLoc, dataInPtr)) {
        callback(pLoc->dataPtr);
        return 2;
    } 
    else {
        return _insert(pList, pPre, dataInPtr);
    }
}

int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr)
{
	NODE *pPre = NULL;
	NODE *pLoc = NULL;

	if(_search(pList, &pPre, &pLoc, keyPtr)){
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;
	}
	else return 0;
}

int searchNode( LIST *pList, void *pArgu, void **dataOutPtr)
{
	NODE *pPre = NULL;
	NODE *pLoc = NULL;

	int found = _search(pList, &pPre, &pLoc, pArgu);
    
	if(found) 
		*dataOutPtr = pLoc->dataPtr;
	else
        *dataOutPtr = NULL;

	return found;
}

int countList( LIST *pList)
{
	return pList->count;
}

int emptyList( LIST *pList)
{
	if (pList->count == 0) return 1;
	else return 0;
}

void traverseList( LIST *pList, void (*callback)(const void *))
{
	NODE* next = pList->head;
	while(next != NULL){
		callback(next->dataPtr);
		next = next->rlink;
	}
}

void traverseListR( LIST *pList, void (*callback)(const void *))
{
	NODE* before = pList->rear;
	while(before != NULL){
		callback(before->dataPtr);
		before = before->llink;
	}
}

