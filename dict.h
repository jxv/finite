#ifndef SCABS_DICT_H
#define SCABS_DICT_H


#include "core.h"


int strlen_as_word(const char*);
void word_from_str(letter_t*, const char*);
void swap_words(letter_t**, int*, letter_t**, int*);
bool load_dict(struct dict*, const char*);
void unload_dict(struct dict*);


#endif


