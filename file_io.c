#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "book.h"
#include "file_io.h"
#include "utils.h"

/* Global library state */
struct Book library[MAX_BOOKS];
int bookCount = 0;

/* Parse one CSV field from *src following RFC 4180:
   - Quoted fields: surrounded by ", with "" meaning a literal "
   - Unquoted fields: read until comma or newline
   Writes the unquoted value into dest (max destSize bytes including NUL).
   Returns pointer to the character after the field (past the trailing comma). */
static const char *parseCSVField(const char *src, char *dest, int destSize) {
    int i = 0;
    destSize--;

    if (*src == '"') {
        src++;
        while (*src) {
            if (*src == '"') {
                if (*(src + 1) == '"') {
                    if (i < destSize) dest[i++] = '"';
                    src += 2;
                } else {
                    src++;
                    break;
                }
            } else {
                if (i < destSize) dest[i++] = *src;
                src++;
            }
        }
    } else {
        while (*src && *src != ',' && *src != '\n' && *src != '\r') {
            if (i < destSize) dest[i++] = *src;
            src++;
        }
    }

    dest[i] = '\0';
    if (*src == ',') src++;
    return src;
}

void loadBooksFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("ERROR: File '%s' could not be opened.\n", filename);
        return;
    }

    char line[4096];

    /* Skip header row */
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

        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
            continue;

        struct Book b;
        char fields[13][300];
        const char *p = line;
        int f;

        for (f = 0; f < 13; f++) {
            if (*p == '\0' || *p == '\n' || *p == '\r') break;
            p = parseCSVField(p, fields[f], sizeof(fields[f]));
        }

        if (f < 12) {
            printf("WARNING: Skipping record with too few fields (got %d).\n", f);
            continue;
        }

        b.bookID               = atoi(fields[0]);
        strncpy(b.title,             fields[1],  sizeof(b.title)            - 1); b.title[sizeof(b.title)-1]                     = '\0';
        strncpy(b.authors,           fields[2],  sizeof(b.authors)          - 1); b.authors[sizeof(b.authors)-1]                 = '\0';
        b.average_rating       = (float)atof(fields[3]);
        strncpy(b.isbn,              fields[4],  sizeof(b.isbn)             - 1); b.isbn[sizeof(b.isbn)-1]                       = '\0';
        strncpy(b.isbn13,            fields[5],  sizeof(b.isbn13)           - 1); b.isbn13[sizeof(b.isbn13)-1]                   = '\0';
        strncpy(b.language_code,     fields[6],  sizeof(b.language_code)    - 1); b.language_code[sizeof(b.language_code)-1]     = '\0';
        b.num_pages            = atoi(fields[7]);
        b.ratings_count        = atoi(fields[8]);
        b.text_reviewers_count = atoi(fields[9]);
        strncpy(b.publication_date,  fields[10], sizeof(b.publication_date) - 1); b.publication_date[sizeof(b.publication_date)-1] = '\0';
        strncpy(b.publisher,         fields[11], sizeof(b.publisher)        - 1); b.publisher[sizeof(b.publisher)-1]             = '\0';

        /* atoi tolerates trailing junk like 0" from previously corrupt saves */
        b.status = (f >= 13) ? atoi(fields[12]) : 0;

        /* Strip any residual surrounding quotes */
        removeQuotes(b.title);
        removeQuotes(b.authors);
        removeQuotes(b.publisher);

        if (b.bookID <= 0) {
            printf("WARNING: Skipping record with invalid ID.\n");
            continue;
        }

        library[bookCount++] = b;
    }

    fclose(file);
    printf("Books loaded successfully. Total: %d\n", bookCount);
}

/* Single consolidated save used by borrow, return, delete, add */
void saveBooksToFile(const char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("ERROR: Unable to open file for writing.\n");
        return;
    }

    for (int i = 0; i < bookCount; i++) {
        if (fprintf(file,
            "%d,\"%s\",\"%s\",%.2f,%s,%s,%s,%d,%d,%d,%s,\"%s\",%d\n",
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
}