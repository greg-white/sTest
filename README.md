# sTest - The C++ unit testing framework.

![](https://img.shields.io/badge/version-1.1-blue.svg)
![](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
![](https://img.shields.io/badge/C%2B%2B-11%2F14%2F17-blue.svg)
![](https://img.shields.io/badge/license-MIT-blue.svg)
---

### Goals:

* simple to use and write tests quickly
* log and locate failed tests, print overall testing summary
* easy to change output, modify log format, adapt and extend to own needs


### Features:
	
* lightweight (one header file, uses standard library)
* information about tests: file name, line number and what was tested/failed
* counting, grouping and merging tests
* no need to: create objects, use scope, catch or throw exceptions
* simple syntax, all commands started with `TEST_`
* allows write tests in natural way, no need to build special constructions 
* implementation has separated logic from printing logs (allows to change output easily)
* output to console by default

### Requiements:

* C++11 (templates and \_\_func\_\_ macro)


### Example:

```
#include "test.h"

void test_sub()
{
    TEST_GROUP_FUNCTION;
    TEST(1 - 1 == 1);
    TEST(1 - 2 == -1);
}

int main()
{
    try
    {
    	TEST_SECTION("arithmetic");
	
        TEST_GROUP("test_add");
        TEST(1 + 1 == 2);
        TEST(1 + 2 == 3);

        test_sub();

        TEST_GROUP("test_mul");
	TEST(0 * 1 == 0);
	
        TEST_MERGE(true);
        TEST(1 * 2 == 2);
        TEST(2 * 1 == 1);

        TEST_GROUP("other");
        if (TEST_IF(1 * 1 == 1))
            TEST(2 / (1 * 1) == 2);

        TEST_SUMMARY;
    }
    catch (...)
    {
        TEST_EXCEPTION;
    }
    return 0;
}
```


# COMMAND LIST

## Tests

### `TEST(X)`

`X` - test expression

This is main and basic testing command. It defines a test, converts `X` to `bool` ans checks if is `true`. When test fails it prints to the output: file name, line number and `X` as string. Passed and failed tests are also counted.

Example:

```
TEST(1 + 1 == 2);
```
&nbsp;

### `TEST_IF(X)`

`X` - test expression

This testing command is recommended to be used in the `if` statement (when you have dependent tests). The only diference from `TEST(X)` is that, if this fails it will be noticed in summary, that some tests may be skipped.


Example:

```
if (TEST_IF(file = CreateTempFile()))
    TEST(DeleteFile(file));
```
&nbsp;


### `TEST_R(X)`

`X` - test expression

You can use this command when you do not want to execute next tests in current function, if this fails. The diference from `TEST(X)` is that, if this fails it returns form current function and it will be noticed in summary, that some tests may be skipped.


Example:

```
void test()
{
    TEST_R(ImportData()); // return if fails
    TEST(ProcessData());
}
```
&nbsp;

### `TEST_MERGE(X)`

`X` - `true` or `false`

Merge all tests from this command to the next `TEST_MERGE(X)` or to the end of test group into one test. Merged tests are counted as one, no matter how many of then will pass or fail.


Example:

```
// one split case
TEST_MERGE(true);
auto vct = Split("a:b", ":");
if (TEST_IF(vct.size() == 2))
{
    TEST(vct[0] == "a");
    TEST(vct[1] == "b");
}

TEST_MERGE(...
```
&nbsp;

## Test grouping

You can optionally divide your tests into gropups with own summaries. You can also split these groups into sections.

### `TEST_GROUP(X)`

`X` - group name

Create a test group named `X`. It also prints to the output summary of previous group (all and failed test count) and the name of the current group.

* Group ends when a new one begins, a section begins or program reach test summary commands.
* Group summary is printed at the end of the group.
* Groups can have same names.
* There can be many groups in one scope.
* When there are tests but no group is curently defined, test will be assigned to an unnamed group.

Example:
```
TEST_GROUP("test_add");
TEST(1 + 1 == 2);
TEST(2 + 2 == 4);
```
&nbsp;


### `TEST_GROUP_FUNCTION`

Define new test group with name of current function.

Example:
```
void test_sub()
{
    TEST_GROUP_FUNCTION; // same as TEST_GROUP("test_sub");
    TEST(1 - 1 == 0);
    TEST(2 - 1 == 1);
}
```
&nbsp;


### `TEST_SECTION(X)`

`X` - section name

Create a test section named `X`. Sections are used to split test groups.

* Section definition ends current group and prints its summary.
* Section has not own summary (only name of the section will be printed).
* Sections can have same names.
* There can be many sections in one scope.

Example:
```
TEST_SECTION("arithmetic");

TEST_GROUP("test_add");
TEST(1 + 1 == 2);
TEST(1 + 2 == 3);

test_sub();		// defines a test group

TEST_SECTION("slist");
test_push_pop();	// defines a test group
test_reverse();		// defines a test group

```
&nbsp;


### `TEST_SECTION_FUNCTION`

Define new test section with name of current function.

Example:
```
void test_string()
{
    TEST_SECTION_FUNCTION; // same as TEST_SECTION("test_string");
    test_trim();
    test_split();    
}
```
&nbsp;


## Summary

### `TEST_SUMMARY`

Use at the end of `try` block that contain your tests. You will get here overall testing summary with total and failed test count.

This command is required.

Example:
```
try
{
    // put your tests here     
    TEST_SUMMARY;
}
catch(...)
{
...
```
&nbsp;


### `TEST_EXCEPTION`

Use in `catch` block atfer your tests, to get informed if they were interrupted by an uncaught exception. You will get file name and line number of the last test before an exception was thrown.

This command is required.

Example:
```
try
{	
    //put you tests and TEST_SUMMARY here
}
catch(...)
{
    TEST_EXCEPTION;
}
```
&nbsp;


In `TEST_SUMMARY` and `TEST_EXCEPTION` commands all test counts (total, failed and group) will be reset.

&nbsp;


## Aditional commands

### `TEST_PRINT(X)`

`X` - message to be printed

Print to the output a custom message.

Example:
```
TEST_PRINT(" [preparing data]");
```
&nbsp;


### `TEST_FAILED`

Check if anny of the tests has failed. Use before `TEST_SUMMARY`.

Example:
```
if(TEST_FAILED)
```
```
return TEST_FAILED;
```
&nbsp;




## Options

### `TEST_EXIT(X)`

`X` - `true` or `false`

Enable or disable program exit in `TEST_SUMMARY` and `TEST_EXCEPTION`. When is enabled and there was a failed test or an exception `exit(EXIT_FAILURE)` will be called. 

By default is set to `true`.

Example:
```
TEST_EXIT(false);
```
&nbsp;


### `TEST_WAIT(X)`

`X` - `true` or `false`

Enable or disable wait for user imput (Enter key) before exit - when `TEST_EXIT(true)` is set.

By default is set to `true`

Example:
```
TEST_WAIT(false);
```
&nbsp;

# Minimal setup

Console application:

```cpp
#include "test.h"

int main()
{
    try
    {
        // put your tests here
        TEST_SUMMARY;
    }
    catch (...)
    {
        TEST_EXCEPTION;
    }
    return 0;
}
```
&nbsp;


# License

**sTest** is licensed under the MIT License, see LICENSE for more information.

&nbsp;




