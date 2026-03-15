#include <stdio.h>
#include <string.h>
#include "book.h"
#include "file_io.h"
#include "utils.h"
#include "admin.h"

static int getNextBookID(void) {
    if (bookCount == 0) return 1;

    int maxID = 0;
    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID > maxID)
            maxID = library[i].bookID;
    }
    return maxID + 1;
}

void addBook(void) {
    if (bookCount >= MAX_BOOKS) {
        printf("ERROR: Library is full.\n");
        return;
    }

    clearInputBuffer();

    struct Book *b = &library[bookCount];
    b->bookID = getNextBookID();

    /* Title and author first - more natural UX order */
    getLine("Enter Title: ", b->title, sizeof(b->title));
    getLine("Enter Authors: ", b->authors, sizeof(b->authors));
    getLine("Enter Publisher: ", b->publisher, sizeof(b->publisher));
    getLine("Enter Publication Date (e.g., MM/DD/YYYY): ", b->publication_date, sizeof(b->publication_date));
    getLine("Enter Language Code: ", b->language_code, sizeof(b->language_code));
    getValidatedISBN(b->isbn, 10, "ISBN");
    getValidatedISBN(b->isbn13, 13, "ISBN13");
    getValidatedIntInRange("Enter Number of Pages (1-10000): ", &b->num_pages, 1, 10000);
    getValidatedFloatInRange("Enter Average Rating (0.0-5.0): ", &b->average_rating, 0.0f, 5.0f);
    getValidatedIntPositive("Enter Ratings Count: ", &b->ratings_count);
    getValidatedIntPositive("Enter Text Reviewers Count: ", &b->text_reviewers_count);

    b->status = 0;
    bookCount++;

    saveBooksToFile("books.csv");

    printf("SUCCESS: '%s' added with ID %d.\n", b->title, b->bookID);
}

void deleteBook(void) {
    long searchID;
    int foundIndex = -1;
    char confirm;

    if (bookCount == 0) {
        printf("Library is empty.\n");
        return;
    }

    if (!safeReadLong("\nEnter the Book ID to delete: ", &searchID)) {
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID == (int)searchID) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1) {
        printf("\nBook with that ID not found.\n");
        return;
    }

    struct Book *b = &library[foundIndex];
    printf("\nDetails of selected book:\n");
    printf("ID: %d\nTitle: %s\nAuthor: %s\nPublisher: %s\n",
           b->bookID, b->title, b->authors, b->publisher);
    printf("------------------\n");

    printf("Are you sure you want to delete this book? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm != 'y' && confirm != 'Y') {
        printf("\n>> Deletion cancelled.\n");
        return;
    }

    /* Shift array left to fill the gap */
    for (int i = foundIndex; i < bookCount - 1; i++) {
        library[i] = library[i + 1];
    }
    bookCount--;

    saveBooksToFile("books.csv");

    printf("\n>> Success: Book deleted.\n");
}