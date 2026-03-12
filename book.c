#include <stdio.h>
#include <string.h>
#include "book.h"

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

void addBook() {
    
    if (bookCount>= MAX_BOOKS){
        printf("Library is now full.\n");
        return;
    }

    struct Book *newBook = &library[bookCount];

   newBook->bookID=bookCount;

    printf("Enter Average Rating: ");
    scanf("%d", &newBook->average_rating);

    printf("Enter Number of Pages: ");
    scanf("%d", &newBook->num_pages);

    printf("Enter Ratings Count: ");
    scanf("%d", &newBook->ratings_count);

    printf("Enter Text Reviewers Count: ");
    scanf("%d", &newBook->text_reviewers_count);

    while (getchar() !='\n');

    printf("Enter Title: ");
    fgets(newBook->title, sizeof(newBook->title), stdin );
    newBook->title[strcspn(newBook->title, "\n")]=0;

    printf("Enter Authors: ");
    fgets(newBook->authors, sizeof(newBook->authors), stdin);
    newBook->authors[strcspn(newBook->authors, "\n")]=0;

    printf("Enter ISBN: ");
    scanf("%s", newBook->isbn);

    printf("Enter ISBN13: ");
    scanf("%s", newBook->isbn13);

    printf("Enter Language Code: ");
    scanf("%s", newBook->language_code);

    printf("Enter Publication Date (YYYY-MM-DD): ");
    scanf("%s", newBook->publication_date);

    while(getchar() != '\n');

    printf("Enter Publisher: ");
    fgets(newBook->publisher, sizeof(newBook->publisher), stdin);
    newBook->publisher[strcspn(newBook->publisher, "\n")]=0;

    newBook->status = 0;
    (bookCount)++;

    FILE *file = fopen("books.csv", "a");
    if(file==NULL){
        printf("File 404\n");
        return;
    }
    fprintf(file, "%d,%s,\"%s\",%.2f,%s,%s,%s,%d,%d,%d,%s,\"%s\",%d\n",
            newBook->bookID,
            newBook->title,
            newBook->authors,
            newBook->average_rating,
            newBook->isbn,
            newBook->isbn13,
            newBook->language_code,
            newBook->num_pages,
            newBook->ratings_count,
            newBook->text_reviewers_count,
            newBook->publication_date,
            newBook->publisher,
            newBook->status);

    fclose(file);

    printf("\n>> Success: '%s' added\n", newBook->title);
}
void deleteBook(){
    char searchTitle[200];
    int foundIndex = -1;
    char confirm;

    if (bookCount ==0){
        printf("Library is Empty.\n");
        return;
    }

    printf("\nEnter the exact title of the book to be deleted: ");
    while (getchar()!='\n');
    fgets(searchTitle, sizeof(searchTitle), stdin);
    searchTitle[strcspn(searchTitle, "\n")]=0;

    for (int i = 0; i<bookCount; i++ ){
        if(stricmp(searchTitle, library[i].title)==0){
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1){
        printf("\nBook not Found.\n", searchTitle);
        return;
    }

    struct Book *b = &library[foundIndex];
    printf("\nDetails of Selected Book: \n");
    printf("ID: %d\nTitle: %s\nAuthor: %s\nPublisher: %s\n", 
            b->bookID, b->title, b->authors, b->publisher);
    printf("------------------\n");

    printf("Are you sure you want to delete this book? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
       
        for (int i = foundIndex; i < bookCount - 1; i++) {
            library[i] = library[i + 1];
        }
        (bookCount)--;

        FILE *file = fopen("books.csv", "w");
        if (file == NULL) {
            printf("File not Found\n");
            return;
        }

        for (int i = 0; i < bookCount; i++) {
            fprintf(file, "%d,%s,\"%s\",%.2f,%s,%s,%s,%d,%d,%d,%s,\"%s\",%d\n",
                    library[i].bookID, library[i].title, library[i].authors,
                    library[i].average_rating, library[i].isbn, library[i].isbn13,
                    library[i].language_code, library[i].num_pages,
                    library[i].ratings_count, library[i].text_reviewers_count,
                    library[i].publication_date, library[i].publisher,
                    library[i].status);
        }
        fclose(file);

        printf("\n>> Success: Book deleted from memory and books.csv.\n");
    } else {
        printf("\n>> Deletion cancelled.\n");
    }

}