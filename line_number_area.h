#ifndef LINE_NUMBER_AREA_H
#define LINE_NUMBER_AREA_H

#include <QColor>
#include <QPainter>
#include <QPlainTextEdit>
#include <QRect>
#include <QResizeEvent>
#include <QSize>
#include <QTextBlock>
#include <QWidget>

class line_number_editor;

class line_number_area : public QWidget {
public:
    explicit line_number_area(line_number_editor *editor);


    [[nodiscard]] QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    line_number_editor *editor;
};

class line_number_editor : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit line_number_editor(QWidget *parent = nullptr)
        : QPlainTextEdit(parent)
          , line_area(new line_number_area(this)) {
        connect(this, &QPlainTextEdit::blockCountChanged, this, &line_number_editor::update_width);
        connect(this, &QPlainTextEdit::updateRequest, this, &line_number_editor::update_area);
        connect(this, &QPlainTextEdit::cursorPositionChanged, this, [this] { line_area->update(); });
        update_width();
    }

    [[nodiscard]] int line_number_area_width() const {
        int digits = 1;
        int max = std::max(1, blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }
        return 6 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    }

    void line_number_area_paint(QPaintEvent *event) {
        QPainter painter(line_area);
        painter.fillRect(event->rect(), QColor(240, 240, 240));

        QTextBlock block = firstVisibleBlock();
        int block_number = block.blockNumber();
        int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        int bottom = top + qRound(blockBoundingRect(block).height());

        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top()) {
                const QString number = QString::number(block_number + 1);
                painter.setPen(QColor(120, 120, 120));
                painter.drawText(0, top, line_area->width() - 3,
                                 fontMetrics().height(), Qt::AlignRight, number);
            }
            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++block_number;
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        QPlainTextEdit::resizeEvent(event);
        const QRect cr = contentsRect();
        line_area->setGeometry(QRect(cr.left(), cr.top(), line_number_area_width(), cr.height()));
    }

private slots:
    void update_width() {
        setViewportMargins(line_number_area_width(), 0, 0, 0);
    }

    void update_area(const QRect &rect, int dy) {
        if (dy) {
            line_area->scroll(0, dy);
        } else {
            line_area->update(0, rect.y(), line_area->width(), rect.height());
        }
        if (rect.contains(viewport()->rect())) {
            update_width();
        }
    }

private:
    line_number_area *line_area;
};

inline line_number_area::line_number_area(line_number_editor *editor)
    : QWidget(editor)
      , editor(editor) {
}

inline QSize line_number_area::sizeHint() const {
    return {editor->line_number_area_width(), 0};
}

inline void line_number_area::paintEvent(QPaintEvent *event) {
    editor->line_number_area_paint(event);
}

#endif