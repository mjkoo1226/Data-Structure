#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void)
{
    TRIE *newnode = (TRIE*)malloc(sizeof(TRIE));
    if (!newnode) return NULL;

    newnode->index = -1; // 초기값 -1로 설정
    for (int i = 0; i < MAX_DEGREE; i++) {
        newnode->subtrees[i] = NULL; // 초기화 수정
    }

    return newnode;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root)
{
    if (!root) return;

    for (int i = 0; i < MAX_DEGREE; i++) {
        if (root->subtrees[i]) {
            trieDestroy(root->subtrees[i]);
        }
    }

    free(root); // 모든 하위 노드를 해제한 후 루트 노드 해제
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index)
{
	if(!*str){
		root->index = dic_index;
		return 1;
	}
	TRIE** sub = &root->subtrees[getIndex(*str)];
	if(!*sub){
		*sub = trieCreateNode();
		if(!*sub) return 0;
	}
	return trieInsert(*sub, str+1, dic_index);
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str)
{
	if(!*str){
		if(!root->subtrees[26]) 
			return -1;
		else
			return root->subtrees[26]->index;
	}

	if (!root) 
		return -1;
	else 
		return trieSearch(root->subtrees[getIndex(*str)], str+1);

}

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

static int trieList_main( TRIE *root, char *dic[], int count)
{
	if(root == NULL) 
		return count;
	if(root->index >= 0) 
		printf("[%d]%s\n", ++count, dic[root->index]);
	for(int i = 0; i < MAX_DEGREE; i++){
		count = trieList_main(root->subtrees[i], dic, count);
	}
	return count;
}

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[])
{
	if(!*str)
		trieList(root, dic);
	if(root == NULL) 
		return;
	triePrefixList(root->subtrees[getIndex(*str)], str + 1, dic);
}

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[])
{
    int len = strlen(str);
    for (int i = 0; i <= len; i++) {
        permuterms[i] = (char*)malloc(sizeof(char) * (len + 2));
        if (i == 0) {
            strcpy(permuterms[i], str);
            permuterms[i][len] = '$';
        } else {
            char f = permuterms[i - 1][0];
            strcpy(permuterms[i], permuterms[i - 1] + 1);
            permuterms[i][len] = f;
        }
        permuterms[i][len + 1] = 0;
    }
    return len + 1;
}

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size)
{
    for (int i = 0; i < size; i++) {
        free(permuterms[i]); // 동적으로 할당한 permuterm 메모리 해제
    }
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[])
{
    char temp[100];  // 문자열 복사본
    strcpy(temp, str);
    int len = strlen(temp);
    temp[len] = '$';

    for (int i = 0; i < len; i++) {
        char c = temp[0];
        memmove(temp, temp + 1, len + 1);
        temp[len] = c;
        if (c == '*') break;
    }

    temp[strchr(temp, '*') - temp] = 0;
    triePrefixList(root, temp, dic);
}


////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int num_words = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{	
		num_p = make_permuterms( str, permuterms);
		
		for (int i = 0; i < num_p; i++)
			trieInsert( permute_trie, permuterms[i], num_words);
		
		clear_permuterms( permuterms, num_p);
		
		dic[num_words++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( permute_trie, str);
			
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < num_words; i++)
		free( dic[i]);
	
	trieDestroy( permute_trie);
	
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
void trieList( TRIE *root, char *dic[])
{
	if (root == NULL) return;

	trieList_main(root, dic, 0);
}

