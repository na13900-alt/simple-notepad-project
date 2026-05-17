#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "spell_checker.h"
#include "spell_checker_highlighter.h"
#include "text_transform.h"

#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QTextDocument>
#include <QTextEdit>
#include <memory>
#include <vector>

namespace Ui {
    class find_replace_dialog;
    class word_frequency_dialog;
}

class main_window : public QMainWindow {
    Q_OBJECT

public:
    main_window();

    ~main_window() override;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void setup_file_menu();

    void setup_edit_menu();

    void setup_format_menu();

    void setup_format_toolbar();

    void setup_search_menu();

    void setup_tools_menu();

    void setup_view_menu();

    void setup_status_bar();

    void open_file(const QString &path = {});

    void save_file();

    void save_file_as();

    void update_title();

    void add_recent_file(const QString &path);

    void rebuild_recent_menu();

    void update_status_bar();

    void apply_transform(const text_transform &transform) const;

    void show_find_replace_dialog();

    void find_next(const QString &term, QTextDocument::FindFlags flags = {}) const;

    void replace_current(const QString &term, const QString &replacement,
                         QTextDocument::FindFlags flags = {}) const;

    void replace_all(const QString &term, const QString &replacement,
                     QTextDocument::FindFlags flags = {}) const;

    void show_word_frequency();

    void check_spelling();

    QTextEdit *editor{nullptr};
    QString current_file;
    std::vector<std::unique_ptr<text_transform> > transforms;

    QDialog *find_replace_dlg{nullptr};
    std::unique_ptr<Ui::find_replace_dialog> find_replace_ui;

    spell_checker checker;
    spell_checker_highlighter *highlighter{nullptr};

    QLabel *status_words{nullptr};
    QLabel *status_lines{nullptr};
    QLabel *status_cursor{nullptr};

    QStringList recent_files;
    QMenu *recent_menu{nullptr};
    static constexpr int max_recent = 5;
};

#endif // MAIN_WINDOW_H
