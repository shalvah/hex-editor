#pragma once
#include <QString>

class ArgumentParser {
public:
    static QString parse(int argc, char *argv[]);
};