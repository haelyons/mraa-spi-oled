/**
Purpose: Help establish multiple interfaces.

Source file in the GE-7000 Code Library.

PN: ME-N60-0
*/

#include <stdint.h>
#include <stddef.h>

// NHD DISPLAY PARAMS
#define SSD1362_OLED_WIDTH              256
#define SSD1362_OLED_HEIGHT             64
#define SSD1362_OLED_LINES              (NEWHAVEN_OLED_HEIGHT/8)

/* BASIC SSD1362 COMMANDS
    Sources:
    https://newhavendisplay.com/content/app_notes/SSD1362.pdf
    https://github.com/wjklimek1/SSD1362_OLED_library/blob/master/SSD1362_OLED_lib/SSD1362_API.h
*/
#define SSD1362_CMD_ENABLE_GREYS    0x00 // Enable greyscale table.
#define SSD1362_CMD_SET_COLS        0x15 // Start & end cols.
#define SSD1362_CMD_SET_WRITE       0x5c // Write to RAM enable.
#define SSD1362_CMD_SET_READ        0x5d // Read from RAM enable.
#define SSD1362_CMD_SET_ROWS        0x75 // Set start & end rows.
#define SSD1362_CMD_SET_REMAP       0xa0 // RAM addressing modes.
#define SSD1362_CMD_SET_START       0xa1 // Set RAM start line.
#define SSD1362_CMD_SET_OFFSET      0xa2 // Set RAM offset (vertical scroll).
#define SSD1362_CMD_SET_PIX_OFF     0xa4 // All pixels off.
#define SSD1362_CMD_SET_PIX_ON      0xa5 // All pixels on.
#define SSD1362_CMD_SET_PIX_NORM    0xa6 // Normal display.
#define SSD1362_CMD_SET_PIX_INV     0xa7 // Inverse display.
#define SSD1362_CMD_SET_PART_ON     0xa8 // Partial display on.
#define SSD1362_CMD_SET_PART_OFF    0xa9 // Partial display off.
#define SSD1362_CMD_SET_VDD         0xab // Set VDD source.
#define SSD1362_CMD_SET_DISP_OFF    0xae // Display logic off.
#define SSD1362_CMD_SET_DISP_ON     0xaf // Display logic on.
#define SSD1362_CMD_SET_PHASE       0xb1 // Clock phase length.
#define SSD1362_CMD_SET_CLOCK       0xb3 // Clock frequency/divider.
#define SSD1362_CMD_SET_ENHANCE_A   0xb4 // Display enhancement A.
#define SSD1362_CMD_SET_GPIOS       0xb5 // GPIO modes.
#define SSD1362_CMD_SET_PERIOD      0xb6 // Pre-charge period.
#define SSD1362_CMD_SET_GREYS       0xb8 // Set greyscale table values.
#define SSD1362_CMD_SET_GREYS_DEF   0xb9 // Set greyscale table to default.
#define SSD1362_CMD_SET_PRE_VOLT    0xbb // Pre-charge voltage.
#define SSD1362_CMD_SET_COM_VOLT    0xbe // Common voltage.
#define SSD1362_CMD_SET_CONTRAST    0xc1 // Contrast.
#define SSD1362_CMD_SET_BRIGHTNESS  0xc7 // Brightness.
#define SSD1362_CMD_SET_MUX         0xca // Mux.
#define SSD1362_CMD_SET_ENHANCE_B   0xd1 // Display enhancement B.
#define SSD1362_CMD_SET_LOCK        0xfd // Command lock.

// DEFAULTS
#define SSD1362_DEFAULT_COL1        0x00 //
#define SSD1362_DEFAULT_COL2        0x77 //
#define SSD1362_DEFAULT_ROW1        0x00 //
#define SSD1362_DEFAULT_ROW2        0x7f //
#define SSD1362_DEFAULT_REMAP1      0x00 //
#define SSD1362_DEFAULT_REMAP2      0x01 //
#define SSD1362_DEFAULT_START       0x00 //
#define SSD1362_DEFAULT_OFFSET      0x00 //
#define SSD1362_DEFAULT_VDD         0x01 //
#define SSD1362_DEFAULT_PHASE       0x74 //
#define SSD1362_DEFAULT_CLOCK       0x50 //
#define SSD1362_DEFAULT_ENHANCE_A1  0xa2 //
#define SSD1362_DEFAULT_ENHANCE_A2  0xb5 //
#define SSD1362_DEFAULT_GPIOS       0x0a //
#define SSD1362_DEFAULT_PERIOD      0x08 //
#define SSD1362_DEFAULT_PRE_VOLT    0x17 //
#define SSD1362_DEFAULT_COM_VOLT    0x04 //
#define SSD1362_DEFAULT_CONTRAST    0x7f //
#define SSD1362_DEFAULT_BRIGHTNESS  0xff //
#define SSD1362_DEFAULT_MUX         0x7f //
#define SSD1362_DEFAULT_ENHANCE_B1  0xa2 //
#define SSD1362_DEFAULT_ENHANCE_B2  0x20 //

// GPIO states.
#define SSD1362_INPUT_COMMAND 0 // Enable command mode for DC# pin.
#define SSD1362_INPUT_DATA    1 // Enable data mode for DC# pin.
#define SSD1362_RESET_ON      0 // Hardware reset.
#define SSD1362_RESET_OFF     1 // Normal operation.

// Ranges -- corresponding to width of NHD
#define SSD1362_COLS_MIN       0x1C // Start column
#define SSD1362_COLS_MAX       0x5B // End column -- 0x77 (119) for 480px SSD1362 MAX
#define SSD1362_ROWS_MIN       0x00 // Start row
#define SSD1362_ROWS_MAX       0x3F // End row -- 0x7f (127) for 128px SSD1362 MIN

// Settings.
#define SSD1362_INC_COLS       0x00 // Increment cols.
#define SSD1362_INC_ROWS       0x01 // Increment rows.
#define SSD1362_SCAN_RIGHT     0x00 // Scan columns left to right.
#define SSD1362_SCAN_LEFT      0x02 // Scan columns right to left.
#define SSD1362_SCAN_DOWN      0x00 // Scan rows from top to bottom.
#define SSD1362_SCAN_UP        0x10 // Scan rows from bottom to top.
#define SSD1362_VDD_EXTERNAL   0x00 // Use external VDD regulator.
#define SSD1362_VDD_INTERNAL   0x01 // Use internal VDD regulator (reset).

// Enable/disable.
#define SSD1362_PARTIAL_ON      0x01 // Partial mode on.
#define SSD1362_PARTIAL_OFF     0x00 // Partial mode off.
#define SSD1362_SPLIT_DISABLE   0x00 // Disable odd/even split of COMs.
#define SSD1362_SPLIT_ENABLE    0x20 // Enable odd/even split of COMS.
#define SSD1362_DUAL_DISABLE    0x00 // Disable dual COM line mode.
#define SSD1362_DUAL_ENABLE     0x10 // Enable dual COM line mode.
#define SSD1362_REMAP_DISABLE   0x00 // Disable nibble re-map.
#define SSD1362_REMAP_ENABLE    0x04 // Enable nibble re-map.
#define SSD1362_COMMAND_LOCK    0x16 // Command lock.
#define SSD1362_COMMAND_UNLOCK  0x12 // Command unlock.

// Resets
#define SSD1362_CLOCK_DIV_RESET  0x01
#define SSD1362_CLOCK_FREQ_RESET 0xc0
#define SSD1362_PERIOD_RESET     0x08

// Column offset
#define SSD1362_COL_OFFSET       0x1c
