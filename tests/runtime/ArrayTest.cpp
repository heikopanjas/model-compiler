#include "runtime/Array.h"

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

using TestArray = runtime::Array<const char*>;

void TestDefaultConstruction()
{
    const TestArray array;

    CHECK_EQ(array.GetCount(), static_cast<size_t>(0));
    CHECK(nullptr == array.GetValueAt(0));
}

void TestAddAndGet()
{
    TestArray array;

    array.AddValue("C++");
    array.AddValue("Rust");

    CHECK_EQ(array.GetCount(), static_cast<size_t>(2));
    CHECK_EQ(std::string(array.GetValueAt(0)), std::string("C++"));
    CHECK_EQ(std::string(array.GetValueAt(1)), std::string("Rust"));
}

void TestNullValueBecomesEmpty()
{
    TestArray array;

    array.AddValue(nullptr);

    CHECK_EQ(array.GetCount(), static_cast<size_t>(1));
    CHECK_EQ(std::string(array.GetValueAt(0)), std::string(""));
}

void TestSetValueAt()
{
    TestArray array;

    array.AddValue("Rust");

    CHECK(array.SetValueAt(0, "Swift"));
    CHECK(array.SetValueAt(1, "C++") == false);
    CHECK_EQ(std::string(array.GetValueAt(0)), std::string("Swift"));
}

void TestClear()
{
    TestArray array;

    array.AddValue("C++");
    array.AddValue("Rust");
    array.Clear();

    CHECK_EQ(array.GetCount(), static_cast<size_t>(0));
    CHECK(nullptr == array.GetValueAt(0));
}

void TestCopyIndependence()
{
    TestArray original;
    original.AddValue("C++");

    TestArray copy(original);
    copy.SetValueAt(0, "Swift");

    CHECK_EQ(std::string(original.GetValueAt(0)), std::string("C++"));
    CHECK_EQ(std::string(copy.GetValueAt(0)), std::string("Swift"));
}

void TestMoveConstruction()
{
    TestArray original;
    original.AddValue("Rust");

    TestArray moved(std::move(original));

    CHECK_EQ(std::string(moved.GetValueAt(0)), std::string("Rust"));
    CHECK_EQ(original.GetCount(), static_cast<size_t>(0));
}

void TestMoveAssignment()
{
    TestArray original;
    original.AddValue("Swift");

    TestArray target;
    target = std::move(original);

    CHECK_EQ(std::string(target.GetValueAt(0)), std::string("Swift"));
    CHECK_EQ(original.GetCount(), static_cast<size_t>(0));
}

// --- Main --------------------------------------------------------------------

int main()
{
    std::cout << "Running Array tests..." << std::endl;

    RUN_TEST(TestDefaultConstruction);
    RUN_TEST(TestAddAndGet);
    RUN_TEST(TestNullValueBecomesEmpty);
    RUN_TEST(TestSetValueAt);
    RUN_TEST(TestClear);
    RUN_TEST(TestCopyIndependence);
    RUN_TEST(TestMoveConstruction);
    RUN_TEST(TestMoveAssignment);

    std::cout << std::endl << "Results: " << g_passed << " passed, " << g_failed << " failed, " << g_total << " total" << std::endl;

    return (g_failed > 0) ? 1 : 0;
}
