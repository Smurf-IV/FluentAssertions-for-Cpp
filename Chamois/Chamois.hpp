#pragma once

/*
 *    Copyright 2014 Oli Wilkinson
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *        distributed under the License is distributed on an "AS IS" BASIS,
 *        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *        See the License for the specific language governing permissions and
 *        limitations under the License.
 *
 *    Then modified by Smurf-IV from 2022-12-12 onwards
 *    https://github.com/Smurf-IV/FluentAssertions-for-Cpp/commits/master
 *    Current date is 2023-06-14
 */

#include <algorithm>
#if _HAS_CXX20
#include <format>
#endif
#include <functional>
#include <iostream>
#include <string>
#include <vector>

 // ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage // Needed to stop trying to replace the __FILE__ usage inside the callers function
 // ReSharper disable CppInconsistentNaming                  // Needed to look like Macros are the actual function calls inside callers function

#ifdef CHAMOIS_UNITTEST_FRAMEWORK
#undef CHAMOIS_UNITTEST_FRAMEWORK
#endif

#define CHAMOIS_UNITTEST_FRAMEWORK_MS 1
#define CHAMOIS_UNITTEST_FRAMEWORK_GTEST 2
#define CHAMOIS_UNITTEST_FRAMEWORK_BOOST 3
#define CHAMOIS_UNITTEST_FRAMEWORK_CATCH2 4

#ifdef MS_CPP_UNITTESTFRAMEWORK
#ifdef CHAMOIS_UNITTEST_FRAMEWORK
#error Multiple test frameworks found
#endif
#define CHAMOIS_UNITTEST_FRAMEWORK CHAMOIS_UNITTEST_FRAMEWORK_MS
#endif

#if defined(GTEST_INCLUDE_GTEST_GTEST_H_) || defined(GOOGLETEST_INCLUDE_GTEST_GTEST_H_)
#ifdef CHAMOIS_UNITTEST_FRAMEWORK
#error Multiple test frameworks found
#endif
#define CHAMOIS_UNITTEST_FRAMEWORK CHAMOIS_UNITTEST_FRAMEWORK_GTEST
#endif

#ifdef BOOST_TEST_MODULE
#include <boost/functional/hash.hpp>
#include <ostream>
#ifdef CHAMOIS_UNITTEST_FRAMEWORK
#error Multiple test frameworks found
#endif
#define CHAMOIS_UNITTEST_FRAMEWORK CHAMOIS_UNITTEST_FRAMEWORK_BOOST
#endif

#ifdef INTERNAL_CATCH_TRY
#ifdef CHAMOIS_UNITTEST_FRAMEWORK
#error Multiple test frameworks found
#endif
#define CHAMOIS_UNITTEST_FRAMEWORK CHAMOIS_UNITTEST_FRAMEWORK_CATCH2
#endif

#ifndef CHAMOIS_UNITTEST_FRAMEWORK
#error No supported test framework found
#endif

namespace evolutional
{
namespace detail
{
#if CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_MS
class MsAssert
{
    static std::wstring message(const std::string& because)
    {
        RETURN_WIDE_STRING(because.c_str());
    }

public:
    template <typename T> static void Equal(const T& expected_value, const T& actual_value, const std::string& because)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(expected_value, actual_value, message(because).c_str());
    }

    template <typename T>
    static void NotEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(expected_value, actual_value, message(because).c_str());
    }

    static void True(const bool& actual_value, const std::string& because)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(actual_value, message(because).c_str());
    }

    static void False(const bool& actual_value, const std::string& because)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse(actual_value, message(because).c_str());
    }

    static void Fail(const std::string& because)
    {
        Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(message(because).c_str());
    }

    template <typename T>
    static void GreaterEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        True(expected_value <= actual_value, because);
    }
    template <typename T>
    static void GreaterThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        True(expected_value < actual_value, because);
    }


    template <typename T>
    static void LessEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        True(expected_value >= actual_value, because);
    }
    template <typename T>
    static void LessThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        True(expected_value > actual_value, because);
    }

    static std::string GetStackMessage(const std::string& file, int line, const std::string& because)
    {
        // TODO: Use the Windows `__LineInfo(const wchar_t* pszFileName, const char* pszFuncName, int lineNumber)`
        // and pass into the asserts above
#if _HAS_CXX20
        auto format = std::format("Location: @ [{}({})]", file, line);
        if (!because.empty())
        {
            format += std::format("\nMessage : {}", because);
        }
        return format;
#else
        return "Location: @ [" + file + "(" + std::to_string(line) + ")]\n" + because;
#endif
    }

};
#endif

#if CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_GTEST
// https://google.github.io/googletest/advanced.html#asserting-on-subroutines-with-an-exception
class ThrowListener : public testing::EmptyTestEventListener
{
    // [DebuggerStepThrough] equivalent ??
    void OnTestPartResult(const testing::TestPartResult& result) override
    {
        if (result.type() == testing::TestPartResult::kFatalFailure)
        {
            // TODO: Replace filename and line number in "new" TestPartResult
            testing::TestPartResult result1(testing::TestPartResult::kFatalFailure, "", 0, result.message());
            throw testing::AssertionException(result1);
        }
    }
};

class GTestAssert
{
public:
    void static SetThrowListener()
    {
        // See https://google.github.io/googletest/advanced.html#asserting-on-subroutines-with-an-exception
        // and search for "Propagating Fatal Failures"
        if (static bool done = false; !done)
        {
            // If this does not fire then check that `GTEST_REMOVE_LEGACY_TEST_CASEAPI_` has been set in all test projects
            static auto listener = new ThrowListener();  // NOLINT(cppcoreguidelines-owning-memory)
            testing::UnitTest::GetInstance()->listeners().Append(listener);
            done = true;
        }
    }

public:
    template <float>
    static void Equal(const float& expected_value, const float& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_FLOAT_EQ(expected_value, actual_value) << because;
    }
    template <double>
    static void Equal(const double& expected_value, const double& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_DOUBLE_EQ(expected_value, actual_value) << because;
    }
    template <typename T>
    static void Equal(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_EQ(expected_value, actual_value) << because;
    }

    template <typename T>
    static void NotEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_NE(expected_value, actual_value) << because;
    }

    template <typename T>
    static void GreaterThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_GT(actual_value, expected_value) << because;
    }

    template <typename T>
    static void GreaterEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_GE(actual_value, expected_value) << because;
    }

    template <typename T>
    static void LessThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_LT(actual_value, expected_value) << because;
    }

    template <typename T>
    static void LessEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_LE(actual_value, expected_value) << because;
    }

    template <typename T>
    static void BeApproximately(const T& expected_value, const T& actual_value, const T& tolerance,
        const std::string& because)
    {
        SetThrowListener();
        ASSERT_NEAR(actual_value, expected_value, tolerance) << because;
    }

    static void True(const bool& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_TRUE(actual_value) << because;
    }

    static void False(const bool& actual_value, const std::string& because)
    {
        SetThrowListener();
        ASSERT_FALSE(actual_value) << because;
    }

    static void Fail(const std::string& because)
    {
        SetThrowListener();
        ASSERT_TRUE(false) << because;
    }

    static std::string GetStackMessage(const std::string& file, int line, const std::string& because)
    {
#if _HAS_CXX20
        auto format = std::format("Location: @ [{}({})]", file, line);
        if (!because.empty())
        {
            format += std::format("\nMessage : {}", because);
        }
        return format;
#else
        return "Location: @ [" + file + "(" + std::to_string(line) + ")]\n" + because;
#endif
    }
};
#endif

#if CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_BOOST
class BoostTestAssert
{
public:
    template <typename T> static void Equal(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_EQUAL(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::equal_impl_frwd(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    static void Equal(const char* expected_value, const char* actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_EQUAL(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::equal_impl_frwd(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    template <> /*static*/ void Equal(const float& expected_value, const float& actual_value, const std::string& because)
    {
        static auto tol = ::boost::math::fpc::percent_tolerance(0.0001);
        if (because.empty())
        {
            BOOST_REQUIRE_CLOSE_FRACTION(actual_value, expected_value, tol);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::check_is_close_t(), because, REQUIRE, CHECK_MSG, (expected_value)(actual_value)(tol));
        }
    }

    template <> /*static*/ void Equal(const double& expected_value, const double& actual_value, const std::string& because)
    {
        static auto tol = ::boost::math::fpc::percent_tolerance(0.0001);
        if (because.empty())
        {
            BOOST_REQUIRE_CLOSE_FRACTION(actual_value, expected_value, tol);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::check_is_close_t(), because, REQUIRE, CHECK_MSG, (expected_value)(actual_value)(tol));
        }
    }

    template <typename T>
    static void NotEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_NE(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::ne_impl(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    static void Equal(const wchar_t* expected_value, const wchar_t* actual_value, const std::string& because)
    {
        Equal(std::wstring(expected_value), std::wstring(actual_value), because);
    }

    static void Equal(const std::wstring& expected_value, const std::wstring& actual_value, const std::string& because)
    {
        boost::hash<std::wstring> x1;
        boost::hash<std::wstring> x2;
        if (because.empty())
        {
            //binary '<<' : no operator found which takes a right - hand operand of type'const std::wstring' (or there is no acceptable conversion)
            BOOST_REQUIRE_EQUAL(x1(actual_value), x2(expected_value));
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::equal_impl_frwd(), because, REQUIRE, CHECK_MSG, (x1(actual_value))(x2(expected_value)));
        }
    }

    static void NotEqual(const wchar_t* expected_value, const wchar_t* actual_value, const std::string& because)
    {
        NotEqual(std::wstring(expected_value), std::wstring(actual_value), because);
    }

    static void NotEqual(const std::wstring& expected_value, const std::wstring& actual_value, const std::string& because)
    {
        boost::hash<std::wstring> x1;
        boost::hash<std::wstring> x2;
        if (because.empty())
        {
            //binary '<<' : no operator found which takes a right - hand operand of type'const std::wstring' (or there is no acceptable conversion)
            BOOST_REQUIRE_NE(x1(actual_value), x2(expected_value));
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::ne_impl(), because, REQUIRE, CHECK_MSG, (x1(actual_value))(x2(expected_value)));
        }
    }

    static void True(const bool& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE(actual_value);
        }
        else
        {
            BOOST_REQUIRE_MESSAGE(actual_value, because);
        }
    }

    static void False(const bool& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE(!actual_value);
        }
        else
        {
            BOOST_REQUIRE_MESSAGE(!actual_value, because);
        }
    }

    static void Fail(const std::string& because)
    {
        BOOST_FAIL(because);
    }

    template <typename T>
    static void GreaterThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_GT(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::gt_impl(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    template <typename T>
    static void GreaterEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_GE(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::ge_impl(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    template <typename T>
    static void LessThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_LT(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::lt_impl(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    template <typename T>
    static void LessEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        if (because.empty())
        {
            BOOST_REQUIRE_LE(actual_value, expected_value);
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::le_impl(), because, REQUIRE, CHECK_MSG, (actual_value)(expected_value));
        }
    }

    static std::string GetStackMessage(const std::string& /*file*/, int /*line*/, const std::string& because)
    {
        // DO nothing as Boost stacks traces the error ;-)
        return because;
    }

};
#endif

#if CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_CATCH2
class Catch2TestAssert
{
public:
    template <typename T> static void Equal(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value == expected_value);
    }
    static void Equal(const char* expected_value, const char* actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE_THAT(actual_value, Catch::Matchers::Equals(expected_value, Catch::CaseSensitive::Yes));
    }
    /*
    template <> static void Equal(const float& expected_value, const float& actual_value, const std::string& because)
                {
                    static auto tol = ::boost::math::fpc::percent_tolerance(0.0001);
                    if (because.empty())
                    {
                        BOOST_REQUIRE_CLOSE_FRACTION(actual_value, expected_value, tol);
                    }
                    else
                    {
                        BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::check_is_close_t(), because, REQUIRE, CHECK_MSG, (expected_value)(actual_value)(tol));
                    }
                }

                template <> static void Equal(const double& expected_value, const double& actual_value, const std::string& because)
                {
                    static auto tol = ::boost::math::fpc::percent_tolerance(0.0001);
                    if (because.empty())
                    {
                        BOOST_REQUIRE_CLOSE_FRACTION(actual_value, expected_value, tol);
                    }
                    else
                    {
                        BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::check_is_close_t(), because, REQUIRE, CHECK_MSG, (expected_value)(actual_value)(tol));
                    }
                }
    */
    template <typename T>
    static void NotEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value != expected_value);
    }
    /*
    static void Equal(const wchar_t* expected_value, const wchar_t* actual_value, const std::string& because)
    {
        Equal(std::wstring(expected_value), std::wstring(actual_value), because);
    }

    static void Equal(const std::wstring& expected_value, const std::wstring& actual_value, const std::string& because)
    {
        boost::hash<std::wstring> x1;
        boost::hash<std::wstring> x2;
        if (because.empty())
        {
            //binary '<<' : no operator found which takes a right - hand operand of type'const std::wstring' (or there is no acceptable conversion)
            BOOST_REQUIRE_EQUAL(x1(actual_value), x2(expected_value));
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::equal_impl_frwd(), because, REQUIRE, CHECK_MSG, (x1(actual_value))(x2(expected_value)));
        }
    }

    static void NotEqual(const wchar_t* expected_value, const wchar_t* actual_value, const std::string& because)
    {
        NotEqual(std::wstring(expected_value), std::wstring(actual_value), because);
    }

    static void NotEqual(const std::wstring& expected_value, const std::wstring& actual_value, const std::string& because)
    {
        boost::hash<std::wstring> x1;
        boost::hash<std::wstring> x2;
        if (because.empty())
        {
            //binary '<<' : no operator found which takes a right - hand operand of type'const std::wstring' (or there is no acceptable conversion)
            BOOST_REQUIRE_NE(x1(actual_value), x2(expected_value));
        }
        else
        {
            BOOST_TEST_TOOL_IMPL(0, ::boost::test_tools::tt_detail::ne_impl(), because, REQUIRE, CHECK_MSG, (x1(actual_value))(x2(expected_value)));
        }
    }
    */
    static void True(const bool& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value);
    }

    template <typename T>
    static void BeApproximately(const T& expected_value, const T& actual_value, const T& tolerance,
        const std::string& because)
    {
        CAPTURE(because);
        REQUIRE_THAT(actual_value, Catch::Matchers::WithinAbs(expected_value, tolerance));  // NOLINT(clang-diagnostic-implicit-int-float-conversion)
    }

    static void False(const bool& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE_FALSE(actual_value);
    }

    static void Fail(const std::string& because)
    {
        INFO(because); REQUIRE(false);
    }

    template <typename T>
    static void GreaterThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value > expected_value);
    }

    template <typename T>
    static void GreaterEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value >= expected_value);
    }

    template <typename T>
    static void LessThan(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value < expected_value);
    }

    template <typename T>
    static void LessEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE(actual_value <= expected_value);
    }

    template <typename _EXPECTEDEXCEPTION, typename FUNCTOR>
    static void Throw(const FUNCTOR func, const std::string& because)
    {
        CAPTURE(because);
        REQUIRE_THROWS_AS(func(), _EXPECTEDEXCEPTION);
    }

    template <typename NOTEXPECTEDEXCEPTION, typename FUNCTOR>
    static void NotThrow(const FUNCTOR func, const std::string& because)
    {
        CAPTURE(because);
        NOTEXPECTEDEXCEPTION notType;
        REQUIRE_NOTHROW(func, notType);
    }

    static std::string GetStackMessage(const std::string& file, int line, const std::string& because)
    {
        // TODO: Use the Windows `__LineInfo(const wchar_t* pszFileName, const char* pszFuncName, int lineNumber)`
        // and pass into the asserts above
#if _HAS_CXX20
        auto format = std::format("Location: @ [{}({})]", file, line);
        if (!because.empty())
        {
            format += std::format("\nMessage : {}", because);
        }
        return format;
#else
        return "Location: @ [" + file + "(" + std::to_string(line) + ")]\n" + because;
#endif
    }

};
#endif

template <class TAssert> class TAssertInternal
{
public:
    template <typename T>
    void AssertEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::Equal(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertNotEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::NotEqual(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertGreater(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::GreaterThan(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertLess(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::LessThan(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertGreaterEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::GreaterEqual(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertLessEqual(const T& expected_value, const T& actual_value, const std::string& because)
    {
        TAssert::LessEqual(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertInRange(const T& actual_value, const T& min_value, const T& max_value, const std::string& because)
    {
        TAssert::True(min_value <= actual_value && actual_value <= max_value, because);
    }

    template <typename T>
    void AssertNotInRange(const T& actual_value, const T& min_value, const T& max_value,
        const std::string& because)
    {
        TAssert::True(min_value > actual_value || actual_value > max_value, because);
    }

    template <typename T>
    void AssertBeApproximately(const T& expected_value, const T& actual_value,
        const T& tolerance,
        const std::string& because)
    {
        TAssert::BeApproximately(expected_value, actual_value, tolerance, because);
    }

    template <typename T>
    void AssertArrayEqual(const T* expected_values, const size_t expected_values_len, const T* actual_values,
        const size_t actual_values_len, const std::string& because)
    {
        TAssert::Equal(expected_values_len, actual_values_len, "Array lengths differ\n" + because);
        for (size_t i = 0; i < expected_values_len; ++i)
        {
            if (expected_values[i] != actual_values[i])
            {
                std::string message("Array contents differ at index: ");
                message += std::to_string(i);
                message += "\n";
                message += because;
                TAssert::Fail(message);
            }
        }
    }

    template <typename T>
    void AssertArrayNotEqual(const T* expected_values, const size_t expected_values_len, const T* actual_values,
        const size_t actual_values_len, const std::string& because)
    {
        if (expected_values_len != actual_values_len)
        {
            return;
        }

        const auto min_len = (std::min)(actual_values_len, expected_values_len);
        bool expected_lhs = false;
        if (expected_values_len < actual_values_len)
        {
            expected_lhs = true;
        }

        const T* lhs = expected_lhs ? expected_values : actual_values;
        const T* rhs = !expected_lhs ? expected_values : actual_values;

        for (size_t i = 0; i < min_len; ++i)
        {
            if (lhs[i] != rhs[i])
            {
                return;
            }
        }
        TAssert::Fail("Array contents the same\n" + because);
    }

    void AssertTrue(const bool& actual_value, const std::string& because)
    {
        TAssert::True(actual_value, because);
    }

    void AssertFalse(const bool& actual_value, const std::string& because)
    {
        TAssert::False(actual_value, because);
    }

    template <typename T>
    void AssertEqual(const T* expected_value, const T* actual_value, const std::string& because)
    {
        TAssert::Equal(expected_value, actual_value, because);
    }

    template <typename T>
    void AssertNotEqual(const T* expected_value, const T* actual_value, const std::string& because)
    {
        TAssert::NotEqual(expected_value, actual_value, because);
    }

    void AssertStringLength(const size_t& expected_length, const std::wstring& actual_string,
        const std::string& because)
    {
        TAssert::Equal(expected_length, actual_string.length(), because);
    }

    void AssertNotStringLength(const size_t& expected_length, const std::wstring& actual_string,
        const std::string& because)
    {
        TAssert::NotEqual(expected_length, actual_string.length(), because);
    }

    void AssertStringEmpty(const std::wstring& actual_string, const std::string& because)
    {
        TAssert::Equal(0, actual_string.length(), because);
    }

    void AssertNotStringEmpty(const std::wstring& actual_string, const std::string& because)
    {
        TAssert::NotEqual(0, actual_string.length(), because);
    }

    void AssertStringStartsWith(const std::string& expected_prefix, const std::string& actual_string,
        const std::string& because)
    {
#if _HAS_CXX20
        if (!actual_string.starts_with(expected_prefix))
        {
            TAssert::True(false, std::format("Actual [{}] does not start with [{}]: because {}", actual_string, expected_prefix, because));
        }
#else
        if (actual_string.substr(0, expected_prefix.length()) != expected_prefix)
        {
            TAssert::True(false, "Actual [" + actual_string +
                "] does not start with [" + expected_prefix +
                "]: because " + because);
        }
#endif
    }

    void AssertStringLength(const size_t& expected_length, const std::string& actual_string,
        const std::string& because) const
    {
        TAssert::Equal(expected_length, actual_string.length(), because);
    }

    void AssertNotStringLength(const size_t& expected_length, const std::string& actual_string,
        const std::string& because)
    {
        TAssert::NotEqual(expected_length, actual_string.length(), because);
    }

    void AssertStringEmpty(const std::string& actual_string, const std::string& because)
    {
        TAssert::Equal(0, actual_string.length(), because);
    }

    void AssertNotStringEmpty(const std::string& actual_string, const std::string& because)
    {
        TAssert::NotEqual(0, actual_string.length(), because);
    }

    void AssertFail(const std::string& because)
    {
        TAssert::Fail(because);
    }

    template <typename _EXPECTEDEXCEPTION, typename FUNCTOR>
    void Throw(const FUNCTOR func, const std::string& because)
    {
        TAssert::template Throw<_EXPECTEDEXCEPTION>(func, because);
    }

    template <typename NOTEXPECTEDEXCEPTION, typename FUNCTOR>
    void NotThrow(const FUNCTOR func, const std::string& because)
    {
        TAssert::NotThrow<NOTEXPECTEDEXCEPTION>(func, because);
    }

    std::string GetStackMessage(const std::string& file, const int line, const std::string& because)
    {
        return TAssert::GetStackMessage(file, line, because);
    }
};

#if CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_MS
typedef TAssertInternal<MsAssert> AssertInternal;
#elif CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_GTEST
using AssertInternal = TAssertInternal<GTestAssert>;
#elif CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_BOOST
typedef TAssertInternal<BoostTestAssert> AssertInternal;
#elif CHAMOIS_UNITTEST_FRAMEWORK == CHAMOIS_UNITTEST_FRAMEWORK_CATCH2
typedef TAssertInternal<Catch2TestAssert> AssertInternal;
#else
#error No supported test framework found
#endif

#define Be(...) _beFunc(__FILE__, __LINE__, __VA_ARGS__)

/* Default */
template <typename T> class ShouldImpl
{
public:
    explicit ShouldImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBe(...) _notBeFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const T m_value;
    AssertInternal& m_assertObj;
};

template <typename T> class ThatImpl
{
public:
    explicit ThatImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    [[nodiscard]] const T& value() const
    {
        return m_value;
    }

    ShouldImpl<T> Should()
    {
        return ShouldImpl<T>(m_assertObj, m_value);
    }

protected:
    const T m_value;
    AssertInternal& m_assertObj;
};

/* Numeric Types */
template <typename T> class ShouldNumImpl
{
public:
    explicit ShouldNumImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const T& expected_value, const std::string& because = {})
    {
        m_assertObj.AssertEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeZero(...) _beZeroFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beZeroFunc(const std::string& file, const int line, const T& /*expected_value*/,
        const std::string& because = {}) const
    {
        m_assertObj.AssertEqual<T>(0, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BePositive(...) _bePositiveFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _bePositiveFunc(const std::string& file, const int line, const T& /*expected_value*/,
        const std::string& because = {}) const
    {
        m_assertObj.AssertGreaterEqual<T>(0, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeNegative(...) _beNegativeFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beNegativeFunc(const std::string& file, const int line, const T& /*expected_value*/,
        const std::string& because = {}) const
    {
        m_assertObj.AssertLess<T>(0, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeGreaterThan(...) _beGreaterThanFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beGreaterThanFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertGreater<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeGreaterThanOrEqual(...) _beGreaterThanOrEqualFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beGreaterThanOrEqualFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertGreaterEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeLessThan(...) _beLessThanFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beLessThanFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertLess<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeLessThanOrEqual(...) _beLessThanOrEqualFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beLessThanOrEqualFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertLessEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeInRange(...) _beInRangeFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beInRangeFunc(const std::string& file, const int line, const T& min_value, const T& max_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertInRange<T>(m_value, min_value, max_value, m_assertObj.GetStackMessage(file, line, because));
    }

    void _notBeFunc(const std::string& file, const int line, const T& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeZero(...) _notBeZeroFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeZeroFunc(const std::string& file, const int line, const T& /*expected_value*/,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual<T>(0, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeInRange(...) _notBeInRangeFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeInRangeFunc(const std::string& file, const int line, const T& min_value, const T& max_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotInRange<T>(m_value, min_value, max_value,
            m_assertObj.GetStackMessage(file, line, because));
    }

#define BeApproximately(...) _beApproximatelyFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beApproximatelyFunc(const std::string& file, const int line, const T& expected_value,
        const T& tolerance,
        const std::string& because = {}) const
    {
        m_assertObj.AssertBeApproximately<T>(expected_value, m_value, tolerance,
            m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const T m_value;
    AssertInternal& m_assertObj;
};

template <typename T> class ThatNumImpl
{
public:
    explicit ThatNumImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    [[nodiscard]] T value() const
    {
        return m_value;
    }

    [[nodiscard]] ShouldNumImpl<T> Should() const
    {
        return ShouldNumImpl<T>(m_assertObj, m_value);
    }

protected:
    const T m_value;
    AssertInternal& m_assertObj;
};

/* Boolean */
class ShouldBoolImpl
{
public:
    explicit ShouldBoolImpl(AssertInternal& assert_obj, const bool& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const bool& expected_value, const std::string& because = {})
    {
        m_assertObj.AssertEqual(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

    void _notBeFunc(const std::string& file, const int line, const bool& expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeTrue(...) _beTrueFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beTrueFunc(const std::string& file, const int line, const std::string& because = {})
    {
        m_assertObj.AssertTrue(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeFalse(...) _beFalseFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beFalseFunc(const std::string& file, const int line, const std::string& because = {})
    {
        m_assertObj.AssertFalse(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeTrue(...) _notBeTrueFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeTrueFunc(const std::string& file, const int line, const std::string& because = {})
    {
        m_assertObj.AssertFalse(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeFalse(...) _notBeFalseFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeFalseFunc(const std::string& file, const int line, const std::string& because = {})
    {
        m_assertObj.AssertTrue(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const bool& m_value;
    AssertInternal& m_assertObj;
};

class ThatBoolImpl
{
public:
    explicit ThatBoolImpl(AssertInternal& assert_obj, const bool& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    [[nodiscard]] const bool& value() const
    {
        return m_value;
    }

    [[nodiscard]] ShouldBoolImpl Should() const
    {
        return ShouldBoolImpl(m_assertObj, m_value);
    }

protected:
    const bool& m_value;
    AssertInternal& m_assertObj;
};

/* String */
template <class T> class ShouldStrImpl
{
public:
    explicit ShouldStrImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const T& expected_value, const std::string& because = {})
    {
        m_assertObj.AssertEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

    void _notBeFunc(const std::string& file, const int line, const T& expected_value, const std::string& because = {})
    {
        m_assertObj.AssertNotEqual<T>(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define HaveLength(...) _haveLengthFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _haveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {}) const
    {
        m_assertObj.AssertStringLength(expected_length, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotHaveLength(...) _notHaveLengthFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notHaveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotStringLength(expected_length, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeEmpty(...) _beEmptyFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beEmptyFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertStringEmpty(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeEmpty(...) _notBeEmptyFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeEmptyFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertNotStringEmpty(m_value, m_assertObj.GetStackMessage(file, line, because));
    }

    ///
    /// The name mangled below to be "StartWith" and not "StartsWith" (Notice the s)
    /// Is because in Catch2 (Maybe others), there is a "StartsWith" defined function !
#define StartWith(...) _startsWithFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _startsWithFunc(const std::string& file, const int line, const T& expected_prefix,
        const std::string& because = {}) const
    {
        m_assertObj.AssertStringStartsWith(expected_prefix, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const T m_value;
    AssertInternal& m_assertObj;
};

template <class T> class ThatStrImpl
{
public:
    explicit ThatStrImpl(AssertInternal& assert_obj, const T& value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    ShouldStrImpl<T> Should()
    {
        return ShouldStrImpl<T>(m_assertObj, m_value);
    }

protected:
    const T m_value;
    AssertInternal& m_assertObj;
};

/* Pointer */
class ShouldPtrImpl
{
public:
    explicit ShouldPtrImpl(AssertInternal& assert_obj, const void* value)
        : m_value(value), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const void* expected_value, const std::string& because = {})
    {
        m_assertObj.AssertEqual(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

    void _notBeFunc(const std::string& file, const int line, const void* expected_value,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual(expected_value, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define BeNull(...) _beNullFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beNullFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertEqual((void*)nullptr, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeNull(...) _notBeNullFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeNullFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual((void*)nullptr, m_value, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const void* m_value;
    AssertInternal& m_assertObj;
};

class ThatPtrImpl
{
public:
    explicit ThatPtrImpl(const AssertInternal& assert_obj, const void* value) : m_value(value), m_assertObj(assert_obj)
    {
    }

    ShouldPtrImpl Should()
    {
        return ShouldPtrImpl(m_assertObj, m_value);
    }

protected:
    const void* m_value;
    AssertInternal m_assertObj;
};

/* Array */
template <typename T, int N> class ShouldArrImpl
{
public:
    explicit ShouldArrImpl(AssertInternal& assert_obj, const T(&value)[N])
        : length_(N), m_value(value), m_assertObj(assert_obj)
    {
    }

    template <int O>
    void _beFunc(const std::string& file, const int line, const T(&expected_value)[O], const std::string& because = {})
    {
        m_assertObj.AssertArrayEqual(expected_value, O, m_value, length_,
            m_assertObj.GetStackMessage(file, line, because));
    }

    template <int O>
    void _notBeFunc(const std::string& file, const int line, const T(&expected_value)[O], const std::string& because = {})
    {
        m_assertObj.AssertArrayNotEqual<T>(expected_value, O, m_value, length_,
            m_assertObj.GetStackMessage(file, line, because));
    }

#define HaveLength(...) _haveLengthFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _haveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {}) const
    {
        m_assertObj.AssertEqual(expected_length, length_, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotHaveLength(...) _notHaveLengthFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notHaveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {}) const
    {
        m_assertObj.AssertNotEqual(expected_length, length_, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const size_t length_;
    const T(&m_value)[N];
    AssertInternal& m_assertObj;
};

template <typename T, int N> class ThatArrImpl
{
public:
    explicit ThatArrImpl(AssertInternal& assert_obj, const T(&value)[N])
        : m_value(value), m_assertObj(assert_obj)
    {
        length_ = N;
    }

    const T* value() const
    {
        return &m_value;
    }

    [[nodiscard]] size_t length() const
    {
        return length_;
    }

    ShouldArrImpl<T, N> Should()
    {
        return ShouldArrImpl<T, N>(m_assertObj, m_value);
    }

protected:
    const T(&m_value)[N];
    size_t length_;
    AssertInternal& m_assertObj;
};

// teach std::ostringstream how to print std::vector
template <typename T>
inline std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& wrapped, const std::vector<T>& item)
{
    wrapped << '[';
    bool first = true;
    for (auto const& element : item) {
        wrapped << (!first ? "," : "") << element;
        first = false;
    }
    return wrapped << ']';
}
// teach std::ostringstream how to print std::pair
template <typename K, typename V>
std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& wrapped, const std::pair<const K, V>& item)
{
    return wrapped << '<' << item.first << ',' << item.second << '>';
}

template <typename Container> class ShouldContainerImpl
{
public:
    typedef typename Container::value_type m_valuetype;
    using iterator = typename Container::iterator;

    explicit ShouldContainerImpl(AssertInternal& assert_obj, const Container& c)
        : container_(c), m_assertObj(assert_obj)
    {
    }

    void _beFunc(const std::string& file, const int line, const Container& other, const std::string& because = {})
    {
        m_assertObj.AssertEqual(other.size(), container_.size(), because);
        auto left_begin = container_.begin();
        auto left_end = container_.end();
        auto right_begin = other.begin();
        auto right_end = other.end();
        {
            std::size_t pos = 0;

            for (; left_begin != left_end && right_begin != right_end; ++left_begin, ++right_begin, ++pos)
            {
                if (*left_begin != *right_begin)
                {
                    std::ostringstream mess;
                    mess << "Mismatch at position " << std::to_string(pos) << ": "
                        << (*left_begin)
                        << " != "
                        << (*right_begin)
                        << "\n" << because;
                    m_assertObj.AssertFail(m_assertObj.GetStackMessage(file, line, mess.str()));
                }
            }

            if (left_begin != left_end) {
                std::size_t r_size = pos;
                while (left_begin != left_end) {
                    ++pos;
                    ++left_begin;
                }

                std::stringstream mess;
                mess << "Collections size mismatch: " << pos << " != " << r_size << "\n" << because;
                m_assertObj.AssertFail(m_assertObj.GetStackMessage(file, line, mess.str()));
            }

            if (right_begin != right_end) {
                std::size_t l_size = pos;
                while (right_begin != right_end) {
                    ++pos;
                    ++right_begin;
                }
                std::stringstream mess;
                mess << "Collections size mismatch: " << l_size << " != " << pos << "\n" << because;
                m_assertObj.AssertFail(m_assertObj.GetStackMessage(file, line, mess.str()));
            }
        }
        //for (auto actual = container_.begin(), expected = other.begin(); actual != container_.end();
        //    ++actual, ++expected)
        //{
        //    m_assertObj.AssertEqual(*expected, *actual, m_assertObj.GetStackMessage(file, line, because));
        //}
    }

    void _notBeFunc(const std::string& file, const int line, const Container& other, const std::string& because = {})
    {
        if (other.size() != container_.size())
        {
            return;
        }

        for (auto actual = container_.begin(), expected = other.begin();
            actual != container_.end() && expected != other.end(); ++actual, ++expected)
        {
            if (*actual != *expected)
            {
                return;
            }
        }

        m_assertObj.AssertFail(m_assertObj.GetStackMessage(file, line, because));
    }

    void _haveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {})
    {
        m_assertObj.AssertEqual(expected_length, container_.size(), m_assertObj.GetStackMessage(file, line, because));
    }

    void _notHaveLengthFunc(const std::string& file, const int line, const size_t expected_length,
        const std::string& because = {})
    {
        m_assertObj.AssertNotEqual(expected_length, container_.size(),
            m_assertObj.GetStackMessage(file, line, because));
    }

#define BeEmpty(...) _beEmptyFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _beEmptyFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertTrue(container_.empty(), m_assertObj.GetStackMessage(file, line, because));
    }

#define NotBeEmpty(...) _notBeEmptyFunc(__FILE__, __LINE__, __VA_ARGS__)
    void _notBeEmptyFunc(const std::string& file, const int line, const std::string& because = {}) const
    {
        m_assertObj.AssertFalse(container_.empty(), m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const Container& container_;
    AssertInternal& m_assertObj;
};

template <typename Container> class ThatContainerImpl
{
public:
    explicit ThatContainerImpl(AssertInternal& assert_obj, const Container& c)
        : container_(c), m_assertObj(assert_obj)
    {
    }

    ShouldContainerImpl<Container> Should()
    {
        return ShouldContainerImpl<Container>(m_assertObj, container_);
    }

private:
    const Container& container_;
    AssertInternal& m_assertObj;
};

template <typename FUNCTOR> class ShouldExceptionImpl
{
public:
    explicit ShouldExceptionImpl(AssertInternal& assert_obj, FUNCTOR func)
        : m_func(func), m_assertObj(assert_obj)
    {
    }

#define Throw(expected_exception, ...) _throw<expected_exception>(__FILE__, __LINE__, __VA_ARGS__)
    template<typename _EXPECTEDEXCEPTION>
    void _throw(const std::string& file, const int line, const std::string& because = {})
    {
        static_assert(std::is_base_of_v<std::exception, _EXPECTEDEXCEPTION>, "ExceptionType must be derived from std::exception");
        m_assertObj.Throw< _EXPECTEDEXCEPTION, FUNCTOR>(m_func, m_assertObj.GetStackMessage(file, line, because));
    }

#define NotThrow(expected_exception, ...) _notThrow<expected_exception>(__FILE__, __LINE__, __VA_ARGS__)
    template<typename NOTEXPECTEDEXCEPTION>
    void _notThrow(const std::string& file, const int line, const std::string& because = {})
    {
        static_assert(std::is_base_of_v<std::exception, NOTEXPECTEDEXCEPTION>, "ExceptionType must be derived from std::exception");
        m_assertObj.NotThrow< NOTEXPECTEDEXCEPTION, FUNCTOR>(m_func, m_assertObj.GetStackMessage(file, line, because));
    }

private:
    const FUNCTOR m_func;
    AssertInternal& m_assertObj;
};

template <typename FUNCTOR> class ExceptionImpl
{
public:
    explicit ExceptionImpl(AssertInternal& assert_obj, FUNCTOR func)
        : m_func(func), m_assertObj(assert_obj)
    {
    }

    ShouldExceptionImpl<FUNCTOR> Should()
    {
        return ShouldExceptionImpl<FUNCTOR>(m_assertObj, m_func);
    }

private:
    const FUNCTOR m_func;
    AssertInternal& m_assertObj;
};
}    // namespace detail

template <class TAssertImpl> class BasicAssert
{
public:
    static void Message(const std::string& value)
    {
        //detail::MessageImpl( value);
        std::cerr << "[ Message  ] " << value << std::endl;
    }

    /* Default */
    template <typename T> static detail::ThatImpl<T> That(const T& value)
    {
        auto var = TAssertImpl();
        return detail::ThatImpl<T>(var, value);
    }

    /* Numeric */
    static detail::ThatNumImpl<std::int8_t> That(const std::int8_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::int8_t>(var, value);
    }
    static detail::ThatNumImpl<std::uint8_t> That(const std::uint8_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::uint8_t>(var, value);
    }

    static detail::ThatNumImpl<std::int16_t> That(const std::int16_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::int16_t>(var, value);
    }

    static detail::ThatNumImpl<std::uint16_t> That(const std::uint16_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::uint16_t>(var, value);
    }

    static detail::ThatNumImpl<std::int32_t> That(const std::int32_t value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::int32_t>(var, value);
    }

    static detail::ThatNumImpl<std::uint32_t> That(const std::uint32_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::uint32_t>(var, value);
    }

    static detail::ThatNumImpl<long> That(const long& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<long>(var, value);
    }

    static detail::ThatNumImpl<unsigned long> That(const unsigned long& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<unsigned long>(var, value);
    }

    static detail::ThatNumImpl<std::int64_t> That(const std::int64_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::int64_t>(var, value);
    }
    static detail::ThatNumImpl<std::uint64_t> That(const std::uint64_t& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<std::uint64_t>(var, value);
    }

    static detail::ThatNumImpl<float> That(const float& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<float>(var, value);
    }

    static detail::ThatNumImpl<double> That(const double& value)
    {
        auto var = TAssertImpl();
        return detail::ThatNumImpl<double>(var, value);
    }

    /* Boolean */
    static detail::ThatBoolImpl That(const bool& value)
    {
        auto var = TAssertImpl();
        return detail::ThatBoolImpl(var, value);
    }

    /* Array */
    template <typename T, int N> static detail::ThatArrImpl<T, N> ThatArray(const T(&value)[N])
    {
        auto var = TAssertImpl();
        return detail::ThatArrImpl<T, N>(var, value);
    }

    /* String */
    static detail::ThatStrImpl<std::wstring> That(const std::wstring& value)
    {
        auto var = TAssertImpl();
        return detail::ThatStrImpl<std::wstring>(var, value);
    }

    static detail::ThatStrImpl<std::string> That(const std::string& value)
    {
        auto var = TAssertImpl();
        return detail::ThatStrImpl<std::string>(var, value);
    }

    static detail::ThatStrImpl<std::wstring> That(const wchar_t* value)
    {
        return That(std::wstring(value));
    }

    static detail::ThatStrImpl<std::string> That(const char* value)
    {
        return That(std::string(value));
    }

    /* Pointer */

    static detail::ThatPtrImpl ThatPtr(const void* value)
    {
        auto var = TAssertImpl();
        return detail::ThatPtrImpl(var, value);
    }

    template <typename T> static detail::ThatPtrImpl ThatPtr(const std::unique_ptr<T>& value)
    {
        auto var = TAssertImpl();
        return detail::ThatPtrImpl(var, value.get());
    }

    template <typename T> static detail::ThatPtrImpl ThatPtr(const std::shared_ptr<T>& value)
    {
        auto var = TAssertImpl();
        return detail::ThatPtrImpl(var, value.get());
    }

    template <typename T> static detail::ThatContainerImpl<T> ThatContainer(const T& container)
    {
        auto var = TAssertImpl();
        return detail::ThatContainerImpl<T>(var, container);
    }


    /* Char Array */
    /*
    template<int N>
    static detail::ThatPtrImpl<char> That(const char (&value)[N])
    {
        return detail::ThatPtrImpl<char>(var, value);
    }

    template<int N>
    static detail::ThatPtrImpl<char> That(const char (&value)[N])
    {
        return detail::ThatPtrImpl<char>(var, value);
    }
    */


    /* cannot do the following due to:  // NOLINT(clang-diagnostic-invalid-utf8)
     * The type-of a lambda expression in C++ is an unnamed closure type.
     * The closure type is generated by the compiler based on the structure and behavior of the lambda expression.
     * Or, put another way:
     * The type-of a lambda is implementation defined, meaning that from C++ it doesn't actually have a type.
     *
    static detail::ExceptionImpl<void()> That(std::function<void()> func)
    {
        auto var = TAssertImpl();
        return detail::ExceptionImpl(var, func);
    }
    */

    static detail::ExceptionImpl<void (*)()> That(void (*func)() )
    {
        auto var = TAssertImpl();
        return detail::ExceptionImpl(var, func);
    }

    template<typename FUNCTOR>
    static detail::ExceptionImpl< FUNCTOR> ThatLambda(FUNCTOR func)
    {
        auto var = TAssertImpl();
        return detail::ExceptionImpl(var, func);
    }

};

using Assert = BasicAssert<detail::AssertInternal>;
}    //namespace evolutional
