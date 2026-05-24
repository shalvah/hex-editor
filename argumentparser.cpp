#include "argumentparser.h"

QString ArgumentParser::parse(int argc, char *argv[]) {
    if (argc >= 2) {
        auto filename = argv[1];
        return QString::fromLocal8Bit(filename);
    }
    return {};
}