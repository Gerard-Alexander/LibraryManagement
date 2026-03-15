#include <stdio.h>
#include "book.h"
#include "file_io.h"
#include "utils.h"
#include "borrow.h"

void borrowBook(void) {
    int id;
    char confirm;

    if (bookCount == 0) {
        printf("ERROR: Library is empty.\n");
        return;
    }

    if (!safeReadInt("Enter Book ID to borrow: ", &id)) {
        return;
    }

    int found = -1;
    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("ERROR: Book not found.\n");
        return;
    }

    if (library[found].status == 1) {
        printf("ERROR: Book already borrowed.\n");
        return;
    }

    printf("\nYou have selected:\n");
    printf("ID: %d\nTitle: %s\nAuthor: %s\n",
           library[found].bookID, library[found].title, library[found].authors);

    printf("Confirm borrowing? (y/n): ");
    scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y') {
        printf("Borrowing cancelled.\n");
        return;
    }

    library[found].status = 1;
    saveBooksToFile("books.csv");

    printf("SUCCESS: '%s' borrowed.\n", library[found].title);
}

void returnBook(void) {
    int id;
    int found = -1;
    char confirm;
    int borrowedCount = 0;

    if (bookCount == 0) {
        printf("Library is empty.\n");
        return;
    }

    printf("\n=== Borrowed Books ===\n");
    printf("%-6s %-30s %-20s\n", "ID", "Title", "Author");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < bookCount; i++) {
        if (library[i].status == 1) {
            printf("%-6d %-30.30s %-20.20s\n",
                   library[i].bookID, library[i].title, library[i].authors);
            borrowedCount++;
        }
    }
    printf("------------------------------------------------------------\n");

    if (borrowedCount == 0) {
        printf("No books are currently borrowed.\n");
        return;
    }

    if (!safeReadInt("Enter Book ID to return: ", &id)) {
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Book not found.\n");
        return;
    }

    if (library[found].status == 0) {
        printf("You did not borrow this book or it is already returned.\n");
        return;
    }

    printf("\nYou are returning:\n");
    printf("ID: %d\nTitle: %s\n", library[found].bookID, library[found].title);

    printf("Confirm return? (y/n): ");
    scanf(" %c", &confirm);
    if (confirm != 'y' && confirm != 'Y') {
        printf("Return cancelled.\n");
        return;
    }

    library[found].status = 0;
    saveBooksToFile("books.csv");

    printf(">> Success: '%s' returned successfully.\n", library[found].title);
}
