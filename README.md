Chamois - Fluent Assertions for C++
===================================

This vbersion of the "Chamois fluent assertions for C++" is a header-only file for C++20 which is designed to facilitate TDD and BDD through the provision of a natural language syntax. Chamois is inspired by the brilliant [Fluent Assertions] for .NET.

The primary way of expressing a test in Chamois is in the form:

```cpp
Assert::That(<actual_value>).Should().Be(<expected_value>);
Assert::That(<actual_value>).Should().NotBe(<expected_value>);
```


Examples
---

Verify that an integer matches the expected value:

```cpp
Assert::That(10).Should().Be(10, "10 is equal to 10");
```

Verify that two arrays are the same:

```cpp
int A[5] = { 1, 2, 3, 4, 5 };
int B[5] = { 1, 2, 3, 4, 5 };
Assert::ThatArray(A).Should().Be(B, "arrays whould be equal");
```

Verify that two strings are equal:

```cpp
Assert::That("hello").Should().Be("hello", "strings should be equal");
```

Or that a string starts with a prefix:

```cpp
Assert::That("hello").Should().StartWith("he", "string should start with he");
```

Verify a boolean is true:

```cpp
Assert::That(true).Should().BeTrue( "true is true");
```

Currently Supported Types
---

* Integral numerics (char, short, int, long, float, double)
* String (via std::string/std::wstring, including const char*/const wchar_t*)
* Arrays (simple arrays)
* Pointers (naked pointers only)
* Any object by reference that supports the equality operator


Supported Test Frameworks
---

* Microsoft's C++ Unit Test Framework
* Google Test (GTest)
* Boost Unit Test Framework (UTF)
* Catch2
* Others may be added in the future

Usage
---

In the majority of cases, you include your test framework header first and then Chamois.hpp. Generally, you should ensure that your types are comparable (== and !=)

**Microsoft C++ Unit Test**

Occasionally, you may get compilation errors in your tests; more often than not this is due to the MS framework not knowing how to compare two values; this is due to the framework converting your type to a string for comparisions.
 To achieve this, you must provide a template specialization for the Microsoft::VisualStudio::CppUnitTestFramework::ToString method. 

**Boost Unit Test Framework**

Ensure that you have included the unit_test.hpp file and defined BOOST_TEST_MODULE before including the Chamois.hpp header.

There are a few limitations in the Boost framework support at present. As a result, the Asserts fall back to a simple BOOST_CHECK( a == b ) and we lose the ability to see the expected and actual values.

# Modifications by "Smurf-IV"
[Fluent Assertions]:https://github.com/Smurf-IV/FluentAssertions-for-Cpp
## Breaking Changes
- No need to specify the `because` clauses to be wide string at any time.
  - i.e. the following 
    - `Assert::That(10).Should().Be(10, "10 is equal to 10");`
  - the `because` optional string is now using the `const std::string&` usage, to simplify memory being passed around the system.

## Done
- Change solution to be VS 2022 and VC-Runtime 14.38 (Can still be used in C++11 and above)
- Introduce Location of actual failure in GoogleTest
- "Location of actual failure" in MsTest is done via both string and stack trace
- Complete move to Std C++20 for All testing frameworks
  - `MsTest` now compiles / runs under `Std C++ 20`
  - `Boost.Test` now compiles / runs under `Std C++ 20`
  - `Google.Test` now compiles / runs under `Std C++ 20`
- [Remove the `Chamois` namespace](https://github.com/Smurf-IV/FluentAssertions-for-Cpp/issues/2)
- Changed to use explicit `std::` sized types; e.g. `std::uint16_t` instead of `short`
- Use the Windows `__LineInfo(const wchar_t* pszFileName, const char* pszFuncName, int lineNumber)`for the stack traces "As needed"

## Continued additions of
- `GreaterEqual(min_value, actual_value, because);`
- `LessEqual(max_value, actual_value, because);`
- `LessThan(min_value, actual_value, because);`
- `GreaterThan(max_value, actual_value, because);`
- Add `std::make_unique` to the list of "acceptable" pointer types
- Add `BeApproximately`
- Add `HaveLength`
- Add `NotHaveLength`
- Add `Should().Throw` for expected exception
- Add `Should().NotThrow` for expected exception

## TBD
- More assertion types that follow the `.Net FluentAssertions` patterns
- Add tests in each framework for the above
- Fix BDD (I do not use it, so no changes have been done in this area)


## BDD
__Note: This needs to be fixed (See below)__

Chamois includes a simple fluent BDD wrapper, allowing tests to be expressed in the "Given, When, Then" form using C++11's lambda functionality.

For example, testing a class behaviour directly:

```cpp
DummyBank bank;

bdd::Given::That([&]() { bank.balance_set(100); })
	.When([&]() { bank.Withdraw(80); })
	.Then([&]() { Assert::That(bank.balance()).Should().Be(20); });
```

Or creating a class which performs the test actions:

```cpp
DummyBankTest test;

bdd::Given::That([&]() { test.The_starting_balance_is_100(); })
	.When([&]() { test.I_withdraw_80(); })
	.Then([&]() { test.The_balance_should_be_20(); });
```


