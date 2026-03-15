#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
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

int safeReadLong(const char *message, long *value) {
    printf("%s", message);
    
    if (scanf("%d", value) != 1) {
        printf("Invalid input. Please enter a valid number.\n");
        while (getchar() != '\n'); // clear buffer
        return 0;
    }

    return 1;
}

// Remove surrounding quotes from a string (used for CSV fields)
void removeQuotes(char *str) {
    int len;

    /* remove leading quotes */
    while (str[0] == '"') {
        memmove(str, str + 1, strlen(str));
    }

    /* remove trailing quotes */
    len = strlen(str);
    while (len > 0 && str[len - 1] == '"') {
        str[len - 1] = '\0';
        len--;
    }
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

        removeQuotes(b.title);
        removeQuotes(b.authors);
        removeQuotes(b.isbn);
        removeQuotes(b.isbn13);
        removeQuotes(b.language_code);
        removeQuotes(b.publication_date);
        removeQuotes(b.publisher);

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

    strncpy(tempText, text, sizeof(tempText));
    strncpy(tempSearch, search, sizeof(tempSearch));

    tempText[sizeof(tempText)-1] = '\0';
    tempSearch[sizeof(tempSearch)-1] = '\0';

    toLowerCase(tempText);
    toLowerCase(tempSearch);

    return strstr(tempText, tempSearch) != NULL;
}

int isNumber(const char *str) {
    if (*str == '\0') return 0;

    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

int bookMatchesKeyword(int i, char *keyword) {
    if (isNumber(keyword)) {

        char idStr[20];
        char pageStr[20];

        sprintf(idStr, "%d", library[i].bookID);
        sprintf(pageStr, "%d", library[i].num_pages);

        if (strstr(idStr, keyword) != NULL)
            return 1;

        if (strstr(pageStr, keyword) != NULL)
            return 1;

        return 0;
    }


    else {
        if (containsIgnoreCase(library[i].title, keyword))
            return 1;
        if (containsIgnoreCase(library[i].authors, keyword))
            return 1;
        if (containsIgnoreCase(library[i].status == 0 ? "Available" : "Borrowed", keyword))
            return 1;
        return 0;
    }
}



//Search book
void searchBook() {

    char input[200];
    int found = 0;

    printf("\nEnter search keywords: ");

    /* Clear leftover input buffer */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0) {
        printf("Empty search.\n");
        return;
    }

    char *keywords[20];
    int keywordCount = 0;

    char *token = strtok(input, " ");
    while (token != NULL && keywordCount < 20) {
        keywords[keywordCount++] = token;
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < bookCount; i++) {

        int match = 1;

        for (int k = 0; k < keywordCount; k++) {
            if (!bookMatchesKeyword(i, keywords[k])) {
                match = 0;
                break;
            }
        }

        if (match) {

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

    if (found)
        printf("==============================================================================================================\n");
    else
        printf("No matching books found.\n");
}

void borrowBook() {

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
    char confirm;
    int borrowedCount = 0;

    if (bookCount == 0) {
        printf("Library is empty.\n");
        return;
    }

    printf("\n=== Borrowed Books ===\n");
    printf("%-6s %-30s %-20s\n", "ID", "Title", "Author");
    printf("------------------------------------------------------------\n");
    for(int i=0; i<bookCount; i++) {
        if(library[i].status == 1) {
            printf("%-6d %-30.30s %-20.20s\n", library[i].bookID, library[i].title, library[i].authors);
            borrowedCount++;
        }
    }
    printf("------------------------------------------------------------\n");

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
        printf("You did not borrow this book or It is already returned.\n");
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

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void getLine(const char *prompt, char *buffer, int size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        // If newline is not found, buffer was too small, so we need to clear the rest of the line.
        if (strchr(buffer, '\n') == NULL) {
            clearInputBuffer();
        } else {
            // Remove the newline character
            buffer[strcspn(buffer, "\n")] = 0;
        }
    } else {
        buffer[0] = '\0'; // Handle error
    }
}

static void getValidatedFloatInRange(const char *prompt, float *value, float min, float max) {
    char line[100];
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') { // Handle empty input from getLine
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%f", value) == 1 && *value >= min && *value <= max) {
            return;
        }
        printf("Invalid input. Please enter a number between %.1f and %.1f.\n", min, max);
    }
}

static void getValidatedIntPositive(const char *prompt, int *value) {
    char line[100];
    long long input;
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%lld", &input) == 1 && input >= 0 && input <= 2147483647) { // INT_MAX
            *value = (int)input;
            return;
        }
        printf("Invalid input. Please enter a non-negative integer.\n");
    }
}

static void getValidatedIntInRange(const char *prompt, int *value, int min, int max) {
    char line[100];
    long long input;
    while (1) {
        getLine(prompt, line, sizeof(line));
        if (line[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }
        if (sscanf(line, "%lld", &input) == 1 && input >= min && input <= max) {
            *value = (int)input;
            return;
        }
        printf("Invalid input. Please enter an integer between %d and %d.\n", min, max);
    }
}

void getValidatedISBN(char *dest, int requiredLen, const char *label) {
    char buffer[100];
    int isValid;

    do {
        isValid = 1;
        printf("Enter %s (%d digits): ", label, requiredLen);
        getLine("", buffer, sizeof(buffer));

        if (strlen(buffer) != requiredLen) {
            printf("ERROR: %s must be exactly %d digits.\n", label, requiredLen);
            isValid = 0;
            continue;
        }

        for (int i = 0; i < requiredLen; i++) {
            if (!isdigit(buffer[i])) {
                printf("ERROR: %s must contain only numbers.\n", label);
                isValid = 0;
                break;
            }
        }
    } while (!isValid);
    strcpy(dest, buffer);
}

static int get_next_book_id() {
    if (bookCount == 0) {
        return 1;
    }
    int max_id = 0;
    for (int i = 0; i < bookCount; i++) {
        if (library[i].bookID > max_id) {
            max_id = library[i].bookID;
        }
    }
    return max_id + 1;
}

void addBook() {

    if (bookCount >= MAX_BOOKS) {
        printf("ERROR: Library is full.\n");
        return;
    }

    clearInputBuffer(); // Clear buffer from menu selection

    struct Book *newBook = &library[bookCount];

    newBook->bookID = get_next_book_id();

    getValidatedFloatInRange("Enter Average Rating (0.0-5.0): ", &newBook->average_rating, 0.0f, 5.0f);
    getValidatedIntInRange("Enter Number of Pages (1-10000): ", &newBook->num_pages, 1, 10000);
    getValidatedIntPositive("Enter Ratings Count (0 or more): ", &newBook->ratings_count);
    getValidatedIntPositive("Enter Text Reviewers Count (0 or more): ", &newBook->text_reviewers_count);

    getLine("Enter Title: ", newBook->title, sizeof(newBook->title));
    getLine("Enter Authors: ", newBook->authors, sizeof(newBook->authors));

    getValidatedISBN(newBook->isbn, 10, "ISBN");
    getValidatedISBN(newBook->isbn13, 13, "ISBN13");

    getLine("Enter Language Code: ", newBook->language_code, sizeof(newBook->language_code));
    getLine("Enter Publication Date (e.g., MM/DD/YYYY): ", newBook->publication_date, sizeof(newBook->publication_date));
    getLine("Enter Publisher: ", newBook->publisher, sizeof(newBook->publisher));

    newBook->status = 0;
    bookCount++;

    FILE *file = fopen("books.csv", "a");

    if (file == NULL) {
        printf("ERROR: Could not open file.\n");
        bookCount--; // Roll back book count
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
    long searchID;
    int foundIndex = -1;
    char confirm;

    if (bookCount ==0){
        printf("Library is Empty.\n");
        return;
    }

    if (!safeReadLong("\nEnter the Book ID to be deleted: ", &searchID)) {
        return;
    }

    for (int i = 0; i<bookCount; i++ ){
        if(library[i].bookID == searchID){
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1){
        printf("\nBook with that ID not found.\n");
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