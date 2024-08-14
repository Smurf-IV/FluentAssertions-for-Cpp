#define BOOST_TEST_MODULE ChamoisTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <map>

#include "Chamois.hpp"


using namespace evolutional;

BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_int_10)
{
    Assert::That(10).Should().Be(10, "10 should be equal to 10");
}
//
BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_int_10)
{
    Assert::That(10).Should().NotBe(5, "10 should be not equal to five");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeGreaterThan_int_10)
{
    Assert::That(10).Should().BeGreaterThan(5, "10 should be greater than five");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeGreaterThan_int_10_5)
{
    Assert::That(10).Should().BeGreaterThanOrEqual(5, "10 should be greater than or equal to five");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeGreaterThanOrEqual_int_10)
{
    Assert::That(10).Should().BeGreaterThanOrEqual(10, "10 should be greater or equal to 10");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeLessThan_int_10_20)
{
    Assert::That(10).Should().BeLessThan(20, "10 should be less than twenty");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeLessThanOrEqual_int_10_10)
{
    Assert::That(10).Should().BeLessThanOrEqual(10, "10 should be less than or equal 10");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_BeLessThanOrEqual_int_10_20)
{
    Assert::That(10).Should().BeLessThanOrEqual(20, "10 should be less than or equal twenty");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_10_BeInRange_int_5_20)
{
    Assert::That(10).Should().BeInRange(5, 20, "10 should bebetween 5 and 20");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_10_NotBeInRange_int_5_9)
{
    Assert::That(10).Should().NotBeInRange(5, 9, "10 should be not between 5 and 9");
}


BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_char_10)
{
    Assert::That((char)10).Should().Be((char)10, "10 should be equal to 10");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_char_10)
{
    Assert::That((char)10).Should().NotBe((char)5, "10 should be not equal to five");
}



BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_float_2_4)
{
    Assert::That(2.4f).Should().Be(2.4f, "2.4 should be equal to 2.4");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_float_2_4)
{
    Assert::That(2.4f).Should().NotBe(2.5f, "2.4 should be not equal to 2.5");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_True_bool)
{
    Assert::That(true).Should().BeTrue("true should be true");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_False_bool)
{
    Assert::That(false).Should().BeFalse("false should be false");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_Strings)
{
    const wchar_t* A = L"hello";
    const wchar_t* B = L"hello";
    Assert::That(A).Should().Be(B, "strings should be equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_Strings_Inline)
{
    Assert::That("hello").Should().Be("hello", "strings should be equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_Strings_Inline_char)
{
    Assert::That("hello").Should().Be("hello", "strings should be equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_Strings_Inline)
{
    Assert::That("hello").Should().NotBe("goodbye", "strings should be not equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_Strings_Inline_char)
{
    Assert::That("hello").Should().NotBe("goodbye", "strings not equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_String_StartsWith)
{
    Assert::That("hello").Should().StartWith("hel", "string should be starts with hel");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_Strings)
{
    const wchar_t* A = L"hello";
    const wchar_t* B = L"goodbye";
    Assert::That(A).Should().NotBe(B, "strings should be not equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Equal_Array)
{
    int A[5] = { 1, 2, 3, 4, 5 };
    int B[5] = { 1, 2, 3, 4, 5 };
    Assert::ThatArray(A).Should().Be(B, "arrays should be equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_Length_Array)
{
    int A[5] = { 1, 2, 3, 4, 5 };
    int B[6] = { 1, 2, 3, 4, 5, 6 };
    Assert::ThatArray(A).Should().NotBe(B, "arrays should be not equal");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_NotEqual_Array)
{
    int A[5] = { 1, 2, 3, 4, 5 };
    int B[5] = { 5, 4, 3, 2, 1 };
    Assert::ThatArray(A).Should().NotBe(B, "arrays should be not equal");
}

/*
Container tests
*/
BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Vector_Equal)
{
    std::vector<int> A = { 1, 2, 3, 4, 5 };
    std::vector<int> B = { 1, 2, 3, 4, 5 };

    Assert::ThatContainer(A).Should().Be(B, "vectors should be equal.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Vector_NotEqual)
{
    std::vector<int> A = { 1, 2, 3, 4, 5 };
    std::vector<int> B = { 5, 4, 3, 2, 1 };

    Assert::ThatContainer(A).Should().NotBe(B, "vectors should be not equal.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Vector_Empty)
{
    std::vector<int> A = {};

    Assert::ThatContainer(A).Should().BeEmpty("vectors should be empty.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Vector_NotEmpty)
{
    std::vector<int> A = { 1, 2, 3, 4, 5 };

    Assert::ThatContainer(A).Should().NotBeEmpty("vectors should be not empty.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Map_Equal)
{
    std::map<int, int> A;
    std::map<int, int> B;

    for (int i = 0; i < 5; ++i) {
        A.insert(std::make_pair(i, i));
        B.insert(std::make_pair(i, i));
    }

    Assert::ThatContainer(A).Should().Be(B, "map should be equal.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Map_NotEqual)
{
    std::map<int, int> A;
    std::map<int, int> B;

    for (int i = 0; i < 5; ++i) {
        A.insert(std::make_pair(i, i));
        B.insert(std::make_pair(5 - i, i));
    }

    Assert::ThatContainer(A).Should().NotBe(B, "map should be not equal.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Map_Empty)
{
    std::map<int, int> A;

    Assert::ThatContainer(A).Should().BeEmpty("map should be empty.");
}

BOOST_AUTO_TEST_CASE(Chamois_Assert_Container_Map_NotEmpty)
{
    std::map<int, int> A;

    for (int i = 0; i < 5; ++i) {
        A.insert(std::make_pair(i, i));
    }

    Assert::ThatContainer(A).Should().NotBeEmpty("map should be not empty.");
}

//#ifdef CHAMOIS_TEST_ENABLE_FAILING_TESTS
BOOST_AUTO_TEST_CASE(DemonstratingFailure_Chamois_Assert_NotEqual_Strings)
{
    const wchar_t* A = L"hello";
    const wchar_t* B = L"hello";
    Assert::That(A).Should().NotBe(B, "strings should be not equal");
}

BOOST_AUTO_TEST_CASE(DemonstratingFailure_Chamois_Assert_NotEqual_Length_Array)
{
    int A[5] = { 1, 2, 3, 4, 5 };
    int B[5] = { 1, 2, 3, 4, 5 };
    Assert::ThatArray(A).Should().NotBe(B, "arrays should be not equal");
}

BOOST_AUTO_TEST_CASE(DemonstratingFailure_Chamois_Assert_NotEqual_Array_Differs)
{
    int A[5] = { 1, 2, 3, 4, 5 };
    int B[5] = { 1, 2, 4, 4, 5 };
    Assert::ThatArray(A).Should().Be(B, "arrays should be not equal");
}

BOOST_AUTO_TEST_CASE(Use_Shared_Ptr)
{
    const auto strPtr = std::make_unique<std::string>("Test String");
    Assert::ThatPtr(strPtr).Should().NotBeNull("Problem Checking nullable string");
}
//#endif