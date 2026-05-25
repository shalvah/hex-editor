#include <QtTest>
#include "../argumentparser.h"

class ArgumentParserTest : public QObject {
    Q_OBJECT

private slots:
    void noArgs() {
        // Simulate: program name only, no path given
        char prog[] = "hex-editor";
        char *argv[] = { prog };
        QCOMPARE(ArgumentParser::parse(1, argv), QString());
    }

    void withArg() {
        char prog[] = "hex-editor";
        char path[] = "/tmp/test.bin";
        char *argv[] = { prog, path };
        QCOMPARE(ArgumentParser::parse(2, argv), QString("/tmp/test.bin"));
    }
};

QTEST_MAIN(ArgumentParserTest)
#include "tst_argumentparser.moc"