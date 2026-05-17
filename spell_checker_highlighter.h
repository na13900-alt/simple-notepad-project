#ifndef SPELL_CHECKER_HIGHLIGHTER_H
#define SPELL_CHECKER_HIGHLIGHTER_H

#include "spell_checker.h"

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QTextDocument>

class spell_checker_highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit spell_checker_highlighter(QTextDocument *document, const spell_checker &checker)
        : QSyntaxHighlighter(document)
          , checker(checker) {
        misspelled_format.setUnderlineColor(Qt::red);
        misspelled_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
    }

protected:
    void highlightBlock(const QString &text) override {
        static const QRegularExpression word_re(R"(\b[A-Za-z]+\b)");
        auto it = word_re.globalMatch(text);
        while (it.hasNext()) {
            const auto match = it.next();
            const std::string word = match.captured().toStdString();
            if (!checker.is_correct(word)) {
                setFormat(match.capturedStart(), match.capturedLength(), misspelled_format);
            }
        }
    }

private:
    const spell_checker &checker;
    QTextCharFormat misspelled_format;
};

#endif // SPELL_CHECKER_HIGHLIGHTER_H
