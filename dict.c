#include <stdio.h>
#include <ctype.h>

#include "dict.h"


int strlen_as_word(const char *str)
{
	int i, len = 0;
	char c;

	NOT(str);

	for (i = 0; str[i] != '\0'; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') 
			len++;
	}
	return len;
}


void word_from_str(letter_t *word, const char *str)
{
	int i, j;
	char c;

	NOT(word), NOT(str);

	for (i = 0, j = 0; str[i] != '\0'; i++) {
		c = toupper(str[i]);
		if (c >= 'A' && c <= 'Z') {
			word[j] = LETTER_A + c - 'A';
			j++;
		}
	}
}


void swap_words(letter_t **w0, int *len0, letter_t **w1, int *len1)
{
	int tmp_len;
	letter_t *tmp_w;

	NOT(w0), NOT(len0), NOT(w1), NOT(len1), NOT(*w0), NOT(*w1);

	tmp_len = *len0;
	tmp_w = *w0;
	*len0 = *len1;
	*w0 = *w1;
	*len1 = tmp_len; 
	*w1 = tmp_w;
}


bool load_dict(struct dict *dict, const char *filename)
{
	long i, j;
	FILE *f = NULL;
	char buf[BOARD_SIZE+1];

	NOT(dict), NOT(filename);

	f = fopen(filename,"r");
	if (f == NULL) {
		return false;
	}
	/* count */
	dict->num = 0;
	while(fgets(buf, BOARD_SIZE+1, f)) {
		if (strlen_as_word(buf) > 1) {
			dict->num ++;
		}
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	rewind(f);
	/* alloc */
	dict->word = alloc_mem(sizeof(letter_t*) * dict->num);
	NOT(dict->word);
	dict->len = alloc_mem(sizeof(long) * dict->num);
	NOT(dict->len);
	for (i = 0; i < dict->num; i++) {
		dict->word[i] = alloc_mem(sizeof(letter_t) * BOARD_SIZE);
		NOT(dict->word[i]);
	}
	/* assign */
	i = 0;
	for(i = 0; i < dict->num && fgets(buf, BOARD_SIZE+1, f); i++) {
		j = strlen_as_word(buf);
		if (j <= 1) {
			i--;
			continue;
		}
		word_from_str(dict->word[i], buf);
		dict->len[i] = j;
	}
	/* error check */
	if (ferror(f)) {
		fclose(f);
		return false;
	}
	fclose(f);
	/* sort */
	return true;
	for (i = 0; i < dict->num; i++) {
		for (j = 0; j < dict->num; j++) {
			if (cmp_word(dict->word[i], dict->len[i],
				     dict->word[j], dict->len[j]) == 1) {
				swap_words(&dict->word[i], &dict->len[i],
					  &dict->word[j], &dict->len[j]);
			}
		}
	}
	return true;
}


void unload_dict(struct dict *dict)
{
	long i;

	NOT(dict);

	free_mem(dict->len);
	for (i = 0; i < dict->num; i++) {
		free_mem(dict->word[i]);
	}
	free_mem(dict->word);
}


