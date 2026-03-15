#include <stdio.h>
#include <string.h>
#include "book.h"
#include "utils.h"
#include "search.h"

/* Internal helper - not exposed in header */
static int bookMatchesKeyword(int i, char *keyword) {
    if (isNumber(keyword)) {
        char idStr[20];
        char pageStr[20];

        sprintf(idStr, "%d", library[i].bookID);
        sprintf(pageStr, "%d", library[i].num_pages);

        if (strstr(idStr, keyword) != NULL)   return 1;
        if (strstr(pageStr, keyword) != NULL) return 1;

        return 0;
    } else {
        if (containsIgnoreCase(library[i].title, keyword))   return 1;
        if (containsIgnoreCase(library[i].authors, keyword)) return 1;
        if (containsIgnoreCase(library[i].status == 0 ? "Available" : "Borrowed", keyword)) return 1;
        return 0;
    }
}

void searchBook(void) {
    char input[200];
    int found = 0;

    printf("\nEnter search keywords: ");

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        printf("Empty search.\n");
        return;
    }

    char *keywords[20];
    int keywordCount = 0;

    char *token = strtok(input, " ");
    while (token != NULL && keywordCount < 20) {
        keywords[keywordCount++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < bookCount; i++) {
        int match = 1;

        for (int k = 0; k < keywordCount; k++) {
            if (!bookMatchesKeyword(i, keywords[k])) {
                match = 0;
                break;
            }
        }

        if (match) {
            if (!found) {
                printf("\n==============================================================================================================\n");
                printf("%-6s %-35s %-25s %-8s %-10s\n",
                       "ID", "Title", "Author", "Pages", "Status");
                printf("==============================================================================================================\n");
            }

            printf("%-6d %-35.35s %-25.25s %-8d %-10s\n",
                   library[i].bookID,
                   library[i].title,
                   library[i].authors,
                   library[i].num_pages,
                   library[i].status == 0 ? "Available" : "Borrowed");

            found = 1;
        }
    }

    if (found)
        printf("==============================================================================================================\n");
    else
        printf("No matching books found.\n");
}
