#ifndef BOOK_H
#define BOOK_H

struct Book
{
    int bookID;
    char title[200];
    char authors[200];
    float average_rating;
    char isbn[20];
    char isbn13[20];
    char language_code[10];
    int num_pages;
    int ratings_count;
    int text_reviewers_count;
    char publication_date[20];
    char publisher[100];
    int status; // 0 = Available, 1 = Borrowed
};

// Function Declarations
void loadBooksFromFile(const char *filename);
void displayBooks();
void searchBook();
void addBook(struct Book library[], int *currentCount, int id, const char *title,
             const char *authors, float rating, const char *isbn,
             const char *isbn13, const char *lang, int pages, int r_count, int rev_count, const char *pub_date,
             const char *publisher);
void deleteBook();

#endif