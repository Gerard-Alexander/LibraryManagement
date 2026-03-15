#ifndef UTILS_H
#define UTILS_H

int  safeReadInt(const char *message, int *value);
int  safeReadLong(const char *message, long *value);
void removeQuotes(char *str);
void toLowerCase(char *str);
int  containsIgnoreCase(const char *text, const char *search);
int  isNumber(const char *str);
void clearInputBuffer(void);
void getLine(const char *prompt, char *buffer, int size);
void getValidatedFloatInRange(const char *prompt, float *value, float min, float max);
void getValidatedIntPositive(const char *prompt, int *value);
void getValidatedIntInRange(const char *prompt, int *value, int min, int max);
void getValidatedISBN(char *dest, int requiredLen, const char *label);

#endif
