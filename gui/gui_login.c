#include "gui_login.h"
#include "gui_main_window.h"

static GtkWidget    *loginWindow = NULL;
static GtkApplication *gApp        = NULL;

static void launchMainWindow(AppRole role) {
    currentRole = role;
    gtk_widget_destroy(loginWindow);
    loginWindow = NULL;
    mainWindow = createMainWindow(gApp);
    gtk_widget_show_all(mainWindow);
}

static void onAdminClicked(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    launchMainWindow(ROLE_ADMIN);
}

static void onUserClicked(GtkButton *btn, gpointer data) {
    (void)btn; (void)data;
    launchMainWindow(ROLE_USER);
}

/* Helper: build a role selection card as a clickable button */
static GtkWidget *makeRoleCard(const char *icon,
                                const char *title,
                                const char *desc,
                                GCallback  callback) {
    GtkWidget *btn   = gtk_button_new();
    GtkWidget *vbox  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *ico   = gtk_label_new(icon);
    GtkWidget *lbl   = gtk_label_new(title);
    GtkWidget *sub   = gtk_label_new(desc);

    gtk_widget_set_name(btn, "role-card-btn");
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "role-card");

    /* Icon */
    PangoAttrList *al = pango_attr_list_new();
    pango_attr_list_insert(al, pango_attr_size_new_absolute(28 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(ico), al);
    pango_attr_list_unref(al);

    /* Title */
    gtk_style_context_add_class(gtk_widget_get_style_context(lbl), "role-card-title");
    gtk_label_set_xalign(GTK_LABEL(lbl), 0.5);

    /* Desc */
    gtk_style_context_add_class(gtk_widget_get_style_context(sub), "role-card-desc");
    gtk_label_set_xalign(GTK_LABEL(sub), 0.5);

    gtk_box_pack_start(GTK_BOX(vbox), ico,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), lbl,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), sub,  FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(btn), vbox);

    gtk_widget_set_size_request(btn, 160, 120);
    g_signal_connect(btn, "clicked", callback, NULL);

    return btn;
}

GtkWidget *createLoginScreen(GtkApplication *app) {
    gApp = app;
    loginWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(loginWindow), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(loginWindow), 480, 400);
    gtk_window_set_position(GTK_WINDOW(loginWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(loginWindow), FALSE);

    applyAppCSS(loginWindow);

    /* Outer centering box */
    GtkWidget *outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(outer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(outer, 40);
    gtk_widget_set_margin_bottom(outer, 40);
    gtk_widget_set_margin_start(outer, 40);
    gtk_widget_set_margin_end(outer, 40);

    /* Card */
    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_style_context_add_class(gtk_widget_get_style_context(card), "login-card");

    /* Logo / title */
    GtkWidget *logo = gtk_label_new("\U0001F4DA");
    PangoAttrList *logoAttr = pango_attr_list_new();
    pango_attr_list_insert(logoAttr, pango_attr_size_new_absolute(40 * PANGO_SCALE));
    gtk_label_set_attributes(GTK_LABEL(logo), logoAttr);
    pango_attr_list_unref(logoAttr);
    gtk_widget_set_halign(logo, GTK_ALIGN_CENTER);

    GtkWidget *title = gtk_label_new("Library Management System");
    gtk_style_context_add_class(gtk_widget_get_style_context(title), "login-title");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);

    GtkWidget *subtitle = gtk_label_new("Select your role to continue");
    gtk_style_context_add_class(gtk_widget_get_style_context(subtitle), "login-subtitle");
    gtk_widget_set_halign(subtitle, GTK_ALIGN_CENTER);

    /* Role cards row */
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_halign(row, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(row, 12);

    GtkWidget *adminCard = makeRoleCard("\U0001F6E1",
                                        "Admin",
                                        "Manage the library\ncollection",
                                        G_CALLBACK(onAdminClicked));
    GtkWidget *userCard  = makeRoleCard("\U0001F464",
                                        "User",
                                        "Browse, borrow\nand return books",
                                        G_CALLBACK(onUserClicked));

    gtk_box_pack_start(GTK_BOX(row), adminCard, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(row), userCard,  FALSE, FALSE, 0);

    /* Version footer */
    GtkWidget *footer = gtk_label_new("v1.0  •  Books loaded from books.csv");
    gtk_style_context_add_class(gtk_widget_get_style_context(footer), "role-card-desc");
    gtk_widget_set_halign(footer, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(footer, 8);

    gtk_box_pack_start(GTK_BOX(card), logo,     FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), title,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), subtitle, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), row,      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(card), footer,   FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(outer), card, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(loginWindow), outer);

    return loginWindow;
}