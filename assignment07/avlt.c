#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y)	(((x) > (y)) ? (x) : (y))

static int getHeight( NODE *root);

static NODE *rotateRight( NODE *root);

static NODE *rotateLeft( NODE *root);

// internal functions (not mandatory)
// used in AVLT_Insert
// return 	pointer to root
static NODE *_insert(NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *), int *duplicated) {
    if (root == NULL) {
        return newPtr;
    }

    int cmp = compare(newPtr->dataPtr, root->dataPtr);

    if (cmp > 0) {
        root->right = _insert(root->right, newPtr, compare, callback, duplicated);    
    } else if (cmp < 0) {
        root->left = _insert(root->left, newPtr, compare, callback, duplicated);
    } else {
        callback(root->dataPtr);
        *duplicated = 1;
        return root;
    }

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    
    if (getHeight(root->left) > getHeight(root->right) + 1) {
        if (compare(newPtr->dataPtr, root->left->dataPtr) < 0) {
            return rotateRight(root);
        } else {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
    } else if (getHeight(root->right) > getHeight(root->left) + 1) {
        if (compare(newPtr->dataPtr, root->right->dataPtr) > 0) {
            return rotateLeft(root);
        } else {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
    }
    
    return root;
}

// used in AVLT_Insert
static NODE *_makeNode( void *dataInPtr)
{
    NODE *newnode = (NODE*)malloc(sizeof(NODE));
    if (newnode == NULL) return NULL;

    newnode->dataPtr = dataInPtr;
    newnode->left = NULL;
    newnode->right = NULL;
    newnode->height = 0;

    return newnode;
}

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *))
{
    if (root == NULL) return;

    _destroy(root->left, callback);
    _destroy(root->right, callback);

    callback(root->dataPtr);
    free(root);
}


// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *))
{
    if(!root) return NULL;
	int res = compare(root->dataPtr, keyPtr);
	if(res == 0){
		NODE *_del_root = root;
		*dataOutPtr = root->dataPtr;
		if(!root->left){
			root=root->right;
			free(_del_root);
		}
		else if(!root->right){
			root=root->left;
			free(_del_root);
		}
		
		else{
			NODE *pLoc = root->right;
			while(pLoc->left) pLoc = pLoc->left;
			void* temp;
			root->right= _delete(root->right, pLoc->dataPtr, &temp, compare);
			root->dataPtr = temp;
			root->height = max(getHeight(root->left), getHeight(root->right))+1;
		}
		
		if(!root) return NULL;
	}
	/// 아래 균형 맞춰줘야함
	else if(res > 0) root->left = _delete(root->left, keyPtr, dataOutPtr, compare);	
	else root->right = _delete(root->right, keyPtr, dataOutPtr, compare);	
    
    // if unbalanced, leftbalance
    if (getHeight(root->left) > getHeight(root->right) + 1) {
	    // LR situation
        if (getHeight(root->left->left) + 1 < getHeight(root->left->right)) {
        root->left = rotateLeft(root->left);
        }
        // LL situation
        root = rotateRight(root); // ptr of new root
    }
    // else if unbalanced, rightbalance 
    else if (getHeight(root->right) > getHeight(root->left) + 1) {
		// RL situation
        if (getHeight(root->right->left) > getHeight(root->right->right) + 1){
		    root->right = rotateRight(root->right);
        } 
        // RR situation
        root = rotateLeft(root); // ptr of new root
    }

    // fix height and balancing with rotation
	// max function should be definded previously
	// #define max(a, b) ((a) > (b) ? (a) : (b))
    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    
    return root;
}

// used in AVLT_Search
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

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *))
{
    if (root == NULL) return;

    _traverse(root->left, callback);
    callback(root->dataPtr);
    _traverse(root->right, callback);
}

// used in AVLT_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *))
{
    if (root == NULL) return;

    _traverseR(root->right, callback);
    callback(root->dataPtr);
    _traverseR(root->left, callback);
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *))
{
    if (root == NULL) return;

    _inorder_print(root->right, level + 1, callback);

    for(int i = 0; i < level; i++){
        printf("\t");
    }
    callback(root->dataPtr);
    
    _inorder_print(root->left, level + 1, callback);
}

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root)
{
    if (root == NULL) 
        return -1;
    else 
        return root->height;
}

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight( NODE *root)
{
    if(root == NULL || root->left == NULL) return root;

    NODE *newroot = root->left;
    root->left = newroot->right;
    newroot->right = root;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    newroot->height = max(getHeight(newroot->left), getHeight(newroot->right)) + 1;

    return newroot;
}

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root)
{
    if(root == NULL || root->right == NULL) return root;

    NODE *newroot = root->right;
    root->right = newroot->left;
    newroot->left = root;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    newroot->height = max(getHeight(newroot->left), getHeight(newroot->right)) + 1;

    return newroot;
}


////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *AVLT_Create( int (*compare)(const void *, const void *))
{
    TREE *newtree = (TREE*)malloc(sizeof(TREE));
    if (newtree == NULL) return NULL;

    newtree->compare = compare;
    newtree->count = 0;
    newtree->root = NULL;

    return newtree;
}

/* Deletes all data in tree and recycles memory
*/
void AVLT_Destroy( TREE *pTree, void (*callback)(void *))
{
    _destroy(pTree->root, callback);
    free(pTree);
}

/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	1 success
			0 overflow
			2 if duplicated key
*/
int AVLT_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *))
{
    NODE *newnode = _makeNode(dataInPtr);
    if (newnode == NULL) return 0;

    int dup = 0;
    pTree->root = _insert(pTree->root, newnode, pTree->compare, callback, &dup);

    if (dup == 1) {
        free(newnode);
        return 2;
    }

    pTree->count++;
    return 1;
}

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Delete( TREE *pTree, void *keyPtr)
{
    if(pTree->root == NULL) return NULL;
    void *gom = NULL;

    pTree->root = _delete(pTree->root, keyPtr, &gom, pTree->compare);
    if(gom != NULL) pTree->count--;

    return gom;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Search( TREE *pTree, void *keyPtr)
{
    NODE *target = _search(pTree->root, keyPtr, pTree->compare);
    if (target == NULL) return NULL;

    return target->dataPtr;
}

/* prints tree using inorder traversal
*/
void AVLT_Traverse( TREE *pTree, void (*callback)(const void *))
{
    _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
*/
void AVLT_TraverseR( TREE *pTree, void (*callback)(const void *))
{
    _traverseR(pTree->root, callback);
}

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *))
{
    _inorder_print(pTree->root, 0, callback);
}

/* returns number of nodes in tree
*/
int AVLT_Count( TREE *pTree)
{
    return pTree->count;
}

/* returns height of the tree
*/
int AVLT_Height( TREE *pTree)
{
    if (pTree->root == NULL) return -1;
    return pTree->root->height;
}