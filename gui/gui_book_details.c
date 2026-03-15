#include "gui_book_details.h"

static GtkWidget *makeDetailRow(GtkWidget *grid, int row,
                                 const char *label, const char *value) {
    GtkWidget *lbl = gtk_label_new(label);
    GtkWidget *val = gtk_label_new(value);

    gtk_style_context_add_class(gtk_widget_get_style_context(lbl), "form-label");
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.0);
    gtk_label_set_xalign(GTK_LABEL(val), 0.0);
    gtk_label_set_selectable(GTK_LABEL(val), TRUE);
    gtk_label_set_line_wrap(GTK_LABEL(val), TRUE);
    gtk_widget_set_hexpand(val, TRUE);

    gtk_grid_attach(GTK_GRID(grid), lbl, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), val, 1, row, 1, 1);

    return val;
}

void showBookDetailsDialog(GtkWindow *parent, int idx) {
    struct Book *b = &library[idx];

    GtkWidget *dlg = gtk_dialog_new_with_buttons(
        "Book Details",
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Close", GTK_RESPONSE_CLOSE,
        NULL);

    gtk_window_set_default_size(GTK_WINDOW(dlg), 480, -1);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    gtk_box_set_spacing(GTK_BOX(content), 12);

    /* Header */
    GtkWidget *titleLbl = gtk_label_new(b->title);
    PangoAttrList *al = pango_attr_list_new();
    pango_attr_list_insert(al, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
    pango_attr_list_insert(al, pango_attr_size_new_absolute(16 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(titleLbl), al);
    pango_attr_list_unref(al);
    gtk_label_set_xalign(GTK_LABEL(titleLbl), 0.0);
    gtk_label_set_line_wrap(GTK_LABEL(titleLbl), TRUE);
    gtk_label_set_selectable(GTK_LABEL(titleLbl), TRUE);

    GtkWidget *authorLbl = gtk_label_new(b->authors);
    gtk_style_context_add_class(gtk_widget_get_style_context(authorLbl), "form-label");
    gtk_label_set_xalign(GTK_LABEL(authorLbl), 0.0);

    /* Status badge */
    GtkWidget *statusLbl = gtk_label_new(b->status == 0 ? "Available" : "Borrowed");
    gtk_style_context_add_class(gtk_widget_get_style_context(statusLbl),
                                 b->status == 0 ? "badge-available" : "badge-borrowed");
    gtk_widget_set_halign(statusLbl, GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(content), titleLbl,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), authorLbl, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content), statusLbl, FALSE, FALSE, 4);

    /* Separator */
    gtk_box_pack_start(GTK_BOX(content), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);

    /* Details grid */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    char buf[64];

    snprintf(buf, sizeof(buf), "%d", b->bookID);
    makeDetailRow(grid, 0, "Book ID",       buf);
    makeDetailRow(grid, 1, "ISBN",           b->isbn);
    makeDetailRow(grid, 2, "ISBN-13",        b->isbn13);

    snprintf(buf, sizeof(buf), "%d", b->num_pages);
    makeDetailRow(grid, 3, "Pages",          buf);

    snprintf(buf, sizeof(buf), "%.2f / 5.00", b->average_rating);
    makeDetailRow(grid, 4, "Rating",         buf);

    snprintf(buf, sizeof(buf), "%d", b->ratings_count);
    makeDetailRow(grid, 5, "Ratings count",  buf);

    snprintf(buf, sizeof(buf), "%d", b->text_reviewers_count);
    makeDetailRow(grid, 6, "Text reviews",   buf);

    makeDetailRow(grid, 7, "Language",       b->language_code);
    makeDetailRow(grid, 8, "Published",      b->publication_date);
    makeDetailRow(grid, 9, "Publisher",      b->publisher);

    gtk_box_pack_start(GTK_BOX(content), grid, FALSE, FALSE, 0);

    gtk_widget_show_all(dlg);
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}
