#pragma once
#include <QString>

/**
 * @brief The ArgumentParser class is responsible for parsing command-line input into usable arguments for the program.
 */
class ArgumentParser {
public:
    static QString parse(int argc, char *argv[]);
};