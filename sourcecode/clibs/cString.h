#ifndef CSTRING_H
#define CSTRING_H
/*******************************\
| Includes
\*******************************/
/* Std-Libs */

/* Project specific */

/*******************************\
| Defines
\*******************************/

/*******************************\
| Enum/Struct/Union
\*******************************/

/*******************************\
| Function declaration
\*******************************/
char *c_strcpy(char *dst, const char *src);
char *c_strtok(char *dst, const char *delim);
int c_strcmp(const char *str1, const char *str2);
int c_strlen(const char *str);
// int c_toLower(char *str);
// int c_toUower(char *str);

#endif