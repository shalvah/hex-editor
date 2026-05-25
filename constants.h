#pragma once
#include <QtGlobal>

namespace Constants {
/**
 * @brief MAX_FILE_SIZE We limit file sizes so we can maintain UI responsiveness.
 */
constexpr qint64 MAX_FILE_SIZE = 100 * 1024 * 1024; // 100 MB

/**
 * @brief BYTES_PER_ROW The number of bytes we want to render in a single row.
 * 16 works best for easy lining up with the hex indices, but 8 works better without needing a large screen.
 */
constexpr int BYTES_PER_ROW = 8;

/**
 * @brief PANEL_COUNT The three panels: Hex, Char, Bin
 */
constexpr int PANEL_COUNT   = 3;

/**
 * @brief TOTAL_COLUMNS Number of columns to show in the UI
 */
constexpr int TOTAL_COLUMNS = BYTES_PER_ROW * PANEL_COUNT; // 24

/**
 * @brief COLUMN_WIDTH_CHAR_PX Number of pixels to use for the columns in the CHAR panel.
 * CHAR columns display only one character, so need only a little space.
 */
constexpr int COLUMN_WIDTH_CHAR_PX = 32;
/**
 * @brief COLUMN_WIDTH_HEX_PX Number of pixels to use for the columns in the HEX panel.
 * HEX columns display two, so need a little more space.
 */
constexpr int COLUMN_WIDTH_HEX_PX = 36;
/**
 * @brief COLUMN_WIDTH_BIN_PX Number of pixels to use for the columns in the BIN panel.
 * BIN columns display 6, so we give them much more space.
 */
constexpr int COLUMN_WIDTH_BIN_PX = 72;
}