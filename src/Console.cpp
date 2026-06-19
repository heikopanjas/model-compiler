#include "Console.h"
#include <iostream>

void Console::ReportError(const runtime::String& message)
{
    std::cerr << message.GetValue() << std::endl;
}

void Console::ReportStatus(const runtime::String& message)
{
    std::cout << message.GetValue() << std::endl;
}
