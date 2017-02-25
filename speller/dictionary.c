/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

#define BASE (256)

/* treat strings as base-256 integers */
/* with digits in the range 1 to 255 */
unsigned long
hash(const char *s, unsigned long m)
{
    unsigned long h;
    unsigned const char *us;
  
    /* cast s to unsigned const char * */
    /* this ensures that elements of s will be treated as having values >= 0 */
    us = (unsigned const char *) s;
  
    h = 0;
    while(*us != '\0')
    {
        h = (h * BASE + *us) % m;
        us++;
    } 
    return h;
}

/**
 * Convert a word to all upper case
 */
void convertToUpper(char *word)
{
    for (int i = 0; i < strlen(word); i++)
    {
        word[i] = toupper(word[i]);
    }
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    char upperWord[LENGTH + 1];
    strcpy(upperWord, word);
    convertToUpper(upperWord);
    
    int hashKey = hash(upperWord, HASHTABLE_LENGTH);
    bool foundWord = false;
    
    node *pointer = hashTable[hashKey];
    
    while(pointer != NULL)
    {
        if (strcmp(upperWord, pointer->word) == 0)
        {
            foundWord = true;
            break;
        }
        pointer = pointer->next;
    }

    return foundWord;
}

/**
 * Add an entry to the hash table.  Return true if successful, false otherwise.
 */
bool addToHashTable(const char *word, unsigned int hashIndex)
{
    //
    // find end of list
    //
    node *ptr = hashTable[hashIndex];
    
    if (ptr == NULL)
    {
        ptr = malloc(sizeof(node));
        if (ptr == NULL)
        {
            return false;
        }
        
        hashTable[hashIndex] = ptr; 
        ptr->next = NULL;
    }
    else
    {
        while (ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        
        ptr->next = malloc(sizeof(node));
        if (ptr->next == NULL)
        {
            // unload(); // free(ptr->next);
            return false;
        }
        
        ptr = ptr->next;
        ptr->next = NULL;
    }

    //
    // copy given word into node
    //
    strcpy(ptr->word, word);
    return true;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    FILE *dictionaryFile = fopen(dictionary, "r");
    
    if (dictionaryFile == NULL)
    {
        return false;
    }

    //
    // initialize hash table to have each entry point to NULL
    //
    for (int index = 0; index < HASHTABLE_LENGTH; index++)
    {
        hashTable[index] = NULL;
    }
    
    int index = 0;
    char currentWord[LENGTH + 1];
    int wordCount = 0;
    for (int c = fgetc(dictionaryFile); c != EOF; c = fgetc(dictionaryFile))
    {
        if (c != '\n')
        {
            currentWord[index] = c;
            index++;
        }
        else
        {
            currentWord[index] = '\0';
            convertToUpper(currentWord);
            
            index = 0;
            wordCount++;
            unsigned long hashIndex = hash(currentWord, HASHTABLE_LENGTH);
            // printf("word number %d is...%s, hash index = %ld\n", wordCount, currentWord, hashIndex);
            
            addToHashTable(currentWord, hashIndex);
        }
    }
    
    fclose(dictionaryFile);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    unsigned int wordCount = 0;
    
    for (int index = 0; index < HASHTABLE_LENGTH; index++)
    {
        node *pointer = hashTable[index];
        while(pointer != NULL)
        {
            // printf("Index: %d....", index);
            // printf("Word in hashtable: %s.\n", pointer->word);
            pointer = pointer->next;
            wordCount++;
        }
    }
    
    // printf("Number of words in dictionary: %d.\n", wordCount);
    return wordCount;
}

/**
 * Free all nodes in lists
 */
void freeNodes(node *ptr)
{
    if (ptr->next != NULL)
    {
        freeNodes(ptr->next);
    }
    free(ptr);
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    for (int i = 0; i < HASHTABLE_LENGTH; i++)
    {
        node *ptr = hashTable[i];
        if (hashTable[i] != NULL)
        {
            while (ptr != NULL)
            {
                node *nextPtr = ptr;
                ptr = ptr->next;
                free(nextPtr);
            }
        }
    }
    return true;
}
