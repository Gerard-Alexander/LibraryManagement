# Library Management System

> A C-language library management application with a console (CLI) interface and a GTK3 graphical (GUI) interface.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Dependencies](#2-dependencies)
3. [Setup & Installation](#3-setup--installation)
4. [Compiling the Program](#4-compiling-the-program)
5. [Running the Program](#5-running-the-program)
6. [Project Structure](#6-project-structure)
7. [Architecture](#7-architecture)
8. [Roles & Permissions](#8-roles--permissions)
9. [Data Model](#9-data-model)
10. [Known Issues & Bug Fixes](#10-known-issues--bug-fixes)
11. [Quick Reference](#11-quick-reference)

---

## 1. Project Overview

The Library Management System is a desktop application written in C that allows users to browse, search, borrow, and return books from a CSV-based library catalog. It provides two interfaces:

- **GUI** (`library_gui.exe`) — a full graphical interface built with GTK3
- **CLI** (`main.exe`) — a console interface for terminal use

All book data is stored in and read from `books.csv`. No database engine is required.

### Key Features

- Browse all 9,999+ books with pagination (50 per page)
- Live search by title, author, ID, or page count
- Filter to view only currently borrowed books
- Borrow and return books with confirmation dialogs
- Full book detail view on row click
- Admin role: add new books and delete existing ones
- Login screen with role selection (Admin / User)
- Logout and return to role selection
- Persistent CSV storage — all changes saved immediately

---

## 2. Dependencies

### GUI Version

| Dependency | Version | Purpose |
|---|---|---|
| GCC | 14.x (MinGW) | C compiler |
| GTK3 | 3.24+ | GUI framework — windows, widgets, dialogs |
| GLib / GObject | 2.x | Core GTK data types, signals, memory |
| Pango | 1.x | Text rendering inside GTK widgets |
| Cairo | 1.x | 2D drawing backend used by GTK |
| MSYS2 | Latest | Provides MinGW64 toolchain and pacman on Windows |
| pkg-config | 0.29+ | Resolves GTK compiler and linker flags |

### CLI Version

No external dependencies beyond GCC. Can be compiled with any MinGW or MSYS2 GCC installation.

---

## 3. Setup & Installation

### Step 1 — Install MSYS2

Download the installer from **https://www.msys2.org** and install to the default path `C:\msys64`.

After installation, open the **MSYS2 MinGW x64** terminal (not the plain MSYS2 terminal).

### Step 2 — Install GTK3 and pkg-config

Run inside the MSYS2 MinGW x64 terminal:

```bash
pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config
```

Press `Y` when prompted. This installs approximately 80MB of GTK3 and its dependencies.

### Step 3 — Configure VS Code (optional)

Add the following to `.vscode/settings.json` to use MSYS2 as the default terminal in VS Code:

```json
{
    "terminal.integrated.defaultProfile.windows": "MSYS2",
    "terminal.integrated.profiles.windows": {
        "MSYS2": {
            "path": "C:\\msys64\\usr\\bin\\bash.exe",
            "args": ["-i"],
            "env": {
                "MSYSTEM": "MINGW64",
                "CHERE_INVOKING": "1"
            }
        }
    }
}
```

### Step 4 — File Placement

Ensure `books.csv` and the `gui/` folder are in the same directory as the compiled executable. The application resolves paths at runtime relative to the working directory.

---

## 4. Compiling the Program

> **Important:** Always compile from the MSYS2 MinGW x64 terminal or via the VS Code task. Compiling from PowerShell will fail because `pkg-config` is not on the Windows PATH.

### GUI Version

```bash
gcc $(pkg-config --cflags gtk+-3.0) -o library_gui.exe \
  main_gui.c file_io.c utils.c display.c search.c \
  borrow.c admin.c menu.c \
  gui/gui_common.c gui/gui_login.c gui/gui_main_window.c \
  gui/gui_book_details.c gui/gui_borrow_dialog.c \
  gui/gui_add_book.c gui/gui_delete_dialog.c \
  $(pkg-config --libs gtk+-3.0)
```

### CLI Version

```bash
gcc -Wall -Wextra -o main.exe \
  main.c file_io.c utils.c display.c \
  search.c borrow.c admin.c menu.c
```

### Using VS Code

Press **`Ctrl+Shift+B`** to run the Build GUI task defined in `.vscode/tasks.json`. The task uses MSYS2 bash directly so `pkg-config` and GTK flags resolve correctly.

---

## 5. Running the Program

### GUI

```bash
./library_gui.exe
```

The login screen appears. Click **Admin** to access book management features, or **User** to browse, search, borrow, and return books.

### CLI

```bash
./main.exe
```

The console menu presents role selection (Admin / User) followed by role-specific actions.

---

## 6. Project Structure

```
TempLibraryManagement/
├── books.csv                    ← book data (loaded at startup)
├── main.c                       ← CLI entry point
├── main_gui.c                   ← GUI entry point
├── book.h                       ← Book struct, MAX_BOOKS, extern globals
├── file_io.h / file_io.c        ← CSV load/save, owns library[] array
├── utils.h / utils.c            ← input helpers, string utilities
├── display.h / display.c        ← displayBooks() for CLI
├── search.h / search.c          ← searchBook(), bookMatchesKeyword()
├── borrow.h / borrow.c          ← borrowBook(), returnBook() for CLI
├── admin.h / admin.c            ← addBook(), deleteBook() for CLI
├── menu.h / menu.c              ← adminMenu(), userMenu() for CLI
├── Makefile                     ← build targets: gui, cli, clean
├── .vscode/
│   ├── tasks.json               ← Ctrl+Shift+B build task
│   ├── launch.json              ← F5 run/debug config
│   └── settings.json            ← MSYS2 terminal profile
└── gui/
    ├── style.css                ← GTK CSS (loaded at runtime)
    ├── gui_common.h / .c        ← shared state, CSS loader
    ├── gui_login.h / .c         ← login/role selection screen
    ├── gui_main_window.h / .c   ← sidebar, table, search, pagination
    ├── gui_book_details.h / .c  ← full detail dialog (row click)
    ├── gui_borrow_dialog.h / .c ← borrow/return confirm dialogs
    ├── gui_add_book.h / .c      ← add book form dialog
    └── gui_delete_dialog.h / .c ← delete confirm dialog
```

---

## 7. Architecture

### Core Logic Modules

The business logic lives in eight modules shared by both the CLI and GUI. Neither interface re-implements any logic — they call into these modules directly.

| Module | Header | Responsibility |
|---|---|---|
| `file_io.c` | `file_io.h` | Owns `library[]` and `bookCount` globals. Loads books via RFC 4180 CSV parser. `saveBooksToFile()` is the single write path for all mutations. |
| `utils.c` | `utils.h` | Input validation: `safeReadInt`, `safeReadLong`, `getLine`, `getValidatedISBN`, `getValidatedFloatInRange`. String utilities: `toLowerCase`, `containsIgnoreCase`, `removeQuotes`, `isNumber`. |
| `search.c` | `search.h` | `searchBook()` for CLI. `bookMatchesKeyword()` does AND-logic multi-keyword matching across title, author, ID, page count, and status. |
| `borrow.c` | `borrow.h` | `borrowBook()` and `returnBook()` for CLI — reads ID, validates, confirms, flips status, saves. |
| `admin.c` | `admin.h` | `addBook()` and `deleteBook()` for CLI — collects validated input, appends or splices `library[]`, saves. |
| `display.c` | `display.h` | `displayBooks()` prints the formatted table to stdout for CLI use. |
| `menu.c` | `menu.h` | `adminMenu()` and `userMenu()` — CLI do-while loops calling the above modules. |
| `book.h` | *(header only)* | Defines `struct Book` with all 13 fields. Declares `library[]` and `bookCount` as `extern`. Defines `MAX_BOOKS` (10000). |

### GUI Layer

The GUI is a separate layer on top of the core modules. It does not duplicate any logic.

| Module | Responsibility |
|---|---|
| `gui_common.c` | Defines `currentRole` and `mainWindow` globals. Loads `style.css` via `gtk_css_provider_load_from_path()`. |
| `gui_login.c` | Login screen with Admin/User role cards. Stores `GtkApplication` reference. On role click, destroys login window and creates the main window under the same `GtkApplication`. |
| `gui_main_window.c` | Main application window. Builds the sidebar, top bar, `GtkTreeView` table, and pagination bar. Owns filter and search state. `refreshBookList()` rebuilds the store on any change. Handles column click for borrow/return via `button-press-event`. |
| `gui_book_details.c` | Modal dialog showing all 13 fields of a book. Opened on row double-click. |
| `gui_borrow_dialog.c` | Confirm dialogs for borrow and return. Uses `gtk_dialog_add_action_widget()` with plain `GtkButton` widgets to avoid GTK's internal button styling overrides. |
| `gui_add_book.c` | Two-column form dialog for adding a book. Validates all required fields inline with an error label. Loops until valid input or Cancel. |
| `gui_delete_dialog.c` | Confirm dialog for deletion. On accept, splices `library[]` and calls `saveBooksToFile()`. |

### Data Flow

All data flows through a single source of truth: the `library[]` array and `bookCount` integer defined in `file_io.c`. Every operation reads from or writes to this array, and every mutation immediately calls `saveBooksToFile()`.

```
Startup:   books.csv  →  loadBooksFromFile()  →  library[]

Read:      library[]  →  refreshBookList()    →  GtkListStore  →  UI

Mutation:  UI action  →  dialog confirm  →  library[i].status = x
                                         →  saveBooksToFile()
                                         →  refreshBookList()
```

### CSV Parsing

The original `sscanf`-based parser was replaced with a full RFC 4180-compliant parser in `file_io.c`. It correctly handles:

- Quoted fields with embedded commas
- Escaped quotes inside quoted fields (`""`)
- Corrupt trailing columns from previous saves (`atoi` tolerates junk suffixes like `0"`)
- Records that the old parser silently dropped entirely

On save, all records are written in clean normalized format, healing any corruption on first write.

---

## 8. Roles & Permissions

| Feature | User | Admin |
|---|:---:|:---:|
| Browse all books | ✓ | ✓ |
| Search books | ✓ | ✓ |
| View borrowed books | ✓ | ✓ |
| View book details | ✓ | ✓ |
| Borrow a book | ✓ | ✓ |
| Return a book | ✓ | ✓ |
| Add a new book | — | ✓ |
| Delete a book | — | ✓ |

---

## 9. Data Model

### struct Book (`book.h`)

| Field | Type | Description |
|---|---|---|
| `bookID` | `int` | Unique numeric identifier |
| `title` | `char[200]` | Book title |
| `authors` | `char[200]` | Author(s), slash-separated |
| `average_rating` | `float` | Rating 0.0–5.0 |
| `isbn` | `char[20]` | 10-digit ISBN |
| `isbn13` | `char[20]` | 13-digit ISBN |
| `language_code` | `char[10]` | e.g. `eng`, `spa`, `fre` |
| `num_pages` | `int` | Page count |
| `ratings_count` | `int` | Number of ratings |
| `text_reviewers_count` | `int` | Number of text reviews |
| `publication_date` | `char[20]` | MM/DD/YYYY format |
| `publisher` | `char[100]` | Publisher name |
| `status` | `int` | `0` = Available, `1` = Borrowed |

### books.csv Format

The CSV file follows RFC 4180. The first row is a header. Each subsequent row is one book with 13 fields. The `status` column was added by this application and defaults to `0` (Available) if absent in the original file.

```
bookID,title,authors,average_rating,isbn,isbn13,language_code,
num_pages,ratings_count,text_reviews_count,publication_date,publisher,status
```

---

## 10. Known Issues & Bug Fixes

### Bugs Fixed During Development

| Bug | Fix |
|---|---|
| `safeReadLong` used `%d` instead of `%ld` | Fixed format specifier in `utils.c` — silently truncated long IDs on 64-bit systems |
| `sscanf` CSV parser dropped records with quoted commas | Replaced with full RFC 4180 parser in `file_io.c` |
| Corrupt status column (`0",0",0",0",1`) | New parser uses `atoi()` which ignores junk suffixes; normalized save heals records |
| `GtkApplication` quit when login window was destroyed | Pass `GtkApplication` to `createMainWindow()` so main window registers under the same app |
| Invalid `activated` signal on `GtkCellRendererText` | Replaced with `button-press-event` on the `GtkTreeView`, detecting clicks on the Action column |
| Dialog buttons overridden by GTK's internal stylesheet | Use `gtk_dialog_add_action_widget()` with pre-built `GtkButton` widgets instead of `gtk_dialog_new_with_buttons()` |

### Known Limitations

- No login authentication — role selection is on the honor system
- No multi-user support — single global `library[]` array
- Borrow status is per-installation and does not sync across machines
- `books.csv` is rewritten in full on every mutation — fine for 10,000 records, would need batching for much larger catalogs

---

## 11. Quick Reference

### Build & Run (MSYS2 MinGW x64 terminal)

```bash
# Navigate to project
cd /c/Users/YourName/Project/TempLibraryManagement

# Build GUI
gcc $(pkg-config --cflags gtk+-3.0) -o library_gui.exe \
  main_gui.c file_io.c utils.c display.c search.c \
  borrow.c admin.c menu.c \
  gui/gui_common.c gui/gui_login.c gui/gui_main_window.c \
  gui/gui_book_details.c gui/gui_borrow_dialog.c \
  gui/gui_add_book.c gui/gui_delete_dialog.c \
  $(pkg-config --libs gtk+-3.0)

# Run GUI
./library_gui.exe

# Build CLI
gcc -Wall -o main.exe main.c file_io.c utils.c display.c \
  search.c borrow.c admin.c menu.c

# Run CLI
./main.exe
```

### VS Code Shortcuts

| Action | Shortcut |
|---|---|
| Build GUI | `Ctrl+Shift+B` |
| Open terminal | `` Ctrl+` `` |
| Run GUI (in MSYS2 terminal) | `./library_gui.exe` |
| Build and run in one step | `Ctrl+Shift+P` → Tasks: Run Task → Build and Run GUI |

---

*Library Management System — C + GTK3*