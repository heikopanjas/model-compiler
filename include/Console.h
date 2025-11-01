#ifndef __CONSOLE_H_INCL__
#define __CONSOLE_H_INCL__

// Set 8-byte alignment for all types in this header
#pragma pack(push, 8)

#include <string>

namespace bbfm {
/// \brief Console output utility for reporting errors and status messages
class Console
{
public:
    /// \brief Report an error message to stderr
    /// \param message The error message to display
    static void ReportError(const std::string& message);

    /// \brief Report a status message to stdout
    /// \param message The status message to display
    static void ReportStatus(const std::string& message);

private:
    // Static-only class - prevent instantiation
    Console()                          = delete;
    ~Console()                         = delete;
    Console(const Console&)            = delete;
    Console& operator=(const Console&) = delete;
};
} // namespace bbfm

// Restore previous alignment
#pragma pack(pop)

#endif // __CONSOLE_H_INCL__
