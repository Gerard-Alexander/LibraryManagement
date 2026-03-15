#ifndef GUI_COMMON_H
#define GUI_COMMON_H

#include <gtk/gtk.h>
#include "../book.h"
#include "../file_io.h"
#include "../search.h"
#include "../borrow.h"
#include "../admin.h"

/* App role */
typedef enum { ROLE_USER = 0, ROLE_ADMIN = 1 } AppRole;

/* Colors */
#define COLOR_SIDEBAR_BG    "#1e2a35"
#define COLOR_SIDEBAR_HOVER "#2c3e50"
#define COLOR_SIDEBAR_ACTIVE "#2980b9"
#define COLOR_ACCENT        "#2980b9"
#define COLOR_ACCENT_DARK   "#1f6391"
#define COLOR_DANGER        "#c0392b"
#define COLOR_DANGER_DARK   "#96281b"
#define COLOR_SUCCESS_BG    "#eafaf1"
#define COLOR_SUCCESS_FG    "#1e8449"
#define COLOR_WARN_BG       "#fef9e7"
#define COLOR_WARN_FG       "#b7770d"
#define COLOR_TEXT_MUTED    "#7f8c8d"
#define COLOR_BORDER        "#e8e8e8"
#define COLOR_ROW_HOVER     "#f0f7ff"

/* Shared app state */
extern AppRole  currentRole;
extern GtkWidget *mainWindow;

/* CSS provider (applied globally) */
void applyAppCSS(GtkWidget *widget);

#endif
