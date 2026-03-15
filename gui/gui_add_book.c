#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gui_add_book.h"

static int getNextBookID(void) {
    if (bookCount == 0) return 1;
    int maxID = 0;
    for (int i = 0; i < bookCount; i++)
        if (library[i].bookID > maxID) maxID = library[i].bookID;
    return maxID + 1;
}

static gboolean isAllDigits(const char *s, int len) {
    if ((int)strlen(s) != len) return FALSE;
    for (int i = 0; i < len; i++)
        if (!isdigit((unsigned char)s[i])) return FALSE;
    return TRUE;
}

static GtkWidget *makeFormField(GtkWidget *vbox,
                                 const char *labelText,
                                 const char *placeholder) {
    GtkWidget *lbl   = gtk_label_new(labelText);
    GtkWidget *entry = gtk_entry_new();

    gtk_style_context_add_class(gtk_widget_get_style_context(lbl),   "form-label");
    gtk_style_context_add_class(gtk_widget_get_style_context(entry), "form-entry");
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_widget_set_hexpand(entry, TRUE);

    gtk_box_pack_start(GTK_BOX(vbox), lbl,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 4);

    return entry;
}

gboolean showAddBookDialog(GtkWindow *parent) {
    if (bookCount >= MAX_BOOKS) {
        GtkWidget *err = gtk_message_dialog_new(parent,
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Library is full (max %d books).", MAX_BOOKS);
        gtk_dialog_run(GTK_DIALOG(err));
        gtk_widget_destroy(err);
        return FALSE;
    }

    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        "Add New Book", parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Cancel",   GTK_RESPONSE_CANCEL,
        "_Add Book", GTK_RESPONSE_ACCEPT,
        NULL);

    gtk_window_set_default_size(GTK_WINDOW(dlg), 520, -1);

    GtkWidget *addBtn = gtk_dialog_get_widget_for_response(
                            GTK_DIALOG(dlg), GTK_RESPONSE_ACCEPT);
    gtk_widget_set_size_request(addBtn, 100, 32);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    gtk_box_set_spacing(GTK_BOX(content), 6);

    GtkWidget *grid     = gtk_grid_new();
    GtkWidget *leftCol  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *rightCol = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_widget_set_hexpand(leftCol,  TRUE);
    gtk_widget_set_hexpand(rightCol, TRUE);

    GtkWidget *eTitle    = makeFormField(leftCol,  "Title *",                  "e.g. The Great Gatsby");
    GtkWidget *eAuthors  = makeFormField(leftCol,  "Authors *",                "e.g. F. Scott Fitzgerald");
    GtkWidget *ePublisher= makeFormField(leftCol,  "Publisher",                "e.g. Scribner");
    GtkWidget *ePubDate  = makeFormField(leftCol,  "Publication Date",         "MM/DD/YYYY");
    GtkWidget *eLang     = makeFormField(leftCol,  "Language Code",            "e.g. eng");

    GtkWidget *eISBN     = makeFormField(rightCol, "ISBN (10 digits) *",       "e.g. 0743273567");
    GtkWidget *eISBN13   = makeFormField(rightCol, "ISBN-13 (13 digits) *",    "e.g. 9780743273565");
    GtkWidget *ePages    = makeFormField(rightCol, "Number of Pages *",        "e.g. 180");
    GtkWidget *eRating   = makeFormField(rightCol, "Average Rating (0.0-5.0)", "e.g. 4.2");
    GtkWidget *eRatCount = makeFormField(rightCol, "Ratings Count",            "e.g. 5000");

    gtk_grid_attach(GTK_GRID(grid), leftCol,  0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rightCol, 1, 0, 1, 1);

    /* Error label — styled via CSS class instead of deprecated override_color */
    GtkWidget *errLbl = gtk_label_new("");
    gtk_style_context_add_class(gtk_widget_get_style_context(errLbl), "form-label-error");
    gtk_label_set_xalign(GTK_LABEL(errLbl), 0.0);

    gtk_box_pack_start(GTK_BOX(content), grid,   TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(content), errLbl, FALSE, FALSE, 4);

    gtk_widget_show_all(dlg);

    gboolean added = FALSE;

    while (TRUE) {
        gint response = gtk_dialog_run(GTK_DIALOG(dlg));
        if (response != GTK_RESPONSE_ACCEPT) break;

        const char *title     = gtk_entry_get_text(GTK_ENTRY(eTitle));
        const char *authors   = gtk_entry_get_text(GTK_ENTRY(eAuthors));
        const char *isbn      = gtk_entry_get_text(GTK_ENTRY(eISBN));
        const char *isbn13    = gtk_entry_get_text(GTK_ENTRY(eISBN13));
        const char *pagesStr  = gtk_entry_get_text(GTK_ENTRY(ePages));
        const char *ratingStr = gtk_entry_get_text(GTK_ENTRY(eRating));

        if (strlen(title) == 0) {
            gtk_label_set_text(GTK_LABEL(errLbl), "Title is required.");
            continue;
        }
        if (strlen(authors) == 0) {
            gtk_label_set_text(GTK_LABEL(errLbl), "Authors field is required.");
            continue;
        }
        if (!isAllDigits(isbn, 10)) {
            gtk_label_set_text(GTK_LABEL(errLbl), "ISBN must be exactly 10 digits.");
            continue;
        }
        if (!isAllDigits(isbn13, 13)) {
            gtk_label_set_text(GTK_LABEL(errLbl), "ISBN-13 must be exactly 13 digits.");
            continue;
        }
        int pages = atoi(pagesStr);
        if (pages < 1 || pages > 10000) {
            gtk_label_set_text(GTK_LABEL(errLbl), "Pages must be between 1 and 10000.");
            continue;
        }
        float rating = (float)atof(ratingStr);
        if (rating < 0.0f || rating > 5.0f) {
            gtk_label_set_text(GTK_LABEL(errLbl), "Rating must be between 0.0 and 5.0.");
            continue;
        }

        struct Book *b = &library[bookCount];
        b->bookID = getNextBookID();

        strncpy(b->title,            title,   sizeof(b->title)            - 1);
        strncpy(b->authors,          authors, sizeof(b->authors)          - 1);
        strncpy(b->isbn,             isbn,    sizeof(b->isbn)             - 1);
        strncpy(b->isbn13,           isbn13,  sizeof(b->isbn13)           - 1);
        strncpy(b->publisher,
                gtk_entry_get_text(GTK_ENTRY(ePublisher)),
                sizeof(b->publisher)        - 1);
        strncpy(b->publication_date,
                gtk_entry_get_text(GTK_ENTRY(ePubDate)),
                sizeof(b->publication_date) - 1);
        strncpy(b->language_code,
                gtk_entry_get_text(GTK_ENTRY(eLang)),
                sizeof(b->language_code)    - 1);

        b->title[sizeof(b->title)-1]                       = '\0';
        b->authors[sizeof(b->authors)-1]                   = '\0';
        b->isbn[sizeof(b->isbn)-1]                         = '\0';
        b->isbn13[sizeof(b->isbn13)-1]                     = '\0';
        b->publisher[sizeof(b->publisher)-1]               = '\0';
        b->publication_date[sizeof(b->publication_date)-1] = '\0';
        b->language_code[sizeof(b->language_code)-1]       = '\0';

        b->num_pages            = pages;
        b->average_rating       = rating;
        b->ratings_count        = atoi(gtk_entry_get_text(GTK_ENTRY(eRatCount)));
        b->text_reviewers_count = 0;
        b->status               = 0;

        bookCount++;
        saveBooksToFile("books.csv");
        added = TRUE;
        break;
    }

    gtk_widget_destroy(dlg);
    return added;
}