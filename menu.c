#include <stdio.h>
#include "menu.h"
#include "admin.h"
#include "borrow.h"
#include "display.h"
#include "search.h"

void adminMenu(void) {
    int choice;
    do {
        printf("\n=== Admin Menu ===\n");
        printf("1. Add book\n");
        printf("2. Delete book\n");
        printf("3. Display all books\n");
        printf("4. Search book\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addBook();     break;
            case 2: deleteBook();  break;
            case 3: displayBooks();break;
            case 4: searchBook();  break;
            case 5: break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

void userMenu(void) {
    int choice;
    do {
        printf("\n=== User Menu ===\n");
        printf("1. Search book\n");
        printf("2. Display all books\n");
        printf("3. Borrow book\n");
        printf("4. Return book\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice (1-5): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: searchBook();  break;
            case 2: displayBooks();break;
            case 3: borrowBook();  break;
            case 4: returnBook();  break;
            case 5: break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}
