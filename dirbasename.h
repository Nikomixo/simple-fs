#ifndef DIRBASENAME_H
#define DIRBASENAME_H

#define MAX_STRING_LENGTH 1024

char *get_dirname(const char *path, char *dirname);
char *get_basename(const char *path, char *basename);

#endif