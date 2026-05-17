# Notepad — Implementation Notes

## Required Features

### 1. Exception Handling

`notepad_exception.h` defines a four-class hierarchy rooted at `std::runtime_error`:

- `notepad_exception` — base class for all application errors
- `file_not_found_exception` — thrown when `QFile::exists()` returns false
- `file_read_exception` — thrown when the file cannot be opened for reading or the stream reports an error
- `file_write_exception` — thrown when the file cannot be opened for writing

Both `open_file()` and `save_file()` wrap their Qt file I/O in `try / catch (const notepad_exception&)` blocks. Errors are displayed with `QMessageBox::critical(this, "Error", ex.what())`.

### 2. Spell Checker

**`spell_checker.h`** — Plain C++ class. Loads `data/words.txt` into a `std::set<std::string>` at startup. `is_correct()` lowercases the word and strips non-alphabetic characters before doing a set lookup (O(log n)). `suggestions()` collects words from the dictionary that share the first letter and have Levenshtein edit distance ≤ 2 from the target, then returns the closest five sorted by distance.

**`spell_checker_highlighter.h`** — Subclass of `QSyntaxHighlighter`. `highlightBlock()` scans each text block with a `\b[A-Za-z]+\b` regex and applies a red `SpellCheckUnderline` format to any word that fails `is_correct()`. The highlighter is attached to `editor->document()` in the constructor, giving real-time feedback as the user types.

**Right-click suggestions** — `main_window::contextMenuEvent()` overrides the default context menu. When the word under the cursor is misspelled, a `QMenu` shows up to 5 replacement suggestions at the top. Clicking a suggestion replaces the word via `QTextCursor`. Standard editor actions (undo, copy, paste…) are appended below the separator.

**Tools > Check Spelling…** — Calls `highlighter->rehighlight()` to force a full pass over the document, then shows an informational message box.

---

## Optional Features

### 1 — Cursor Line / Column Indicator

Three `QLabel` widgets are added to the status bar in `setup_status_bar()`: **Words**, **Lines**, and **Ln X, Col Y**. Both `textChanged` and `cursorPositionChanged` signals of the editor are connected to `update_status_bar()`, which reads `QTextCursor::blockNumber()` and `columnNumber()` for the cursor position, and splits the plain text on whitespace for the word count.

### 2 — Font Dialog

**Format > Font…** opens `QFontDialog::getFont()`. If text is selected the chosen font is merged into the selection's `QTextCharFormat`; otherwise `selectAll()` is called first to apply the font to the whole document.

### 3 — Color Picker

**Format > Text Color…** opens `QColorDialog::getColor()`. The chosen `QColor` is applied to the current selection (or the insertion point for future typing) via `editor->mergeCurrentCharFormat()` with a foreground brush.

### 4 — Print

**File > Print…** opens `QPrintDialog`. On acceptance the document is sent to the printer with `QTextEdit::print()`. Requires `Qt6::PrintSupport` in `CMakeLists.txt`.

### 5 — Recent Files

**File > Recent Files** is a `QMenu` that tracks the last five opened files. The list is stored in `QSettings("Notepad", "Notepad")` under the key `recent_files`, loaded on startup and saved in the destructor. Each entry is a clickable action that calls `open_file(path)` directly.

---

## File Overview

| File | Role |
|---|---|
| `main.cpp` | Entry point |
| `main_window.h / .cpp` | Main window — menus, toolbar, file I/O, status bar, spell-check context menu |
| `text_transform.h` | Five `text_transform` subclasses (uppercase, lowercase, capitalize, sentence case, swap case) |
| `notepad_exception.h` | Exception hierarchy |
| `spell_checker.h` | Word-list loader, `is_correct()`, Levenshtein `suggestions()` |
| `spell_checker_highlighter.h` | `QSyntaxHighlighter` subclass for real-time red underlines |
| `line_number_area.h` | `QPlainTextEdit` + margin widget for line numbers (available for activation) |
| `find_replace_dialog.ui` | Qt Designer form for Find / Replace |
| `word_frequency_dialog.ui` | Qt Designer form for Word Frequency |
| `CMakeLists.txt` | Build config — `Qt6::Widgets` + `Qt6::PrintSupport` |
| `data/words.txt` | 370 k-word public-domain dictionary |
| `data/images/` | SVG icons (bold, italic, underline) |
