#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "book.h"

#define MAX_LINE 1024

//Global Variables
struct Book library[MAX_BOOKS];
int bookCount = 0;

int safeReadInt(const char *message, int *value) {
    printf("%s", message);
    
    if (scanf("%d", value) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        while (getchar() != '\n'); // clear buffer
        return 0;
    }

    return 1;
}

int safeReadFloat(const char *message, float *value) {
    printf("%s", message);

    if (scanf("%f", value) != 1) {
        printf("Invalid input. Please enter a valid decimal number.\n");
        while (getchar() != '\n');
        return 0;
    }

    return 1;
}

void safeReadString(const char *message, char *buffer, int size) {
    printf("%s", message);

    while (getchar() != '\n'); 
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
}

//Load Books from CSV file
void loadBooksFromFile(const char* filename) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("ERROR: File '%s' could not be opened.\n", filename);
        return;
    }

    char line[MAX_LINE];

    if (fgets(line, sizeof(line), file) == NULL) {
        printf("ERROR: File is empty.\n");
        fclose(file);
        return;
    }

    while (fgets(line, sizeof(line), file)) {

        if (bookCount >= MAX_BOOKS) {
            printf("WARNING: Library capacity reached.\n");
            break;
        }

        struct Book b;

        int parsed = sscanf(line,
            "%d,%199[^,],%199[^,],%f,%19[^,],%19[^,],%9[^,],%d,%d,%d,%19[^,],%99[^\n]",
            &b.bookID, b.title, b.authors, &b.average_rating,
            b.isbn, b.isbn13, b.language_code,
            &b.num_pages, &b.ratings_count,
            &b.text_reviewers_count,
            b.publication_date, b.publisher);

        if (parsed != 12) {
            printf("WARNING: Skipping corrupted record.\n");
            continue;
        }

        b.status = 0;
        library[bookCount++] = b;
    }

    fclose(file);

    printf("Books loaded successfully. Total: %d\n", bookCount);
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

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}
int containsIgnoreCase(const char *text, const char *search) {
    char tempText[500];
    char tempSearch[200];

    strcpy(tempText, text);
    strcpy(tempSearch, search);

    toLowerCase(tempText);
    toLowerCase(tempSearch);

    return strstr(tempText, tempSearch) != NULL;
}
int bookMatchesKeyword(int i, char *keyword) {

    char idStr[20], pagesStr[20], ratingStr[20];

    sprintf(idStr, "%d", library[i].bookID);
    sprintf(pagesStr, "%d", library[i].num_pages);
    sprintf(ratingStr, "%.2f", library[i].average_rating);

    return
        containsIgnoreCase(idStr, keyword) ||
        containsIgnoreCase(library[i].title, keyword) ||
        containsIgnoreCase(library[i].authors, keyword) ||
        containsIgnoreCase(library[i].publisher, keyword) ||
        containsIgnoreCase(library[i].isbn, keyword) ||
        containsIgnoreCase(library[i].language_code, keyword) ||
        containsIgnoreCase(ratingStr, keyword) ||
        containsIgnoreCase(pagesStr, keyword);
}
//Search book
void searchBook() {

    char input[200];
    int found = 0;

    printf("\nEnter search keywords: ");
    getchar();
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    char *keywords[20];
    int keywordCount = 0;

    char *token = strtok(input, " ");
    while (token != NULL && keywordCount < 20) {
        keywords[keywordCount++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < bookCount; i++) {

        int matchAll = 1;

        for (int k = 0; k < keywordCount; k++) {
            if (!bookMatchesKeyword(i, keywords[k])) {
                matchAll = 0;
                break;
            }
        }

        if (matchAll) {

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
void borrowBook() {

    int id;

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

    library[found].status = 1;

    FILE *file = fopen("books.csv", "w");

    if (file == NULL) {
        printf("ERROR: Unable to update file.\n");
        return;
    }

    for (int i = 0; i < bookCount; i++) {

        if (fprintf(file,
            "%d,%s,\"%s\",%.2f,%s,%s,%s,%d,%d,%d,%s,\"%s\",%d\n",
            library[i].bookID,
            library[i].title,
            library[i].authors,
            library[i].average_rating,
            library[i].isbn,
            library[i].isbn13,
            library[i].language_code,
            library[i].num_pages,
            library[i].ratings_count,
            library[i].text_reviewers_count,
            library[i].publication_date,
            library[i].publisher,
            library[i].status) < 0) {

            printf("ERROR: Failed writing to file.\n");
            fclose(file);
            return;
        }
    }

    fclose(file);

    printf("SUCCESS: '%s' borrowed.\n", library[found].title);
}

void returnBook() {
    int id;
    int found = -1;

    if (bookCount == 0) {
        printf("Library is empty.\n");
        return;
    }

    printf("Enter Book ID to return: ");
    scanf("%d", &id);

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
        printf("You did not borrow this book or It is already returned.\n");
        return;
    }

    library[found].status = 0;

    // Rewrite file
    FILE *file = fopen("books.csv", "w");
    if (file == NULL) {
        printf("Error updating file.\n");
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

    printf(">> Success: '%s' returned successfully.\n", library[found].title);
}

void addBook() {

    if (bookCount >= MAX_BOOKS) {
        printf("ERROR: Library is full.\n");
        return;
    }

    struct Book *newBook = &library[bookCount];

   newBook->bookID=bookCount;

    if (!safeReadFloat("Enter Average Rating: ", &newBook->average_rating)) return;

    if (!safeReadInt("Enter Number of Pages: ", &newBook->num_pages)) return;

    if (!safeReadInt("Enter Ratings Count: ", &newBook->ratings_count)) return;

    if (!safeReadInt("Enter Text Reviewers Count: ", &newBook->text_reviewers_count)) return;

    safeReadString("Enter Title: ", newBook->title, sizeof(newBook->title));
    safeReadString("Enter Authors: ", newBook->authors, sizeof(newBook->authors));

    printf("Enter ISBN: ");
    scanf("%19s", newBook->isbn);

    printf("Enter ISBN13: ");
    scanf("%19s", newBook->isbn13);

    printf("Enter Language Code: ");
    scanf("%9s", newBook->language_code);

    printf("Enter Publication Date: ");
    scanf("%19s", newBook->publication_date);

    safeReadString("Enter Publisher: ", newBook->publisher, sizeof(newBook->publisher));

    newBook->status = 0;
    bookCount++;

    FILE *file = fopen("books.csv", "a");

    if (file == NULL) {
        printf("ERROR: Could not open file.\n");
        return;
    }

    fprintf(file,
        "%d,%s,\"%s\",%.2f,%s,%s,%s,%d,%d,%d,%s,\"%s\",%d\n",
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

    printf("SUCCESS: '%s' added.\n", newBook->title);
}
void deleteBook(){
    char searchTitle[200];
    int foundIndex = -1;
    char confirm;

    if (bookCount ==0){
        printf("Library is Empty.\n");
        return;
    }
    if (fgets(searchTitle, sizeof(searchTitle), stdin) == NULL) {
        printf("ERROR: Failed reading input.\n");
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