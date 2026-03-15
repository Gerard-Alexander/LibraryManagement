#include "gui_borrow_dialog.h"

static gboolean confirmAction(GtkWindow  *parent,
                               const char *title,
                               const char *message,
                               const char *confirmLabel) {
    GtkWidget *dlg = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dlg), title);
    gtk_window_set_transient_for(GTK_WINDOW(dlg), parent);
    gtk_window_set_modal(GTK_WINDOW(dlg), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dlg), 380, -1);
    gtk_window_set_resizable(GTK_WINDOW(dlg), FALSE);

    /* Content area */
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 24);
    gtk_box_set_spacing(GTK_BOX(content), 12);

    GtkWidget *msgLbl = gtk_label_new(message);
    gtk_label_set_line_wrap(GTK_LABEL(msgLbl), TRUE);
    gtk_label_set_xalign(GTK_LABEL(msgLbl), 0.0);
    gtk_label_set_max_width_chars(GTK_LABEL(msgLbl), 50);
    gtk_box_pack_start(GTK_BOX(content), msgLbl, FALSE, FALSE, 0);

    /* Add plain buttons as action widgets — GTK won't restyle action widgets
       added this way since they're not created by gtk_dialog_new_with_buttons */
    GtkWidget *cancelBtn  = gtk_button_new_with_label("Cancel");
    GtkWidget *confirmBtn = gtk_button_new_with_label(confirmLabel);
    gtk_widget_set_size_request(cancelBtn,  90, 32);
    gtk_widget_set_size_request(confirmBtn, 90, 32);

    gtk_dialog_add_action_widget(GTK_DIALOG(dlg), cancelBtn,
                                  GTK_RESPONSE_CANCEL);
    gtk_dialog_add_action_widget(GTK_DIALOG(dlg), confirmBtn,
                                  GTK_RESPONSE_ACCEPT);

    gtk_widget_show_all(dlg);
    gint response = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    return response == GTK_RESPONSE_ACCEPT;
}

gboolean showBorrowDialog(GtkWindow *parent, int idx) {
    struct Book *b = &library[idx];
    char msg[512];
    snprintf(msg, sizeof(msg),
             "Borrow \"%s\" by %s?\n\nThis book will be marked as Borrowed.",
             b->title, b->authors);
    if (!confirmAction(parent, "Borrow Book", msg, "Borrow"))
        return FALSE;
    b->status = 1;
    saveBooksToFile("books.csv");
    return TRUE;
}

gboolean showReturnDialog(GtkWindow *parent, int idx) {
    struct Book *b = &library[idx];
    char msg[512];
    snprintf(msg, sizeof(msg),
             "Return \"%s\"?\n\nThis book will be marked as Available again.",
             b->title);
    if (!confirmAction(parent, "Return Book", msg, "Return"))
        return FALSE;
    b->status = 0;
    saveBooksToFile("books.csv");
    return TRUE;
}