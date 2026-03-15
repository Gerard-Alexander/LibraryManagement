#include "gui_delete_dialog.h"

gboolean showDeleteDialog(GtkWindow *parent, int idx) {
    struct Book *b = &library[idx];
    char msg[512];
    snprintf(msg, sizeof(msg),
             "Permanently delete \"%s\" by %s?\n\nThis cannot be undone.",
             b->title, b->authors);

    GtkWidget *dlg = gtk_dialog_new();
    gtk_window_set_title(GTK_WINDOW(dlg), "Delete Book");
    gtk_window_set_transient_for(GTK_WINDOW(dlg), parent);
    gtk_window_set_modal(GTK_WINDOW(dlg), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(dlg), 380, -1);
    gtk_window_set_resizable(GTK_WINDOW(dlg), FALSE);

    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    gtk_container_set_border_width(GTK_CONTAINER(content), 24);
    gtk_box_set_spacing(GTK_BOX(content), 12);

    GtkWidget *msgLbl = gtk_label_new(msg);
    gtk_label_set_line_wrap(GTK_LABEL(msgLbl), TRUE);
    gtk_label_set_xalign(GTK_LABEL(msgLbl), 0.0);
    gtk_label_set_max_width_chars(GTK_LABEL(msgLbl), 50);
    gtk_box_pack_start(GTK_BOX(content), msgLbl, FALSE, FALSE, 0);

    GtkWidget *cancelBtn = gtk_button_new_with_label("Cancel");
    GtkWidget *deleteBtn = gtk_button_new_with_label("Delete");
    gtk_widget_set_size_request(cancelBtn, 90, 32);
    gtk_widget_set_size_request(deleteBtn, 90, 32);

    gtk_dialog_add_action_widget(GTK_DIALOG(dlg), cancelBtn,
                                  GTK_RESPONSE_CANCEL);
    gtk_dialog_add_action_widget(GTK_DIALOG(dlg), deleteBtn,
                                  GTK_RESPONSE_ACCEPT);

    gtk_widget_show_all(dlg);
    gint response = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);

    if (response != GTK_RESPONSE_ACCEPT) return FALSE;

    for (int i = idx; i < bookCount - 1; i++)
        library[i] = library[i + 1];
    bookCount--;
    saveBooksToFile("books.csv");
    return TRUE;
}