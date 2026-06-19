#include "runtime/String.h"

#include <iostream>
#include <string>
#include <utility>

// --- Minimal test harness ----------------------------------------------------

static int g_total  = 0;
static int g_passed = 0;
static int g_failed = 0;

#define CHECK(expr) \
    do { \
        ++g_total; \
        if (expr) { ++g_passed; } \
        else { \
            ++g_failed; \
            std::cerr << "  FAIL: " << __FILE__ << ":" << __LINE__ \
                      << " -- " << #expr << std::endl; \
        } \
    } while (false)

#define CHECK_EQ(a, b) \
    do { \
        ++g_total; \
        if ((a) == (b)) { ++g_passed; } \
        else { \
            ++g_failed; \
            std::cerr << "  FAIL: " << __FILE__ << ":" << __LINE__ \
                      << " -- expected (" << #a << " == " << #b << ")" \
                      << " got [" << (a) << "] vs [" << (b) << "]" << std::endl; \
        } \
    } while (false)

#define RUN_TEST(func) \
    do { \
        std::cout << "  " << #func << "..." << std::flush; \
        const int before = g_failed; \
        func(); \
        if (g_failed == before) { std::cout << " OK" << std::endl; } \
        else { std::cout << " FAILED" << std::endl; } \
    } while (false)

// --- Tests -------------------------------------------------------------------

using runtime::String;

void TestDefaultConstruction()
{
    const String value;
    CHECK(value.IsEmpty());
    CHECK_EQ(value.GetLength(), static_cast<size_t>(0));
    CHECK_EQ(value.GetValue(), std::string(""));
}

void TestConstructFromStdString()
{
    const std::string source("podcast");
    const String      value(source);

    CHECK(value.IsEmpty() == false);
    CHECK_EQ(value.GetLength(), source.size());
    CHECK_EQ(value.GetValue(), source);
}

void TestConstructFromCString()
{
    const String value("episode");

    CHECK(value.IsEmpty() == false);
    CHECK_EQ(value.GetLength(), static_cast<size_t>(7));
    CHECK_EQ(value.GetValue(), std::string("episode"));
}

void TestConstructFromNullCString()
{
    const String value(nullptr);

    CHECK(value.IsEmpty());
    CHECK_EQ(value.GetLength(), static_cast<size_t>(0));
    CHECK_EQ(value.GetValue(), std::string(""));
}

void TestSetValue()
{
    String value;

    value.SetValue("feed");

    CHECK(value.IsEmpty() == false);
    CHECK_EQ(value.GetLength(), static_cast<size_t>(4));
    CHECK_EQ(value.GetValue(), std::string("feed"));
}

void TestCopyConstruct()
{
    const String original("author");
    const String copy(original);

    CHECK_EQ(copy.GetValue(), original.GetValue());
}

void TestCopyAssign()
{
    const String original("publisher");
    String       target;

    target = original;

    CHECK_EQ(target.GetValue(), std::string("publisher"));
}

void TestCopyIndependence()
{
    String original("asset");
    String copy(original);

    copy.SetValue("media");

    CHECK_EQ(original.GetValue(), std::string("asset"));
    CHECK_EQ(copy.GetValue(), std::string("media"));
}

void TestMoveConstruct()
{
    String original("chapter");
    String moved(std::move(original));

    CHECK_EQ(moved.GetValue(), std::string("chapter"));
    CHECK(original.IsEmpty());
}

void TestMoveAssign()
{
    String original("transcript");
    String target;

    target = std::move(original);

    CHECK_EQ(target.GetValue(), std::string("transcript"));
    CHECK(original.IsEmpty());
}

void TestImplicitStdStringConversion()
{
    const String value("tag");

    const std::string converted = value;

    CHECK_EQ(converted, std::string("tag"));
}

void TestImplicitCStringConversion()
{
    const String value("guid");

    const char* converted = value;

    CHECK(nullptr != converted);
    CHECK_EQ(std::string(converted), std::string("guid"));
}

void TestSelfAssignment()
{
    String  value("category");
    String& ref = value;

    value = ref;

    CHECK_EQ(value.GetValue(), std::string("category"));
}

void TestSelfMoveAssignment()
{
    String  value("collection");
    String& ref = value;

    value = std::move(ref);

    CHECK_EQ(value.GetValue(), std::string("collection"));
}

// --- Main --------------------------------------------------------------------

int main()
{
    std::cout << "Running String tests..." << std::endl;

    RUN_TEST(TestDefaultConstruction);
    RUN_TEST(TestConstructFromStdString);
    RUN_TEST(TestConstructFromCString);
    RUN_TEST(TestConstructFromNullCString);
    RUN_TEST(TestSetValue);
    RUN_TEST(TestCopyConstruct);
    RUN_TEST(TestCopyAssign);
    RUN_TEST(TestCopyIndependence);
    RUN_TEST(TestMoveConstruct);
    RUN_TEST(TestMoveAssign);
    RUN_TEST(TestImplicitStdStringConversion);
    RUN_TEST(TestImplicitCStringConversion);
    RUN_TEST(TestSelfAssignment);
    RUN_TEST(TestSelfMoveAssignment);

    std::cout << std::endl << "Results: " << g_passed << " passed, " << g_failed << " failed, " << g_total << " total" << std::endl;

    return (g_failed > 0) ? 1 : 0;
}
