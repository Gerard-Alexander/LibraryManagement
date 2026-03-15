#include "gui_common.h"
#include <stdio.h>
#include <string.h>

AppRole   currentRole = ROLE_USER;
GtkWidget *mainWindow  = NULL;

/* Resolve style.css relative to the executable at runtime */
static void findCSSPath(char *out, size_t size) {
    /* Try same directory as cwd first, then the gui/ subdir */
    const char *candidates[] = {
        "gui/style.css",
        "style.css",
        NULL
    };
    for (int i = 0; candidates[i]; i++) {
        FILE *f = fopen(candidates[i], "r");
        if (f) {
            fclose(f);
            strncpy(out, candidates[i], size - 1);
            out[size - 1] = '\0';
            return;
        }
    }
    /* Fallback */
    strncpy(out, "gui/style.css", size - 1);
    out[size - 1] = '\0';
}

void applyAppCSS(GtkWidget *widget) {
    (void)widget;

    char cssPath[512];
    findCSSPath(cssPath, sizeof(cssPath));

    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    gtk_css_provider_load_from_path(provider, cssPath, &error);

    if (error) {
        g_printerr("CSS load warning: %s\n", error->message);
        g_error_free(error);
    }

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}
