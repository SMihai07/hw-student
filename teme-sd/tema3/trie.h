#ifndef __TRIE_H__
#define __TRIE_H__

#include <stdio.h>

typedef struct nod
{
	int isWord;
	struct nod *children[36];

} TNod, *TTrie;

TTrie initialize_node();
void add_word(TTrie root, char *word);
int find(TTrie root, char *word);
int numberOfChildren(TTrie node);
int delete(TTrie node, char *word);
char *find_longest_prefix(TTrie root, char *word);
void print_word(TTrie node, char *prefix, FILE *out, char *word, int len);
void find_all_with_prefix(TTrie root, char *prefix, FILE *out);
void mean_length_helper(TTrie node, char *prefix, char *word, int *sum, int *nr, int len);
void mean_length(TTrie root, char *prefix, FILE *out);
void erase_trie(TTrie *root);


#endif