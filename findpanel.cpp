#include "findpanel.h"
#include "bytebuffer.h"
#include "constants.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QKeyEvent>
#include <QShortcut>

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
        int matchLength = 0;
        m_matches = search(m_input->text(), mode, matchLength);
        m_current = m_matches.isEmpty() ? -1 : 0;
        m_model.setSearchMatches(m_matches, m_current, matchLength);
        updateStatusLabel();
        if (!m_matches.isEmpty())
            emit requestScrollToRow(m_matches[0] / Constants::BYTES_PER_ROW);
    });

    // Re-run search when mode changes
    connect(m_modeCombo, &QComboBox::currentIndexChanged, this, [this]() {
        m_input->textChanged(m_input->text());
    });

    // Set up "Previous" and "Next" buttons
    connect(prevBtn,  &QPushButton::clicked, this, &FindPanel::findPrevious);
    connect(nextBtn,  &QPushButton::clicked, this, &FindPanel::findNext);
    // Bind Enter key to "Next"
    connect(m_input, &QLineEdit::returnPressed, this, &FindPanel::findNext);
    // Set up "Close" button
    connect(closeBtn, &QPushButton::clicked, this, &FindPanel::clearResults);
    connect(closeBtn, &QPushButton::clicked, this, [this](){
        if (parentWidget()) parentWidget()->hide();
        hide();
    });
    // Bind Esc key to "Close" button
    connect(new QShortcut(Qt::Key_Escape, this), &QShortcut::activated, closeBtn, &QPushButton::click);
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
    
    int matchLength = 0;
    auto mode = static_cast<EditorModel::Panel>(m_modeCombo->currentData().toInt());
    search(m_input->text(), mode, matchLength);
    
    m_model.setSearchMatches(m_matches, m_current, matchLength);
    updateStatusLabel();
    emit requestScrollToRow(m_matches[m_current] / Constants::BYTES_PER_ROW);
}

void FindPanel::findPrevious() {
    if (m_matches.isEmpty()) return;
    m_current = (m_current - 1 + m_matches.size()) % m_matches.size();
    
    int matchLength = 0;
    auto mode = static_cast<EditorModel::Panel>(m_modeCombo->currentData().toInt());
    search(m_input->text(), mode, matchLength);
    
    m_model.setSearchMatches(m_matches, m_current, matchLength);
    updateStatusLabel();
    emit requestScrollToRow(m_matches[m_current] / Constants::BYTES_PER_ROW);
}

void FindPanel::clearResults() {
    m_matches.clear();
    m_current = -1;
    m_model.setSearchMatches({}, -1, 0);
    m_input->clear();
    updateStatusLabel();
}

void FindPanel::updateStatusLabel() {
    if (m_matches.isEmpty()) {
        m_status->setText(m_input->text().isEmpty() ? "" : "No matches");
        m_status->setStyleSheet("color: red;");
    } else {
        m_status->setText(QString("%1 / %2").arg(m_current + 1).arg(m_matches.size()));
        m_status->setStyleSheet("");
    }
}

QList<int> FindPanel::search(const QString &text, EditorModel::Panel mode, int &outMatchLength) const {
    outMatchLength = 0;
    if (text.trimmed().isEmpty()) return {};

    // Convert search text to a byte sequence based on mode
    QByteArray needle;
    const QString trimmed = text.trimmed();

    if (mode == EditorModel::Panel::Hex) {
        // Remove all whitespace to support both "FF 0A" and "FF0A"
        QString hexString = text;
        hexString.remove(QRegularExpression("\\s+"));
        
        // There must be an even number of characters
        if (hexString.length() % 2 != 0) return {};

        // Create byte sequences
        for (int i = 0; i < hexString.length(); i += 2) {
            bool ok;
            uint val = hexString.mid(i, 2).toUInt(&ok, 16);
            if (!ok || val > 0xFF) return {}; // Not a valid byte
            needle.append(static_cast<char>(val));
        }
    } else if (mode == EditorModel::Panel::Char) {
        // All characters are allowed; use Latin1/ASCII, since only single-character encodings are supported
        needle = trimmed.toLatin1();
    } else { // Bin
        QString binString = text;
        binString.remove(QRegularExpression("\\s+"));
        
        if (binString.length() % 8 != 0) return {};

        // Create byte sequences
        for (int i = 0; i < binString.length(); i += 8) {
            bool ok;
            uint val = binString.mid(i, 8).toUInt(&ok, 2);
            if (!ok || val > 0xFF) return {}; // Not a valid byte
            needle.append(static_cast<char>(val));
        }
    }

    if (needle.isEmpty()) return {};
    outMatchLength = needle.length();

    // Our search is a simple O(N) scan, which may struggle on larger files.
    // To prevent copying the whole array, we use a `const` reference.
    // In future, we could implement more sophisticated search algorithms.
    QList<int> results;
    const QByteArray &haystack = m_buffer.rawData();
    
    int index = 0;
    while ((index = haystack.indexOf(needle, index)) != -1) {
        results.append(index);
        index += 1; // Step forward by 1 to catch overlapping matches
    }

    return results;
}