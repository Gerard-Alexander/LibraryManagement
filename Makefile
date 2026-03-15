CC        = gcc
CFLAGS    = -Wall -Wextra -g
GTK_FLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS  = $(shell pkg-config --libs   gtk+-3.0)

CORE_SRCS = file_io.c utils.c display.c search.c borrow.c admin.c menu.c
GUI_SRCS  = gui/gui_common.c gui/gui_login.c gui/gui_main_window.c \
            gui/gui_book_details.c gui/gui_borrow_dialog.c \
            gui/gui_add_book.c gui/gui_delete_dialog.c

all: gui

gui: main_gui.c $(CORE_SRCS) $(GUI_SRCS)
	$(CC) $(CFLAGS) $(GTK_FLAGS) -o library_gui.exe \
	    main_gui.c $(CORE_SRCS) $(GUI_SRCS) $(GTK_LIBS)

cli: main.c $(CORE_SRCS)
	$(CC) $(CFLAGS) -o main.exe main.c $(CORE_SRCS)

clean:
	del /Q *.exe 2>nul || rm -f *.exe

.PHONY: all gui cli clean
