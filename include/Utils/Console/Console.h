#ifndef UTILS_CONSOLE_H
#define UTILS_CONSOLE_H

// STD
#include <stdint.h>
#include <stddef.h>

// Basic Text Colors
#define CRESET      "\033[0m"   // Reset all attributes
#define BLACK            "\033[0;30m" // Black
#define RED              "\033[0;31m" // Red
#define GREEN            "\033[0;32m" // Green
#define YELLOW           "\033[0;33m" // Yellow
#define BLUE             "\033[0;34m" // Blue
#define MAGENTA          "\033[0;35m" // Magenta
#define CYAN             "\033[0;36m" // Cyan
#define WHITE            "\033[0;37m" // White

// Bold Text Colors
#define BOLD_BLACK       "\033[1;30m" // Bold Black
#define BOLD_RED         "\033[1;31m" // Bold Red
#define BOLD_GREEN       "\033[1;32m" // Bold Green
#define BOLD_YELLOW      "\033[1;33m" // Bold Yellow
#define BOLD_BLUE        "\033[1;34m" // Bold Blue
#define BOLD_MAGENTA     "\033[1;35m" // Bold Magenta
#define BOLD_CYAN        "\033[1;36m" // Bold Cyan
#define BOLD_WHITE       "\033[1;37m" // Bold White

// Underlined Text Colors
#define UNDERLINE_BLACK  "\033[4;30m" // Underlined Black
#define UNDERLINE_RED    "\033[4;31m" // Underlined Red
#define UNDERLINE_GREEN  "\033[4;32m" // Underlined Green
#define UNDERLINE_YELLOW "\033[4;33m" // Underlined Yellow
#define UNDERLINE_BLUE   "\033[4;34m" // Underlined Blue
#define UNDERLINE_MAGENTA "\033[4;35m" // Underlined Magenta
#define UNDERLINE_CYAN   "\033[4;36m" // Underlined Cyan
#define UNDERLINE_WHITE  "\033[4;37m" // Underlined White

// Background Colors
#define BACKGROUND_BLACK "\033[40m"   // Black Background
#define BACKGROUND_RED   "\033[41m"   // Red Background
#define BACKGROUND_GREEN "\033[42m"   // Green Background
#define BACKGROUND_YELLOW "\033[43m"  // Yellow Background
#define BACKGROUND_BLUE  "\033[44m"   // Blue Background
#define BACKGROUND_MAGENTA "\033[45m" // Magenta Background
#define BACKGROUND_CYAN  "\033[46m"   // Cyan Background
#define BACKGROUND_WHITE "\033[47m"   // White Background

// High Intensity Colors (Bright Versions)
#define BRIGHT_BLACK     "\033[0;90m"  // Bright Black (Gray)
#define BRIGHT_RED       "\033[0;91m"  // Bright Red
#define BRIGHT_GREEN     "\033[0;92m"  // Bright Green
#define BRIGHT_YELLOW    "\033[0;93m"  // Bright Yellow
#define BRIGHT_BLUE      "\033[0;94m"  // Bright Blue
#define BRIGHT_MAGENTA   "\033[0;95m"  // Bright Magenta
#define BRIGHT_CYAN      "\033[0;96m"  // Bright Cyan
#define BRIGHT_WHITE     "\033[0;97m"  // Bright White

// High Intensity Background Colors
#define BRIGHT_BG_BLACK  "\033[0;100m" // Bright Black Background
#define BRIGHT_BG_RED    "\033[0;101m" // Bright Red Background
#define BRIGHT_BG_GREEN  "\033[0;102m" // Bright Green Background
#define BRIGHT_BG_YELLOW "\033[0;103m" // Bright Yellow Background
#define BRIGHT_BG_BLUE   "\033[0;104m" // Bright Blue Background
#define BRIGHT_BG_MAGENTA "\033[0;105m" // Bright Magenta Background
#define BRIGHT_BG_CYAN   "\033[0;106m" // Bright Cyan Background
#define BRIGHT_BG_WHITE  "\033[0;107m" // Bright White Background

// General Utility

const char*
Txt
  (const char* text);

#endif