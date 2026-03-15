#include <string.h>
#include <stdio.h>
#include "gui_main_window.h"
#include "gui_login.h"
#include "gui_book_details.h"
#include "gui_borrow_dialog.h"
#include "gui_add_book.h"
#include "gui_delete_dialog.h"
#include "../utils.h"

/* ── View modes ─────────────────────────────────────────── */
typedef enum { VIEW_ALL = 0, VIEW_SEARCH, VIEW_BORROWED } ViewMode;

/* ── Module-level state ─────────────────────────────────── */
static GtkApplication *gApp      = NULL;
static GtkWidget   *gWindow      = NULL;
static GtkWidget   *gTreeView    = NULL;
static GtkListStore *gStore      = NULL;
static GtkWidget   *gSearchEntry = NULL;
static GtkWidget   *gStatusLabel = NULL;
static GtkWidget   *gPageLabel   = NULL;
static GtkWidget   *gPrevBtn     = NULL;
static GtkWidget   *gNextBtn     = NULL;
static GtkWidget   *gPageTitle   = NULL;
static GtkWidget   *gAddBtn      = NULL;
static GtkWidget   *gNavAll      = NULL;
static GtkWidget   *gNavSearch   = NULL;
static GtkWidget   *gNavBorrowed = NULL;
static GtkWidget   *gNavAdd      = NULL;
static GtkWidget   *gNavDelete   = NULL;
static GtkWidget   *gRoleName    = NULL;

static ViewMode  currentView     = VIEW_ALL;
static int       currentPage     = 0;
static int       pageSize        = 50;
static char      searchQuery[200] = "";

/* Filtered index list */
static int filteredIndices[MAX_BOOKS];
static int filteredCount = 0;

/* ── List store columns ─────────────────────────────────── */
enum {
    COL_INDEX = 0,   /* hidden: index into library[] */
    COL_ID,
    COL_TITLE,
    COL_AUTHOR,
    COL_PAGES,
    COL_RATING,
    COL_STATUS,
    COL_STATUS_COLOR,  /* background color for status cell */
    N_COLS
};

/* ── Filter helpers ─────────────────────────────────────── */
static int bookMatchesQuery(int i, const char *query) {
    if (strlen(query) == 0) return 1;

    char input[200];
    strncpy(input, query, sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';

    char *keywords[20];
    int   kwCount = 0;
    char *tok = strtok(input, " ");
    while (tok && kwCount < 20) {
        keywords[kwCount++] = tok;
        tok = strtok(NULL, " ");
    }

    for (int k = 0; k < kwCount; k++) {
        int match = 0;
        if (isNumber(keywords[k])) {
            char idStr[20], pageStr[20];
            snprintf(idStr,   sizeof(idStr),   "%d", library[i].bookID);
            snprintf(pageStr, sizeof(pageStr), "%d", library[i].num_pages);
            if (strstr(idStr,   keywords[k])) match = 1;
            if (strstr(pageStr, keywords[k])) match = 1;
        } else {
            if (containsIgnoreCase(library[i].title,   keywords[k])) match = 1;
            if (containsIgnoreCase(library[i].authors, keywords[k])) match = 1;
            if (containsIgnoreCase(library[i].status == 0 ?
                "available" : "borrowed", keywords[k]))               match = 1;
        }
        if (!match) return 0;
    }
    return 1;
}

static void rebuildFilteredList(void) {
    filteredCount = 0;
    for (int i = 0; i < bookCount; i++) {
        if (currentView == VIEW_BORROWED && library[i].status != 1) continue;
        if (!bookMatchesQuery(i, searchQuery)) continue;
        filteredIndices[filteredCount++] = i;
    }
}

/* ── Store / table refresh ──────────────────────────────── */
void refreshBookList(void) {
    rebuildFilteredList();

    int totalPages = (filteredCount + pageSize - 1) / pageSize;
    if (totalPages == 0) totalPages = 1;
    if (currentPage >= totalPages) currentPage = totalPages - 1;

    int start = currentPage * pageSize;
    int end   = start + pageSize;
    if (end > filteredCount) end = filteredCount;

    gtk_list_store_clear(gStore);

    for (int i = start; i < end; i++) {
        int       idx = filteredIndices[i];
        struct Book *b = &library[idx];
        GtkTreeIter iter;
        char ratingBuf[16];
        snprintf(ratingBuf, sizeof(ratingBuf), "%.2f", b->average_rating);

        gtk_list_store_append(gStore, &iter);
        gtk_list_store_set(gStore, &iter,
            COL_INDEX,        idx,
            COL_ID,           b->bookID,
            COL_TITLE,        b->title,
            COL_AUTHOR,       b->authors,
            COL_PAGES,        b->num_pages,
            COL_RATING,       ratingBuf,
            COL_STATUS,       b->status == 0 ? "Available" : "Borrowed",
            COL_STATUS_COLOR, b->status == 0 ? "#eafaf1"   : "#fef9e7",
            -1);
    }

    /* Status bar */
    char statusBuf[512];
    if (strlen(searchQuery) > 0)
        snprintf(statusBuf, sizeof(statusBuf),
                 "Showing %d–%d of %d results for \"%s\"",
                 filteredCount ? start + 1 : 0, end, filteredCount, searchQuery);
    else
        snprintf(statusBuf, sizeof(statusBuf),
                 "Showing %d–%d of %d books",
                 filteredCount ? start + 1 : 0, end, filteredCount);
    gtk_label_set_text(GTK_LABEL(gStatusLabel), statusBuf);

    /* Pagination */
    char pageBuf[32];
    snprintf(pageBuf, sizeof(pageBuf), "Page %d of %d", currentPage + 1, totalPages);
    gtk_label_set_text(GTK_LABEL(gPageLabel), pageBuf);
    gtk_widget_set_sensitive(gPrevBtn, currentPage > 0);
    gtk_widget_set_sensitive(gNextBtn, currentPage < totalPages - 1);
}

/* ── Callbacks ──────────────────────────────────────────── */
static void onSearchChanged(GtkEntry *entry, gpointer data) {
    (void)data;
    const char *text = gtk_entry_get_text(entry);
    strncpy(searchQuery, text, sizeof(searchQuery) - 1);
    searchQuery[sizeof(searchQuery) - 1] = '\0';
    currentPage  = 0;
    currentView  = strlen(searchQuery) > 0 ? VIEW_SEARCH : VIEW_ALL;
    refreshBookList();
}

static void setView(ViewMode mode, const char *title) {
    currentView = mode;
    currentPage = 0;
    memset(searchQuery, 0, sizeof(searchQuery));
    gtk_entry_set_text(GTK_ENTRY(gSearchEntry), "");
    gtk_label_set_text(GTK_LABEL(gPageTitle), title);

    /* Update nav active states */
    GtkStyleContext *sc;
    #define SET_ACTIVE(btn, active) \
        sc = gtk_widget_get_style_context(btn); \
        if (active) gtk_style_context_add_class(sc, "active"); \
        else gtk_style_context_remove_class(sc, "active");

    SET_ACTIVE(gNavAll,      mode == VIEW_ALL);
    SET_ACTIVE(gNavSearch,   mode == VIEW_SEARCH);
    SET_ACTIVE(gNavBorrowed, mode == VIEW_BORROWED);
    #undef SET_ACTIVE

    refreshBookList();
}

static void onNavAll(GtkButton *b, gpointer d)      { (void)b;(void)d; setView(VIEW_ALL,      "All Books"); }
static void onNavSearch(GtkButton *b, gpointer d)   { (void)b;(void)d;
    gtk_widget_grab_focus(gSearchEntry);
    setView(VIEW_SEARCH, "Search"); }
static void onNavBorrowed(GtkButton *b, gpointer d) { (void)b;(void)d; setView(VIEW_BORROWED, "Borrowed Books"); }

static void onNavAdd(GtkButton *b, gpointer d) {
    (void)b; (void)d;
    if (showAddBookDialog(GTK_WINDOW(gWindow)))
        refreshBookList();
}

static void onNavDelete(GtkButton *b, gpointer d) {
    (void)b; (void)d;
    GtkTreeSelection *sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(gTreeView));
    GtkTreeIter iter;
    GtkTreeModel *model;
    if (!gtk_tree_selection_get_selected(sel, &model, &iter)) {
        GtkWidget *info = gtk_message_dialog_new(GTK_WINDOW(gWindow),
            GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "Please select a book to delete.");
        gtk_dialog_run(GTK_DIALOG(info));
        gtk_widget_destroy(info);
        return;
    }
    int idx;
    gtk_tree_model_get(model, &iter, COL_INDEX, &idx, -1);
    if (showDeleteDialog(GTK_WINDOW(gWindow), idx))
        refreshBookList();
}

static void onPrevPage(GtkButton *b, gpointer d) {
    (void)b; (void)d;
    if (currentPage > 0) { currentPage--; refreshBookList(); }
}

static void onNextPage(GtkButton *b, gpointer d) {
    (void)b; (void)d;
    int total = (filteredCount + pageSize - 1) / pageSize;
    if (currentPage < total - 1) { currentPage++; refreshBookList(); }
}

static void onRowActivated(GtkTreeView *tv, GtkTreePath *path,
                            GtkTreeViewColumn *col, gpointer data) {
    (void)col; (void)data;
    GtkTreeModel *model = gtk_tree_view_get_model(tv);
    GtkTreeIter   iter;
    gtk_tree_model_get_iter(model, &iter, path);
    int idx;
    gtk_tree_model_get(model, &iter, COL_INDEX, &idx, -1);
    showBookDetailsDialog(GTK_WINDOW(gWindow), idx);
}

/* ── Action button cell renderer ────────────────────────── */
typedef struct { GtkCellRenderer *renderer; GtkTreeViewColumn *col; } BtnColData;

static void borrowReturnCellFunc(GtkTreeViewColumn *col,
                                  GtkCellRenderer   *cell,
                                  GtkTreeModel      *model,
                                  GtkTreeIter       *iter,
                                  gpointer           data) {
    (void)col; (void)data;
    int idx;
    gtk_tree_model_get(model, iter, COL_INDEX, &idx, -1);
    int borrowed = library[idx].status;
    g_object_set(cell,
        "text",             borrowed ? "Return" : "Borrow",
        "cell-background",  borrowed ? "#c0392b" : "#2980b9",
        "foreground",       "#ffffff",
        NULL);
}

static gboolean onTreeButtonPress(GtkWidget *widget,
                                   GdkEventButton *event,
                                   gpointer data) {
    (void)data;
    if (event->type != GDK_BUTTON_PRESS || event->button != 1)
        return FALSE;

    GtkTreePath       *path = NULL;
    GtkTreeViewColumn *col  = NULL;

    if (!gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(widget),
                                        (gint)event->x, (gint)event->y,
                                        &path, &col, NULL, NULL))
        return FALSE;

    GList *cols = gtk_tree_view_get_columns(GTK_TREE_VIEW(widget));
    GtkTreeViewColumn *actionCol = g_list_last(cols)->data;
    g_list_free(cols);

    if (col != actionCol) {
        gtk_tree_path_free(path);
        return FALSE;
    }

    GtkTreeIter   iter;
    GtkTreeModel *model = GTK_TREE_MODEL(gStore);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);

    int idx;
    gtk_tree_model_get(model, &iter, COL_INDEX, &idx, -1);

    if (library[idx].status == 0)
        showBorrowDialog(GTK_WINDOW(gWindow), idx);
    else
        showReturnDialog(GTK_WINDOW(gWindow), idx);

    refreshBookList();
    return TRUE;
}

/* ── Logout ─────────────────────────────────────────────── */
static void onLogout(GtkButton *b, gpointer d) {
    (void)b; (void)d;
    /* Destroy main window and reopen login */
    gtk_widget_destroy(gWindow);
    gWindow = NULL;
    GtkWidget *login = createLoginScreen(gApp);
    gtk_widget_show_all(login);
}

/* ── Sidebar nav button ─────────────────────────────────── */
static GtkWidget *makeNavButton(const char *label, GCallback cb) {
    GtkWidget *btn = gtk_button_new_with_label(label);
    gtk_style_context_add_class(gtk_widget_get_style_context(btn), "nav-btn");
    gtk_button_set_relief(GTK_BUTTON(btn), GTK_RELIEF_NONE);
    gtk_widget_set_halign(btn, GTK_ALIGN_FILL);
    g_signal_connect(btn, "clicked", cb, NULL);
    return btn;
}

/* ── Main window construction ───────────────────────────── */
GtkWidget *createMainWindow(GtkApplication *app) {
    gApp    = app;
    gWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(gWindow), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(gWindow), 1000, 640);
    gtk_window_set_position(GTK_WINDOW(gWindow), GTK_WIN_POS_CENTER);

    /* ── Root horizontal pane ── */
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    /* ════════════════════════════════════════
       SIDEBAR
       ════════════════════════════════════════ */
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_style_context_add_class(gtk_widget_get_style_context(sidebar), "sidebar");
    gtk_widget_set_size_request(sidebar, 200, -1);

    /* App title in sidebar */
    GtkWidget *appTitle = gtk_label_new("📚 LibraryMS");
    gtk_widget_set_margin_top(appTitle, 16);
    gtk_widget_set_margin_bottom(appTitle, 8);
    gtk_widget_set_margin_start(appTitle, 16);
    gtk_widget_set_halign(appTitle, GTK_ALIGN_START);
    {
        PangoAttrList *al = pango_attr_list_new();
        pango_attr_list_insert(al, pango_attr_weight_new(PANGO_WEIGHT_BOLD));
        pango_attr_list_insert(al, pango_attr_size_new_absolute(13 * PANGO_SCALE));
        pango_attr_list_insert(al, pango_attr_foreground_new(0xffff, 0xffff, 0xffff));
        gtk_label_set_attributes(GTK_LABEL(appTitle), al);
        pango_attr_list_unref(al);
    }
    gtk_box_pack_start(GTK_BOX(sidebar), appTitle, FALSE, FALSE, 0);

    /* Nav section label */
    GtkWidget *navLabel = gtk_label_new("BROWSE");
    gtk_style_context_add_class(gtk_widget_get_style_context(navLabel), "sidebar-title");
    gtk_widget_set_halign(navLabel, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(sidebar), navLabel, FALSE, FALSE, 0);

    gNavAll      = makeNavButton("  All Books",     G_CALLBACK(onNavAll));
    gNavSearch   = makeNavButton("  Search",        G_CALLBACK(onNavSearch));
    gNavBorrowed = makeNavButton("  Borrowed",      G_CALLBACK(onNavBorrowed));
    gtk_style_context_add_class(gtk_widget_get_style_context(gNavAll), "active");

    gtk_box_pack_start(GTK_BOX(sidebar), gNavAll,      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), gNavSearch,   FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), gNavBorrowed, FALSE, FALSE, 0);

    /* Admin section — only shown for ROLE_ADMIN */
    if (currentRole == ROLE_ADMIN) {
        GtkWidget *adminLabel = gtk_label_new("ADMIN");
        gtk_style_context_add_class(gtk_widget_get_style_context(adminLabel), "sidebar-title");
        gtk_widget_set_halign(adminLabel, GTK_ALIGN_START);
        gtk_widget_set_margin_top(adminLabel, 8);
        gtk_box_pack_start(GTK_BOX(sidebar), adminLabel, FALSE, FALSE, 0);

        gNavAdd    = makeNavButton("  Add Book",    G_CALLBACK(onNavAdd));
        gNavDelete = makeNavButton("  Delete Book", G_CALLBACK(onNavDelete));
        gtk_box_pack_start(GTK_BOX(sidebar), gNavAdd,    FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(sidebar), gNavDelete, FALSE, FALSE, 0);
    }

    /* Spacer */
    GtkWidget *spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_vexpand(spacer, TRUE);
    gtk_box_pack_start(GTK_BOX(sidebar), spacer, TRUE, TRUE, 0);

    /* Role badge at bottom */
    GtkWidget *roleBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_style_context_add_class(gtk_widget_get_style_context(roleBox), "role-box");
    gtk_widget_set_margin_bottom(roleBox, 12);

    GtkWidget *roleLabel = gtk_label_new("Signed in as");
    gtk_style_context_add_class(gtk_widget_get_style_context(roleLabel), "role-label");
    gtk_label_set_xalign(GTK_LABEL(roleLabel), 0.0);

    gRoleName = gtk_label_new(currentRole == ROLE_ADMIN ? "Admin" : "User");
    gtk_style_context_add_class(gtk_widget_get_style_context(gRoleName), "role-name");
    gtk_label_set_xalign(GTK_LABEL(gRoleName), 0.0);

    gtk_box_pack_start(GTK_BOX(roleBox), roleLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(roleBox), gRoleName,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), roleBox, FALSE, FALSE, 0);

    /* Logout button */
    GtkWidget *logoutBtn = makeNavButton("  Logout", G_CALLBACK(onLogout));
    gtk_style_context_add_class(gtk_widget_get_style_context(logoutBtn), "nav-btn-logout");
    gtk_widget_set_margin_bottom(logoutBtn, 8);
    gtk_box_pack_start(GTK_BOX(sidebar), logoutBtn, FALSE, FALSE, 0);

    /* ════════════════════════════════════════
       MAIN CONTENT
       ════════════════════════════════════════ */
    GtkWidget *mainArea = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(mainArea, TRUE);

    /* ── Top bar ── */
    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_style_context_add_class(gtk_widget_get_style_context(topbar), "topbar");
    gtk_widget_set_margin_start(topbar, 0);

    gPageTitle = gtk_label_new("All Books");
    gtk_style_context_add_class(gtk_widget_get_style_context(gPageTitle), "page-title");
    gtk_widget_set_hexpand(gPageTitle, TRUE);
    gtk_label_set_xalign(GTK_LABEL(gPageTitle), 0.0);

    gSearchEntry = gtk_entry_new();
    gtk_style_context_add_class(gtk_widget_get_style_context(gSearchEntry), "search-entry");
    gtk_entry_set_placeholder_text(GTK_ENTRY(gSearchEntry), "Search title, author, ID…");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(gSearchEntry),
                                       GTK_ENTRY_ICON_PRIMARY, "edit-find-symbolic");
    gtk_widget_set_size_request(gSearchEntry, 260, -1);
    g_signal_connect(gSearchEntry, "changed", G_CALLBACK(onSearchChanged), NULL);

    gtk_box_pack_start(GTK_BOX(topbar), gPageTitle,    TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(topbar), gSearchEntry,  FALSE, FALSE, 0);

    if (currentRole == ROLE_ADMIN) {
        gAddBtn = gtk_button_new_with_label("+ Add Book");
        g_signal_connect(gAddBtn, "clicked", G_CALLBACK(onNavAdd), NULL);
        gtk_box_pack_start(GTK_BOX(topbar), gAddBtn, FALSE, FALSE, 0);
    }

    /* ── Tree view ── */
    gStore = gtk_list_store_new(N_COLS,
        G_TYPE_INT,     /* COL_INDEX */
        G_TYPE_INT,     /* COL_ID */
        G_TYPE_STRING,  /* COL_TITLE */
        G_TYPE_STRING,  /* COL_AUTHOR */
        G_TYPE_INT,     /* COL_PAGES */
        G_TYPE_STRING,  /* COL_RATING */
        G_TYPE_STRING,  /* COL_STATUS */
        G_TYPE_STRING   /* COL_STATUS_COLOR */
    );

    gTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(gStore));
    gtk_style_context_add_class(gtk_widget_get_style_context(gTreeView), "book-tree");
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gTreeView), TRUE);
    gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(gTreeView), FALSE);
    g_signal_connect(gTreeView, "row-activated", G_CALLBACK(onRowActivated), NULL);

    /* Helper macro for simple text columns */
    #define ADD_TEXT_COL(title, col_id, min_w) do { \
        GtkCellRenderer   *r = gtk_cell_renderer_text_new(); \
        GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes( \
                                   title, r, "text", col_id, NULL); \
        gtk_tree_view_column_set_resizable(c, TRUE); \
        gtk_tree_view_column_set_min_width(c, min_w); \
        gtk_tree_view_append_column(GTK_TREE_VIEW(gTreeView), c); \
    } while(0)

    ADD_TEXT_COL("ID",     COL_ID,     50);
    ADD_TEXT_COL("Title",  COL_TITLE,  260);
    ADD_TEXT_COL("Author", COL_AUTHOR, 180);
    ADD_TEXT_COL("Pages",  COL_PAGES,  60);
    ADD_TEXT_COL("Rating", COL_RATING, 60);
    #undef ADD_TEXT_COL

    /* Status column with colored background */
    {
        GtkCellRenderer   *r = gtk_cell_renderer_text_new();
        g_object_set(r, "xpad", 8, "ypad", 3, NULL);
        GtkTreeViewColumn *c = gtk_tree_view_column_new_with_attributes(
                                   "Status", r,
                                   "text",             COL_STATUS,
                                   "cell-background",  COL_STATUS_COLOR,
                                   NULL);
        gtk_tree_view_column_set_min_width(c, 90);
        gtk_tree_view_append_column(GTK_TREE_VIEW(gTreeView), c);
    }

    /* Borrow/Return action column */
    {
        GtkCellRenderer   *r = gtk_cell_renderer_text_new();
        g_object_set(r, "xpad", 8, "ypad", 3,
                        "mode", GTK_CELL_RENDERER_MODE_ACTIVATABLE, NULL);
        GtkTreeViewColumn *c = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(c, "Action");
        gtk_tree_view_column_pack_start(c, r, TRUE);
        gtk_tree_view_column_set_cell_data_func(c, r, borrowReturnCellFunc, NULL, NULL);
        gtk_tree_view_column_set_min_width(c, 80);
        g_signal_connect(gTreeView, "button-press-event", G_CALLBACK(onTreeButtonPress), NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(gTreeView), c);
    }

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled), gTreeView);

    /* ── Status / pagination bar ── */
    GtkWidget *bottomBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_style_context_add_class(gtk_widget_get_style_context(bottomBar), "statusbar");

    gStatusLabel = gtk_label_new("");
    gtk_widget_set_hexpand(gStatusLabel, TRUE);
    gtk_label_set_xalign(GTK_LABEL(gStatusLabel), 0.0);

    gPageLabel = gtk_label_new("Page 1 of 1");
    gPrevBtn   = gtk_button_new_with_label("‹ Prev");
    gNextBtn   = gtk_button_new_with_label("Next ›");
    gtk_widget_set_sensitive(gPrevBtn, FALSE);
    gtk_widget_set_sensitive(gNextBtn, FALSE);
    g_signal_connect(gPrevBtn, "clicked", G_CALLBACK(onPrevPage), NULL);
    g_signal_connect(gNextBtn, "clicked", G_CALLBACK(onNextPage), NULL);

    gtk_box_pack_start(GTK_BOX(bottomBar), gStatusLabel, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(bottomBar), gPrevBtn,     FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(bottomBar), gPageLabel,   FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(bottomBar), gNextBtn,     FALSE, FALSE, 0);

    /* ── Assemble main area ── */
    gtk_box_pack_start(GTK_BOX(mainArea), topbar,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(mainArea), scrolled,  TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(mainArea), bottomBar, FALSE, FALSE, 0);

    /* ── Assemble root ── */
    gtk_box_pack_start(GTK_BOX(hbox), sidebar,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), mainArea, TRUE,  TRUE,  0);
    gtk_container_add(GTK_CONTAINER(gWindow), hbox);

    refreshBookList();

    return gWindow;
}