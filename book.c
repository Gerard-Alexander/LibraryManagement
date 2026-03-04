#include <stdio.h>
#include <string.h>
#include "book.h"

#define MAX_BOOKS 1000
#define MAX_LINE 1024

//Global Variables
struct Book library[MAX_BOOKS];
int bookCount = 0;

//Load Books from CSV file
void loadBooksFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Could not open file.\n");
        return;
    }

    char line[MAX_LINE];

    // Skip header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) && bookCount < MAX_BOOKS) {
    struct Book b;

    if (sscanf(line, "%d,%199[^,],%199[^,],%f,%19[^,],%19[^,],%9[^,],%d,%d,%d,%19[^,],%99[^\n]",
        &b.bookID,
        b.title,
        b.authors,
        &b.average_rating,
        b.isbn,
        b.isbn13,
        b.language_code,
        &b.num_pages,
        &b.ratings_count,
        &b.text_reviewers_count,
        b.publication_date,
        b.publisher
    ) == 12)   // Make sure parsing succeeded
    {
        b.status = 0;
        library[bookCount++] = b;
    }
}

    fclose(file);
    printf("Books loaded successfully! Total: %d\n", bookCount);
}

void displayBooks() {
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

//Search book
void searchBook() {
    char input[200];
    int found = 0;

    printf("\nEnter search keyword: ");
    getchar(); // clear buffer if needed
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    for (int i = 0; i < bookCount; i++) {

        char idStr[20], pagesStr[20], ratingStr[20];
        sprintf(idStr, "%d", library[i].bookID);
        sprintf(pagesStr, "%d", library[i].num_pages);
        sprintf(ratingStr, "%.2f", library[i].average_rating);

        if (
            strstr(idStr, input) != NULL ||
            strstr(library[i].title, input) != NULL ||
            strstr(library[i].authors, input) != NULL ||
            strstr(library[i].publisher, input) != NULL ||
            strstr(library[i].isbn, input) != NULL ||
            strstr(library[i].language_code, input) != NULL ||
            strstr(ratingStr, input) != NULL ||
            strstr(pagesStr, input) != NULL
        ) {
            // Print header only once
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

    if (found) {
        printf("==============================================================================================================\n");
    } else {
        printf("No matching books found.\n");
    }
}