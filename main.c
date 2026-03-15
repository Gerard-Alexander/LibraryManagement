#include <stdio.h>
#include "file_io.h"
#include "menu.h"

int main(void) {
    int choice;

    loadBooksFromFile("books.csv");

    do {
        printf("\nLibrary Management System\n");
        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. Exit\n");
        printf("Enter your choice (1-3): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: adminMenu(); break;
            case 2: userMenu();  break;
            case 3: printf("Exiting program.\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}
