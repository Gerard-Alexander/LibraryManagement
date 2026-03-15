#include "gui/gui_common.h"
#include "gui/gui_login.h"
#include "file_io.h"

static void onActivate(GtkApplication *app, gpointer data) {
    (void)data;

    loadBooksFromFile("books.csv");

    GtkWidget *login = createLoginScreen(app);
    gtk_widget_show_all(login);
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new(
        "com.library.management",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(onActivate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
