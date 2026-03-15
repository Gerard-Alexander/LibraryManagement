#ifndef GUI_MAIN_WINDOW_H
#define GUI_MAIN_WINDOW_H

#include "gui_common.h"

GtkWidget *createMainWindow(GtkApplication *app);
void       refreshBookList(void);

#endif