#include "findpanel.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QKeyEvent>

FindPanel::FindPanel(ByteBuffer &buffer, EditorModel &model, QWidget *parent)
    : QWidget(parent), m_buffer(buffer), m_model(model)
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);

    auto *closeBtn = new QPushButton("✕", this);
    closeBtn->setFixedSize(20, 20);
    closeBtn->setFlat(true);

    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItem("HEX",  static_cast<int>(EditorModel::Panel::Hex));
    m_modeCombo->addItem("CHAR", static_cast<int>(EditorModel::Panel::Char));
    m_modeCombo->addItem("BIN",  static_cast<int>(EditorModel::Panel::Bin));

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("Find…");
    m_input->setFixedWidth(200);

    auto *prevBtn = new QPushButton("▲ Previous", this);
    auto *nextBtn = new QPushButton("▼ Next",     this);

    m_status = new QLabel(this);
    m_status->setMinimumWidth(100);

    layout->addWidget(closeBtn);
    layout->addWidget(m_modeCombo);
    layout->addWidget(m_input);
    layout->addWidget(prevBtn);
    layout->addWidget(nextBtn);
    layout->addWidget(m_status);
    layout->addStretch();

    // Search as you type
    connect(m_input, &QLineEdit::textChanged, this, [this]() {
        auto mode = static_cast<EditorModel::Panel>(m_modeCombo->currentData().toInt());
        m_matches = search(m_input->text(), mode);
        m_current = m_matches.isEmpty() ? -1 : 0;
        m_model.setSearchMatches(m_matches, m_current);
        updateStatus();
        if (!m_matches.isEmpty())
            emit requestScrollToRow(m_matches[0] / Constants::BYTES_PER_ROW);
    });

    // Re-run search when mode changes
    connect(m_modeCombo, &QComboBox::currentIndexChanged, this, [this]() {
        m_input->textChanged(m_input->text());
    });

    connect(m_input, &QLineEdit::returnPressed, this, &FindPanel::findNext);

    connect(prevBtn,  &QPushButton::clicked, this, &FindPanel::findPrevious);
    connect(nextBtn,  &QPushButton::clicked, this, &FindPanel::findNext);
    connect(closeBtn, &QPushButton::clicked, this, &FindPanel::clearResults);
    connect(closeBtn, &QPushButton::clicked, this, [this](){
        if (parentWidget()) parentWidget()->hide();
        hide();
    });
}

void FindPanel::activate() {
    if (parentWidget()) parentWidget()->show();
    show();
    m_input->setFocus();
    m_input->selectAll();
}

void FindPanel::findNext() {
    if (m_matches.isEmpty()) return;
    m_current = (m_current + 1) % m_matches.size();
    m_model.setSearchMatches(m_matches, m_current);
    updateStatus();
    emit requestScrollToRow(m_matches[m_current] / Constants::BYTES_PER_ROW);
}

void FindPanel::findPrevious() {
    if (m_matches.isEmpty()) return;
    m_current = (m_current - 1 + m_matches.size()) % m_matches.size();
    m_model.setSearchMatches(m_matches, m_current);
    updateStatus();
    emit requestScrollToRow(m_matches[m_current] / Constants::BYTES_PER_ROW);
}

void FindPanel::clearResults() {
    m_matches.clear();
    m_current = -1;
    m_model.setSearchMatches({}, -1);
    m_input->clear();
    updateStatus();
}

void FindPanel::updateStatus() {
    if (m_matches.isEmpty()) {
        m_status->setText(m_input->text().isEmpty() ? "" : "No matches");
        m_status->setStyleSheet("color: red;");
    } else {
        m_status->setText(QString("%1 / %2").arg(m_current + 1).arg(m_matches.size()));
        m_status->setStyleSheet("");
    }
}

QList<int> FindPanel::search(const QString &text, EditorModel::Panel mode) const {
    if (text.trimmed().isEmpty()) return {};

    // Convert search text to a byte sequence based on mode
    QByteArray needle;
    const QString trimmed = text.trimmed();

    if (mode == EditorModel::Panel::Hex) {
        // Split on whitespace so "FF 0A" or "FF0A" both work
        const QStringList tokens = trimmed.split(QRegularExpression("\\s+"),
                                                 Qt::SkipEmptyParts);
        for (const QString &token : tokens) {
            bool ok;
            uint val = token.toUInt(&ok, 16);
            if (!ok || val > 0xFF) return {}; // invalid input
            needle.append(static_cast<char>(val));
        }
    } else if (mode == EditorModel::Panel::Char) {
        needle = trimmed.toLatin1();
    } else { // Bin
        const QStringList tokens = trimmed.split(QRegularExpression("\\s+"),
                                                 Qt::SkipEmptyParts);
        for (const QString &token : tokens) {
            bool ok;
            uint val = token.toUInt(&ok, 2);
            if (!ok || val > 0xFF) return {};
            needle.append(static_cast<char>(val));
        }
    }

    if (needle.isEmpty()) return {};

    // Naive search — optimized to prevent allocations
    QList<int> results;
    const QByteArray &haystack = m_buffer.rawData();
    
    int index = 0;
    while ((index = haystack.indexOf(needle, index)) != -1) {
        results.append(index);
        index += 1; // Step forward by 1 to catch overlapping matches, matching previous behavior
    }

    return results;
}