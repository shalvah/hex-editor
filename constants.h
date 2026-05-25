#pragma once
#include <QtGlobal>

namespace Constants {
/**
 * @brief MAX_FILE_SIZE We limit file sizes so we can maintain UI responsiveness.
 */
constexpr qint64 MAX_FILE_SIZE = 100 * 1024 * 1024; // 100 MB
}