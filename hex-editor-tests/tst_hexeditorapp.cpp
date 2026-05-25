#include <QtTest>
#include <QTemporaryFile>
#include <QStackedWidget>
#include "../hexeditorapp.h"
#include "../mainwindow.h"

class HexEditorAppTest : public QObject {
    Q_OBJECT

private slots:
    void openFileViaCommandLineArgument() {
        QTemporaryFile tmp;
        QVERIFY(tmp.open());
        tmp.write("TEST");
        tmp.close();

        // Simulate command line arguments
        char prog[] = "hex-editor";
        QByteArray pathBytes = tmp.fileName().toLocal8Bit();
        char *argv[] = { prog, pathBytes.data() };
        int argc = 2;

        HexEditorApp hexApp;
        hexApp.initialize(argc, argv);

        MainWindow* window = hexApp.mainWindow();
        QVERIFY(window != nullptr);

        // Verify successful load
        QCOMPARE(window->m_stackedWidget->currentIndex(), 1);
        QCOMPARE(window->m_stackedWidget->currentWidget(), window->m_tableView);
        QCOMPARE(window->windowTitle(), QString("%1 — Hex Editor").arg(QFileInfo(tmp.fileName()).fileName()));
    }
};

QTEST_MAIN(HexEditorAppTest)
#include "tst_hexeditorapp.moc"