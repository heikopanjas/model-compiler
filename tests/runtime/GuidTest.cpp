#include "runtime/Guid.h"

#include <cstring>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>

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

#define CHECK_NE(a, b) \
    do { \
        ++g_total; \
        if ((a) != (b)) { ++g_passed; } \
        else { \
            ++g_failed; \
            std::cerr << "  FAIL: " << __FILE__ << ":" << __LINE__ \
                      << " -- expected (" << #a << " != " << #b << ")" \
                      << " got [" << (a) << "]" << std::endl; \
        } \
    } while (false)

#define RUN_TEST(func) \
    do { \
        std::cout << "  " << #func << "..." << std::flush; \
        int before = g_failed; \
        func(); \
        if (g_failed == before) { std::cout << " OK" << std::endl; } \
        else { std::cout << " FAILED" << std::endl; } \
    } while (false)

// --- Tests -------------------------------------------------------------------

using bbfm::runtime::Guid;

// --- Construction & Validity -------------------------------------------------

void TestDefaultConstruction()
{
    Guid g;
    CHECK(g.IsValid());
    CHECK(!g.IsNil());
    CHECK(nullptr != g.GetBytes());

    // Canonical format: 36 chars, dashes at positions 8, 13, 18, 23
    std::string s = g.ToString();
    CHECK_EQ(static_cast<int>(s.size()), 36);
    CHECK_EQ(s[8], '-');
    CHECK_EQ(s[13], '-');
    CHECK_EQ(s[18], '-');
    CHECK_EQ(s[23], '-');

    // All chars should be lowercase hex digits or dashes
    bool allValid = true;
    for (char c : s)
    {
        if ('-' != c && !('0' <= c && c <= '9') && !('a' <= c && c <= 'f'))
        {
            allValid = false;
            break;
        }
    }
    CHECK(allValid);
}

void TestConstructFromCanonicalString()
{
    Guid g("550e8400-e29b-41d4-a716-446655440000");
    CHECK(g.IsValid());
    CHECK(!g.IsNil());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestConstructFromUppercaseString()
{
    Guid g("550E8400-E29B-41D4-A716-446655440000");
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestConstructFromNoDashString()
{
    Guid g("550e8400e29b41d4a716446655440000");
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestConstructFromBracedString()
{
    Guid g("{550e8400-e29b-41d4-a716-446655440000}");
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestConstructFromEmptyString()
{
    Guid g("");
    CHECK(!g.IsValid());
    CHECK(g.IsNil());
}

void TestConstructFromInvalidString()
{
    Guid g1("not-a-uuid");
    CHECK(!g1.IsValid());

    Guid g2("550e8400-e29b-41d4-a716");
    CHECK(!g2.IsValid());

    Guid g3("550e8400-e29b-41d4-a716-44665544000Z");
    CHECK(!g3.IsValid());

    Guid g4("550e8400e29b41d4a71644665544000");
    CHECK(!g4.IsValid());
}

void TestConstructFromNilString()
{
    Guid g("00000000-0000-0000-0000-000000000000");
    CHECK(g.IsValid());
    CHECK(g.IsNil());
    CHECK_EQ(g.ToString(), std::string("00000000-0000-0000-0000-000000000000"));
}

// --- Generation via default constructor --------------------------------------

void TestGenerateUniqueness()
{
    Guid a;
    Guid b;
    CHECK(a != b);
}

void TestGenerateBulkUniqueness()
{
    std::set<std::string> seen;
    constexpr int         COUNT = 100;

    for (int i = 0; i < COUNT; ++i)
    {
        Guid g;
        CHECK(g.IsValid());
        seen.insert(g.ToString());
    }
    CHECK_EQ(static_cast<int>(seen.size()), COUNT);
}

// --- GetValue / SetValue / ToString ------------------------------------------

void TestGetValueAndToString()
{
    Guid g("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
    CHECK_EQ(g.GetValue(), g.ToString());

    std::string s = g;
    CHECK_EQ(s, g.ToString());
}

void TestSetValue()
{
    Guid g;
    CHECK(g.IsValid());

    g.SetValue("550e8400-e29b-41d4-a716-446655440000");
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));

    g.SetValue("");
    CHECK(!g.IsValid());
}

void TestSetValueReplace()
{
    Guid g("550e8400-e29b-41d4-a716-446655440000");
    CHECK(g.IsValid());

    g.SetValue("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("a1b2c3d4-e5f6-7890-abcd-ef1234567890"));
}

// --- GetBytes ----------------------------------------------------------------

void TestGetBytes()
{
    Guid g("01020304-0506-0708-090a-0b0c0d0e0f10");
    CHECK(nullptr != g.GetBytes());

    const uint8_t expected[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    CHECK(0 == std::memcmp(g.GetBytes(), expected, 16));
}

void TestGetBytesDefault()
{
    Guid g;
    CHECK(nullptr != g.GetBytes());
}

// --- Copy Semantics ----------------------------------------------------------

void TestCopyConstruct()
{
    Guid original("550e8400-e29b-41d4-a716-446655440000");
    Guid copy(original);
    CHECK(copy.IsValid());
    CHECK(original == copy);
    CHECK_EQ(copy.ToString(), original.ToString());
}

void TestCopyAssign()
{
    Guid original("550e8400-e29b-41d4-a716-446655440000");
    Guid target;
    target = original;

    CHECK(target.IsValid());
    CHECK(original == target);
    CHECK_EQ(target.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestCopyIndependence()
{
    Guid original("550e8400-e29b-41d4-a716-446655440000");
    Guid copy(original);

    copy.SetValue("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
    CHECK_EQ(original.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
    CHECK_EQ(copy.ToString(), std::string("a1b2c3d4-e5f6-7890-abcd-ef1234567890"));
}

void TestCopyUninitialized()
{
    Guid a;
    Guid b(a);
    CHECK(b.IsValid());
    CHECK(a == b);
}

// --- Move Semantics ----------------------------------------------------------

void TestMoveConstruct()
{
    Guid original("550e8400-e29b-41d4-a716-446655440000");
    Guid moved(std::move(original));

    CHECK(moved.IsValid());
    CHECK_EQ(moved.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
    CHECK(!original.IsValid());
}

void TestMoveAssign()
{
    Guid        original("550e8400-e29b-41d4-a716-446655440000");
    Guid        target;
    std::string targetBefore = target.ToString();
    target                   = std::move(original);

    CHECK(target.IsValid());
    CHECK_EQ(target.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
    CHECK(!original.IsValid());
}

// --- Comparison Operators ----------------------------------------------------

void TestEqualitySameValue()
{
    Guid a("550e8400-e29b-41d4-a716-446655440000");
    Guid b("550e8400-e29b-41d4-a716-446655440000");
    CHECK(a == b);
    CHECK(!(a != b));
}

void TestEqualityDifferentValues()
{
    Guid a("550e8400-e29b-41d4-a716-446655440000");
    Guid b("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
    CHECK(a != b);
    CHECK(!(a == b));
}

void TestEqualityDefaultGuids()
{
    Guid a;
    Guid b;
    // Two default-constructed Guids are each randomly generated, so they differ
    CHECK(a != b);
}

void TestEqualityDefaultAndNil()
{
    Guid def;
    Guid nil = Guid::Nil();
    // Default is randomly generated, not nil
    CHECK(def != nil);
}

void TestLessThanOrdering()
{
    Guid a("00000000-0000-0000-0000-000000000001");
    Guid b("00000000-0000-0000-0000-000000000002");
    Guid c("ffffffff-ffff-ffff-ffff-ffffffffffff");

    CHECK(a < b);
    CHECK(b < c);
    CHECK(a < c);
    CHECK(!(b < a));
    CHECK(!(a < a));
}

void TestOrderedSet()
{
    std::set<Guid> s;
    s.insert(Guid("ffffffff-ffff-ffff-ffff-ffffffffffff"));
    s.insert(Guid("00000000-0000-0000-0000-000000000001"));
    s.insert(Guid("00000000-0000-0000-0000-000000000002"));

    auto it = s.begin();
    CHECK_EQ(it->ToString(), std::string("00000000-0000-0000-0000-000000000001"));
    ++it;
    CHECK_EQ(it->ToString(), std::string("00000000-0000-0000-0000-000000000002"));
    ++it;
    CHECK_EQ(it->ToString(), std::string("ffffffff-ffff-ffff-ffff-ffffffffffff"));
}

// --- Nil and IsNil -----------------------------------------------------------

void TestNilFunction()
{
    Guid nil = Guid::Nil();
    CHECK(nil.IsValid());
    CHECK(nil.IsNil());
    CHECK_EQ(nil.ToString(), std::string("00000000-0000-0000-0000-000000000000"));
}

void TestTwoNilsEqual()
{
    Guid a = Guid::Nil();
    Guid b = Guid::Nil();
    CHECK(a == b);
}

void TestIsNilOnGenerated()
{
    Guid g;
    CHECK(!g.IsNil());
}

void TestIsNilOnNonZero()
{
    Guid g("550e8400-e29b-41d4-a716-446655440000");
    CHECK(!g.IsNil());
}

// --- std::hash ---------------------------------------------------------------

void TestHashEqual()
{
    Guid            a("550e8400-e29b-41d4-a716-446655440000");
    Guid            b("550e8400-e29b-41d4-a716-446655440000");
    std::hash<Guid> hasher;
    CHECK_EQ(hasher(a), hasher(b));
}

void TestHashUnorderedSet()
{
    std::unordered_set<Guid> set;
    Guid                     a("550e8400-e29b-41d4-a716-446655440000");
    Guid                     b("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
    set.insert(a);
    set.insert(b);
    set.insert(a); // duplicate
    CHECK_EQ(static_cast<int>(set.size()), 2);
}

void TestHashDistribution()
{
    std::unordered_set<size_t> hashes;
    std::hash<Guid>            hasher;
    constexpr int              COUNT = 100;

    for (int i = 0; i < COUNT; ++i)
    {
        hashes.insert(hasher(Guid()));
    }
    // Expect at least 90% unique hashes
    CHECK(static_cast<int>(hashes.size()) >= 90);
}

void TestHashDefaultAndNilConsistent()
{
    Guid            nil1 = Guid::Nil();
    Guid            nil2 = Guid::Nil();
    std::hash<Guid> hasher;
    CHECK_EQ(hasher(nil1), hasher(nil2));
}

// --- Edge Cases --------------------------------------------------------------

void TestSelfAssignment()
{
    Guid  g("550e8400-e29b-41d4-a716-446655440000");
    Guid& ref = g;
    g         = ref;
    CHECK(g.IsValid());
    CHECK_EQ(g.ToString(), std::string("550e8400-e29b-41d4-a716-446655440000"));
}

void TestSelfMoveAssignment()
{
    Guid  g("550e8400-e29b-41d4-a716-446655440000");
    Guid& ref = g;
    g         = std::move(ref);
    // Should not crash -- state may be valid or moved-from
    (void)g.IsValid();
}

// --- Main --------------------------------------------------------------------

int main()
{
    std::cout << "Running Guid tests..." << std::endl;

    // Construction & Validity
    RUN_TEST(TestDefaultConstruction);
    RUN_TEST(TestConstructFromCanonicalString);
    RUN_TEST(TestConstructFromUppercaseString);
    RUN_TEST(TestConstructFromNoDashString);
    RUN_TEST(TestConstructFromBracedString);
    RUN_TEST(TestConstructFromEmptyString);
    RUN_TEST(TestConstructFromInvalidString);
    RUN_TEST(TestConstructFromNilString);

    // Generate (via default constructor)
    RUN_TEST(TestGenerateUniqueness);
    RUN_TEST(TestGenerateBulkUniqueness);

    // GetValue / SetValue / ToString
    RUN_TEST(TestGetValueAndToString);
    RUN_TEST(TestSetValue);
    RUN_TEST(TestSetValueReplace);

    // GetBytes
    RUN_TEST(TestGetBytes);
    RUN_TEST(TestGetBytesDefault);

    // Copy Semantics
    RUN_TEST(TestCopyConstruct);
    RUN_TEST(TestCopyAssign);
    RUN_TEST(TestCopyIndependence);
    RUN_TEST(TestCopyUninitialized);

    // Move Semantics
    RUN_TEST(TestMoveConstruct);
    RUN_TEST(TestMoveAssign);

    // Comparison Operators
    RUN_TEST(TestEqualitySameValue);
    RUN_TEST(TestEqualityDifferentValues);
    RUN_TEST(TestEqualityDefaultGuids);
    RUN_TEST(TestEqualityDefaultAndNil);
    RUN_TEST(TestLessThanOrdering);
    RUN_TEST(TestOrderedSet);

    // Nil and IsNil
    RUN_TEST(TestNilFunction);
    RUN_TEST(TestTwoNilsEqual);
    RUN_TEST(TestIsNilOnGenerated);
    RUN_TEST(TestIsNilOnNonZero);

    // std::hash
    RUN_TEST(TestHashEqual);
    RUN_TEST(TestHashUnorderedSet);
    RUN_TEST(TestHashDistribution);
    RUN_TEST(TestHashDefaultAndNilConsistent);

    // Edge Cases
    RUN_TEST(TestSelfAssignment);
    RUN_TEST(TestSelfMoveAssignment);

    std::cout << std::endl << "Results: " << g_passed << " passed, " << g_failed << " failed, " << g_total << " total" << std::endl;

    return (0 < g_failed) ? 1 : 0;
}
