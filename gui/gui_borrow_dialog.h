#ifndef GUI_BORROW_DIALOG_H
#define GUI_BORROW_DIALOG_H

#include "gui_common.h"

/* Returns TRUE if the action was confirmed and performed */
gboolean showBorrowDialog(GtkWindow *parent, int bookIndex);
gboolean showReturnDialog(GtkWindow *parent, int bookIndex);

#endif
