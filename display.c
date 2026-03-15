#include <stdio.h>
#include "book.h"
#include "display.h"

void displayBooks(void) {
    if (bookCount == 0) {
        printf("No books available.\n");
        return;
    }

    printf("\n==============================================================================================================\n");
    printf("%-6s %-35s %-25s %-8s %-10s\n",
           "ID", "Title", "Author", "Pages", "Status");
    printf("==============================================================================================================\n");

    for (int i = 0; i < bookCount; i++) {
        printf("%-6d %-35.35s %-25.25s %-8d %-10s\n",
               library[i].bookID,
               library[i].title,
               library[i].authors,
               library[i].num_pages,
               library[i].status == 0 ? "Available" : "Borrowed");
    }

    printf("==============================================================================================================\n");
}
