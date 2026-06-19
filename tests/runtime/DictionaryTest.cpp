#include "runtime/Dictionary.h"

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

using TestDictionary = runtime::Dictionary<const char*, const char*>;

void TestDefaultConstruction()
{
    const TestDictionary dictionary;

    CHECK_EQ(dictionary.GetCount(), static_cast<size_t>(0));
    CHECK(dictionary.ContainsKey("Language") == false);
    CHECK(nullptr == dictionary.GetValue("Language"));
}

void TestSetAndGet()
{
    TestDictionary dictionary;

    dictionary.SetValue("Language", "Swift");

    CHECK_EQ(dictionary.GetCount(), static_cast<size_t>(1));
    CHECK(dictionary.ContainsKey("Language"));
    CHECK_EQ(std::string(dictionary.GetValue("Language")), std::string("Swift"));
}

void TestOverwritePreservesCount()
{
    TestDictionary dictionary;

    dictionary.SetValue("Language", "Rust");
    dictionary.SetValue("Language", "Swift");

    CHECK_EQ(dictionary.GetCount(), static_cast<size_t>(1));
    CHECK_EQ(std::string(dictionary.GetValue("Language")), std::string("Swift"));
}

void TestNullValueBecomesEmpty()
{
    TestDictionary dictionary;

    dictionary.SetValue("Experimental", nullptr);

    CHECK_EQ(dictionary.GetCount(), static_cast<size_t>(1));
    CHECK_EQ(std::string(dictionary.GetValue("Experimental")), std::string(""));
}

void TestNullAndEmptyKeysIgnored()
{
    TestDictionary dictionary;

    dictionary.SetValue(nullptr, "ignored");
    dictionary.SetValue("", "ignored");

    CHECK_EQ(dictionary.GetCount(), static_cast<size_t>(0));
}

void TestIndexAccess()
{
    TestDictionary dictionary;

    dictionary.SetValue("Language", "C++");
    dictionary.SetValue("FileExtension", ".h");

    CHECK_EQ(std::string(dictionary.GetKeyAt(0)), std::string("Language"));
    CHECK_EQ(std::string(dictionary.GetValueAt(0)), std::string("C++"));
    CHECK_EQ(std::string(dictionary.GetKeyAt(1)), std::string("FileExtension"));
    CHECK_EQ(std::string(dictionary.GetValueAt(1)), std::string(".h"));
    CHECK(nullptr == dictionary.GetKeyAt(2));
    CHECK(nullptr == dictionary.GetValueAt(2));
}

void TestCopyIndependence()
{
    TestDictionary original;
    original.SetValue("Language", "Rust");

    TestDictionary copy(original);
    copy.SetValue("Language", "Swift");

    CHECK_EQ(std::string(original.GetValue("Language")), std::string("Rust"));
    CHECK_EQ(std::string(copy.GetValue("Language")), std::string("Swift"));
}

void TestMoveConstruction()
{
    TestDictionary original;
    original.SetValue("Language", "C++");

    TestDictionary moved(std::move(original));

    CHECK_EQ(std::string(moved.GetValue("Language")), std::string("C++"));
    CHECK_EQ(original.GetCount(), static_cast<size_t>(0));
}

void TestMoveAssignment()
{
    TestDictionary original;
    original.SetValue("Language", "Swift");

    TestDictionary target;
    target = std::move(original);

    CHECK_EQ(std::string(target.GetValue("Language")), std::string("Swift"));
    CHECK_EQ(original.GetCount(), static_cast<size_t>(0));
}

// --- Main --------------------------------------------------------------------

int main()
{
    std::cout << "Running Dictionary tests..." << std::endl;

    RUN_TEST(TestDefaultConstruction);
    RUN_TEST(TestSetAndGet);
    RUN_TEST(TestOverwritePreservesCount);
    RUN_TEST(TestNullValueBecomesEmpty);
    RUN_TEST(TestNullAndEmptyKeysIgnored);
    RUN_TEST(TestIndexAccess);
    RUN_TEST(TestCopyIndependence);
    RUN_TEST(TestMoveConstruction);
    RUN_TEST(TestMoveAssignment);

    std::cout << std::endl << "Results: " << g_passed << " passed, " << g_failed << " failed, " << g_total << " total" << std::endl;

    return (g_failed > 0) ? 1 : 0;
}
