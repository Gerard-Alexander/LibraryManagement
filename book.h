#ifndef BOOK_H
#define BOOK_H

#define MAX_BOOKS 10000

struct Book {
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
    int status; /* 0 = Available, 1 = Borrowed */
};

/* Global library state - defined in file_io.c */
extern struct Book library[MAX_BOOKS];
extern int bookCount;

#endif
