#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert
static int _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *))
{
    int cmp = compare(newPtr->dataPtr, root->dataPtr);

    if (cmp > 0){
        if(root->right == NULL){
            root->right = newPtr;
            return 0;
        }
        else
            return _insert(root->right, newPtr, compare, callback);
    }
    else if (cmp < 0){
        if(root->left == NULL){
            root->left = newPtr;
            return 0;
        }
        return _insert(root->left, newPtr, compare, callback);
    }
    else{
        callback(root->dataPtr);
        return 1;
    }
}

// used in BST_Insert
static NODE *_makeNode( void *dataInPtr)
{
    NODE *newnode = (NODE*)malloc(sizeof(NODE));
    if (newnode == NULL) return NULL;

    newnode->dataPtr = dataInPtr;
    newnode->left = NULL;
    newnode->right = NULL;

    return newnode;
}

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *))
{
    if(!root) return;

    _destroy(root->left, callback);
    _destroy(root->right, callback);

    callback(root->dataPtr);
    free(root);
}

// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *))
{
    if(!root) return root;

    int cmp = compare(keyPtr, root->dataPtr);

    if(cmp > 0){
        root->right = _delete(root->right, keyPtr, dataOutPtr, compare);
    }
    else if(cmp < 0){
        root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
    }
    else{
        *dataOutPtr = root->dataPtr;
        if(root->right == NULL){
            NODE *temp = root->left;
            free(root);
            root = temp;
        }
        else if(root->left == NULL){
            NODE *temp = root->right;
            free(root);
            root = temp;
        }
        else{
            NODE *pLoc = root->right;
            NODE *pPre = NULL;

            while (pLoc->left != NULL) {
                pPre = pLoc;
                pLoc = pLoc->left;
            }

            if (pPre != NULL){
                pPre->left = pLoc->right;
                pLoc->right = root->right;
            }

            pLoc->left = root->left;
            free(root);
            root = pLoc;
        }
    }
    return root;
}

// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *))
{
    if(root == NULL) return NULL;

    int cmp = compare(keyPtr, root->dataPtr);

    if(cmp < 0)
        return _search(root->left, keyPtr, compare);
    else if (cmp > 0)
        return _search(root->right, keyPtr, compare);
    else
        return root;
}

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *))
{
    if(!root) return;

    _traverse(root->left, callback);
    callback(root->dataPtr);
    _traverse(root->right, callback);
}

// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *))
{
    if(!root) return;

    _traverseR(root->right, callback);
    callback(root->dataPtr);
    _traverseR(root->left, callback);
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *))
{
    if(!root) return;
    
    _inorder_print(root->right, level + 1, callback);

    for(int i = 0; i < level; i++)
        printf("\t");
    callback(root->dataPtr);

    _inorder_print(root->left, level + 1, callback);
}


////////////////////////////////////////////////////////////////////////////////
// bst.h function declarations

TREE *BST_Create( int (*compare)(const void *, const void *))
{
    TREE *newtree = (TREE*)malloc(sizeof(TREE));
    if (newtree == NULL) return NULL;

    newtree->compare = compare;
    newtree->count = 0;
    newtree->root = NULL;

    return newtree;
}

void BST_Destroy( TREE *pTree, void (*callback)(void *))
{
    _destroy(pTree->root, callback);
    free(pTree);
}

int BST_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *))
{
    NODE *newnode = _makeNode(dataInPtr);
    if (!newnode) return 0;

    if(pTree->root == NULL){
        pTree->root = newnode;
        pTree->count++;
        return 1;
    }

    int gom = _insert(pTree->root, newnode, pTree->compare, callback);

    if(gom){
        free(newnode);
        return 2;
    }
    else{
        pTree->count++;
        return 1;
    }
}

void *BST_Delete( TREE *pTree, void *keyPtr)
{
    void *gom = NULL;

    pTree->root = _delete(pTree->root, keyPtr, &gom, pTree->compare);
    if(gom != NULL) pTree->count--;

    return gom;
}

void *BST_Search( TREE *pTree, void *keyPtr)
{
    NODE *targetnode = _search(pTree->root, keyPtr, pTree->compare);
    if (targetnode == NULL) return NULL;

    return targetnode->dataPtr;
}

void BST_Traverse( TREE *pTree, void (*callback)(const void *))
{
    _traverse(pTree->root, callback);
}

void BST_TraverseR( TREE *pTree, void (*callback)(const void *))
{
    _traverseR(pTree->root, callback);
}

void printTree( TREE *pTree, void (*callback)(const void *))
{
    _inorder_print(pTree->root, 0, callback);
}

int BST_Count( TREE *pTree)
{
    return pTree->count;
}