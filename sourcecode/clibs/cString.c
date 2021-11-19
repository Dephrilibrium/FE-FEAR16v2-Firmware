/*******************************\
| Includes
\*******************************/
/* Std-Libs */
#include "stdint.h"
#include "stddef.h"

/* Project specific */
#include "cString.h"

/*******************************\
| Local function declarations
\*******************************/

/*******************************\
| Global variables
\*******************************/

/*******************************\
| Function definitons
\*******************************/
char *c_strcpy(char *dst, const char *src)
{
    char *dstBak = dst;
    while (*src != '\0')
    {
        *dst = *src;
        src++;
        dst++;
    }
    return dstBak;
}

char *c_strtok(char *dst, const char *delim)
{
    static char *ret = 0;  // Return-pointer
    static char *srch = 0; // Search-pointer

    if (dst != NULL) // When new string given use this as new reference
        ret = dst;
    else // Otherwise continue from backupped one
    {
        if (*srch == '\0') // But check if it has already reached the end!
            return NULL;

        ret = srch; // When not finished start from there!
    }

    srch = ret;

    while (*srch != '\0' && *srch != *delim) // Stop when hit delimitier or termination
        srch++;

    for (; *srch == *delim; srch++) // When delim found check for more
        *srch = '\0';               // and terminate them also

    return ret;
}

int c_strcmp(const char *str1, const char *str2)
{
    for (; *str1 == *str2; str1++, str2++)
        if (*str1 == '\0')
            return 0;

    return *str1 - *str2;
}

int c_strlen(const char *str)
{
    int cnt = 0;
    for (; str[cnt] != '\0'; cnt++)
        ; // Count

    return cnt;
}

// int c_toLower(char *str)
// {
//     // Needs to be implemented
//     // while (*str != '\0')
//     // {

//     // }
// }

// int c_toUower(char *str)
// {
//     // Needs to be implemented
// }