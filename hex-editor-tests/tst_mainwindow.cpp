#include <QtTest>
#include <QTemporaryFile>
#include <QStackedWidget>
#include "../mainwindow.h"

class MainWindowTest : public QObject {
    Q_OBJECT

private slots:
    void stackTransitionOnLoad() {
        MainWindow window;

        // Verify initial state: Stack should be on the empty state (index 0)
        QCOMPARE(window.m_stackedWidget->currentIndex(), 0);
        QCOMPARE(window.m_stackedWidget->currentWidget(), window.m_emptyStateWidget);

        // Create a temporary file to load
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();

        // Load the file
        window.openFile(tmp.fileName());

        // Verify state after loading: Stack should be on the editor view (index 1)
        QCOMPARE(window.m_stackedWidget->currentIndex(), 1);
        QCOMPARE(window.m_stackedWidget->currentWidget(), window.m_tableView);
        QCOMPARE(window.windowTitle(), QString("%1 — Hex Editor").arg(QFileInfo(tmp.fileName()).fileName()));
    }
};

QTEST_MAIN(MainWindowTest)
#include "tst_mainwindow.moc"