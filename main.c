#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

void adminMenu() {
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
            case 1: addBook(); break;
            case 2: deleteBook(); break;
            case 3: displayBooks(); break;
            case 4: searchBook(); break;
            case 5: break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

void userMenu() {
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
            case 1: searchBook(); break;
            case 2: displayBooks(); break;
            case 3: borrowBook(); break;
            case 4: returnBook(); break;
            case 5: break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
}

int main() {
    int choice;
    loadBooksFromFile("books.csv");

    do
    {
        printf("\nLibrary Management System\n");
        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. Exit\n");   
        printf("Enter your choice (1-3): ");
        scanf("%d", &choice);
        
        switch (choice)
        {
            case 1: adminMenu(); break;
            case 2: userMenu(); break;
            case 3: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);
    return 0;
}
