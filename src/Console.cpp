#include "Console.h"
#include <iostream>

namespace bbfm {
void Console::ReportError(const std::string& message)
{
    std::cerr << message << std::endl;
}

void Console::ReportStatus(const std::string& message)
{
    std::cout << message << std::endl;
}
} // namespace bbfm
