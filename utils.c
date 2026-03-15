#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "utils.h"

int safeReadInt(const char *message, int *value) {
    printf("%s", message);

    if (scanf("%d", value) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        while (getchar() != '\n');
        return 0;
    }

    return 1;
}

/* Fixed: was using %d instead of %ld */
int safeReadLong(const char *message, long *value) {
    printf("%s", message);

    if (scanf("%ld", value) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        while (getchar() != '\n');
        return 0;
    }

    return 1;
}

void removeQuotes(char *str) {
    int len;

    while (str[0] == '"') {
        memmove(str, str + 1, strlen(str));
    }

    len = strlen(str);
    while (len > 0 && str[len - 1] == '"') {
        str[len - 1] = '\0';
        len--;
    }
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

int containsIgnoreCase(const char *text, const char *search) {
    char tempText[500];
    char tempSearch[200];

    strncpy(tempText, text, sizeof(tempText) - 1);
    strncpy(tempSearch, search, sizeof(tempSearch) - 1);
    tempText[sizeof(tempText) - 1] = '\0';
    tempSearch[sizeof(tempSearch) - 1] = '\0';

    toLowerCase(tempText);
    toLowerCase(tempSearch);

    return strstr(tempText, tempSearch) != NULL;
}

int isNumber(const char *str) {
    if (*str == '\0') return 0;

    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

void clearInputBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void getLine(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        if (strchr(buffer, '\n') == NULL) {
            clearInputBuffer();
        } else {
            buffer[strcspn(buffer, "\n")] = 0;
        }
    } else {
        buffer[0] = '\0';
    }
}

void getValidatedFloatInRange(const char *prompt, float *value, float min, float max) {
    char line[100];
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%f", value) == 1 && *value >= min && *value <= max) {
            return;
        }
        printf("Invalid input. Please enter a number between %.1f and %.1f.\n", min, max);
    }
}

void getValidatedIntPositive(const char *prompt, int *value) {
    char line[100];
    long long input;
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%lld", &input) == 1 && input >= 0 && input <= 2147483647LL) {
            *value = (int)input;
            return;
        }
        printf("Invalid input. Please enter a non-negative integer.\n");
    }
}

void getValidatedIntInRange(const char *prompt, int *value, int min, int max) {
    char line[100];
    long long input;
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%lld", &input) == 1 && input >= min && input <= max) {
            *value = (int)input;
            return;
        }
        printf("Invalid input. Please enter an integer between %d and %d.\n", min, max);
    }
}

void getValidatedISBN(char *dest, int requiredLen, const char *label) {
    char buffer[100];
    int isValid;

    do {
        isValid = 1;
        printf("Enter %s (%d digits): ", label, requiredLen);
        getLine("", buffer, sizeof(buffer));

        if ((int)strlen(buffer) != requiredLen) {
            printf("ERROR: %s must be exactly %d digits.\n", label, requiredLen);
            isValid = 0;
            continue;
        }

        for (int i = 0; i < requiredLen; i++) {
            if (!isdigit(buffer[i])) {
                printf("ERROR: %s must contain only numbers.\n", label);
                isValid = 0;
                break;
            }
        }
    } while (!isValid);

    strcpy(dest, buffer);
}
