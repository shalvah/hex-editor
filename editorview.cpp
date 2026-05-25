#include "editorview.h"
#include "constants.h"
#include <QLineEdit>
#include <QHeaderView>
#include <QPainter>

// ── EditorDelegate ───────────────────────────────────────────────────────────

EditorDelegate::EditorDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QWidget *EditorDelegate::createEditor(QWidget *parent,
                                      const QStyleOptionViewItem &,
                                      const QModelIndex &index) const
{
    auto *edit = new QLineEdit(parent);
    edit->setAlignment(Qt::AlignCenter);

    // Apply input mask based on panel so invalid characters can't be typed
    auto panel = EditorModel::panelForColumn(index.column());
    switch (panel) {
    case EditorModel::Panel::Hex:
        edit->setMaxLength(2);
        edit->setInputMask("HH"); // exactly 2 hex digits
        break;
    case EditorModel::Panel::Char:
        edit->setMaxLength(1);
        break;
    case EditorModel::Panel::Bin:
        edit->setMaxLength(8);
        edit->setInputMask("BBBBBBBB"); // exactly 8 binary digits
        break;
    }

    return edit;
}

void EditorDelegate::setEditorData(QWidget *editor,
                                   const QModelIndex &index) const
{
    auto *edit = qobject_cast<QLineEdit *>(editor);
    if (!edit) return;
    edit->setText(index.data(Qt::EditRole).toString());
    edit->selectAll();
}

void EditorDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
{
    auto *edit = qobject_cast<QLineEdit *>(editor);
    if (!edit) return;
    model->setData(index, edit->text(), Qt::EditRole);
}

// ── EditorView ───────────────────────────────────────────────────────────────

EditorView::EditorView(QWidget *parent)
    : QTableView(parent)
{
    setItemDelegate(&m_delegate);
    setShowGrid(true);
    setAlternatingRowColors(true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(22);
}

void EditorView::setEditorModel(EditorModel *model) {
    m_editorModel = model;
    setModel(model);
    applyColumnWidths();
}

void EditorView::applyColumnWidths() {
    if (!m_editorModel) return;

    for (int col = 0; col < Constants::TOTAL_COLUMNS; ++col) {
        auto panel = EditorModel::panelForColumn(col);
        switch (panel) {
        case EditorModel::Panel::Hex:  setColumnWidth(col, 36); break;
        case EditorModel::Panel::Char: setColumnWidth(col, 24); break;
        case EditorModel::Panel::Bin:  setColumnWidth(col, 72); break;
        }
    }
}

void EditorView::paintEvent(QPaintEvent *event) {
    // Draw the table normally first
    QTableView::paintEvent(event);

    // Then draw vertical lines to visually separate the three panels
    if (!m_editorModel) return;

    QPainter painter(viewport());
    painter.setPen(QPen(QColor(180, 180, 180), 2));

    // Draw a separator line after column 7 and after column 15
    for (int boundary : {Constants::BYTES_PER_ROW, Constants::BYTES_PER_ROW * 2}) {
        int x = columnViewportPosition(boundary);
        painter.drawLine(x, 0, x, viewport()->height());
    }
}