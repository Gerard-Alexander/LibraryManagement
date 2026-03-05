#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "book.h"

int main() {
    int choice;
    loadBooksFromFile("books.csv");

    do
    {
        printf("\nLibrary Management System\n");
        printf("1. Add book\n");
        printf("2. Display all books\n");
        printf("3. Search book\n");
        printf("4. Borrow book\n");
        printf("5. Return book\n");
        printf("6. Delete book\n");
        printf("7. Exit\n");   
        printf("Enter your choice (1-7): ");
        scanf("%d", &choice);
        
        switch (choice)
        {
            case 1: addBook(); break;
            case 2: displayBooks(); break;
            case 3: searchBook(); break;
            case 4: borrowBook(); break;
            case 5: returnBook(); break;
            case 6: deleteBook(); break;
            case 7: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);
    return 0;
}

