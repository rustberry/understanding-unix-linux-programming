/* splitline.c - commmand reading and parsing functions for smsh
 *    
 *    char *next_cmd(char *prompt, FILE *fp) - get next command
 *    char **splitline(char *str);           - parse a string

 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"smsh.h"

char *cp;

char *next_cmd(char *prompt, FILE *fp) {
    printf("%s", prompt);
    char input[BUFSIZ];
    if (fgets(input, BUFSIZ, fp)) {
        input[strlen(input) - 1] = '\0';
        cp = malloc(strlen(input));
        if (cp == NULL) {
            perror("malloc failed");
        }
        strcpy(cp, input);
        return cp;
    }
    perror("fgets failed");
    exit(1);
}

char **splitline(char *str) 
/*
 * purpose: split a line into array of white-space separated tokens
 * returns: a NULL-terminated array of pointers to copies of the tokens
 *          or NULL if line if no tokens on the line
 *  action: traverse the array, locate strings, make copies
 *    note: strtok() could work, but we may want to add quotes later
 */
{
    char **arg_list;
    char *cp = str;
    size_t pre_pos = 0, counter = 0;
    size_t len = 0;
    size_t INITI_ARGLEN = 16;

    arg_list = (char **)malloc(sizeof(char*) * INITI_ARGLEN);
    
    while (*cp == ' ') cp++;  // Leading blanks
    for (size_t i = 0; i <= strlen(cp); i++) {
        if (*cp == '\0') break;
        while (cp[i] != ' ' && cp[i] != '\0') i++;

        len = i - pre_pos;
        arg_list[counter] = (char *)malloc(sizeof(char) * (len + 1));

        if (arg_list[counter] == NULL) {
            perror("malloc failed");
            exit(1);
        }
        strncpy(arg_list[counter], (cp + pre_pos), len);
        arg_list[counter][len] = '\0';

        while (cp[i] == ' ') i++;  // Trailing blank.

        if (*cp == '\0') break;

        pre_pos = i;
        counter++;

        if (counter >= INITI_ARGLEN) {
            while (counter >= INITI_ARGLEN) {
                INITI_ARGLEN += INITI_ARGLEN >> 1;  // init * 1.5
            }
       
            arg_list = (char **)realloc(arg_list, (sizeof(char*) * INITI_ARGLEN));
        }
    }
    
    return arg_list;
}

void freelist(char **ch_list) {
    for (size_t i = 0; ch_list[i]; i++) {
        free(ch_list[i]);
    }
}
