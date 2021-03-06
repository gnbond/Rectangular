# Rectangular

A simple 2-dimensional container template for C++.

This is provided in two mostly-compatible versions:
 - In this directory, a version written in modern style that supports C++11 and up
 - In the c++03 directory, a version that supports C++98 and C++03 using more traditional style

This class template wraps a `std::vector<>` to provide a simple, lightweight 2-dimensional dynamic array container that behaves well with other standard library features.  It is implemented as 1 small header file, with a very forgiving licence, that can be just copied into any project, free or commercial.  I needed this for a little project I was working on, and broke it out into a separate project here so it can be simply re-used.

By wrapping `std::vector<>`, a lot of standard library machinery comes for free.  Iterators are provided that iterate over the (one-dimensional) underlying data in [row-major order](https://en.wikipedia.org/wiki/Row-_and_column-major_order).  These are just forwards to the underlying `vector<>`.  Move semantics and swap are supported, again by leveraging the underlying `vector<>`.

In terms of abstraction, `rectangular` is somewhere between `std::array<>` and `std::vector<>`:
 - unlike `std::array<>`, dimensions can be set at run-time and it can be resized
 - unlike `std::vector<>` the only shape-changing operation is `resize()`.  
 - Front or back  (accessors such as `front()` or modifiers such as `emplace_back()`) make no sense on a 2-D array unless you wish to access/add a whole row/column (which is an operation much too complicated for this class).
 - As with both, `operator[]` is not bounds-checked but `.at(y,x)` may throw `std::out_of_range()`
    - but see `checked_rectangular<>` below

If you have already partaken of the Boost kool-aid, then [Boost.MultiArray](https://www.boost.org/doc/libs/1_73_0/libs/multi_array/doc/user.html) could solve the same problem.   MultiArray is much more general (N-dimensional arrays) and has many more features, but is much heavier weight and (IMO) much less intuitive to use.  On the other hand, if you already use Boost, then MultiArray is well documented, peer-reviewed, and tried and tested.

Bugs/Comments/Pull requests to https://github.com/gnbond/Rectangular

## What this is not

This is not a linear algebra package, and not intended for matrix arithmetic.  Use something like [Eigen](https://gitlab.com/libeigen/eigen) or see the comparison listing at [Wikipedia](https://en.wikipedia.org/wiki/Comparison_of_linear_algebra_libraries) for actual matrix libraries.

It's not a big data library like `NumPy`, again there are C++ alternatives (or just use NumPy, not everything has to be in C++).

I/O is not provided, that's a whole nother can of worms.  Use standard algorithms and the content iterators.

Iterating over rows or columns is not provided, but could be fairly easily added with proxy classes similar to `checked_rectangular`.  But if you need this sort of capability, again look at a proper linear algebra library.

## What's in a name?

Standard library type names are `lower_case_underscore_separated` so this should follow that convention. Names must start with a letter so something like `2d_array` is out, and `array_2d` looks awkward. Both (by similarity to `std::array<>`) imply compile-time fixed size.  `matrix` implies linear algebra, and this is not an arithmetic type. `rectangle` sounds more like a shape from an introductory OO tutorial than a container.  So I settled on `gnb::rectangular`, in the `gnb::` namespace because library bits should not be in global namespace.

## A note on co-ordinate systems

The intent is to mirror C/C++ built-in multi-dimensional arrays.  So data is stored in row-major order, with the first index being row number (y value running from 0 .. height()-1) and the second index being column number (x value running from 0 .. width()-1).   As with `vector<>` and `array<>`, but unlike built-in arrays, all indexes are *unsigned* (actually, `vector<>::size_type`) so `r[-1][0]` will likely coredump, not access the first element of the last row.  Sorry, all you Python programmers.

You can visualise this as an image with the following layout, x increasing across the image and y increasing down the image:

    r[0][0]        .. r[0][x]        .. r[0][width-1]
    .                 .                 .
    r[y][0]        .. r[y][x]        .. r[y][width-1]
    .                 .                 .
    r[height-1][0] .. r[height-1][x] .. r[height-1][width-1]

## Invariants and exceptions: To Throw or not to Throw?

In a 1-D `vector<>` it is simple to add or remove elements at will without affecting the integrity of the container.  For a 2-D container, it is important to maintain the invariant that `size() == height() * width()`.  This is why `append()` or `pop_front()` are not supported operations.  The only way to change the size of a `rectangular` is via `.resize()`, which has well-defined semantics regarding added or superfluous contents and will maintain the invariant.

The constructors that use a range to specify the initial contents (iterator or initializer_list) must also maintain the invariant.  There are three possible approaches in these cases:
 1. Require the user to specify `height` and `width` explicitly, and insist on exactly `height x width` elements in the initializer.
 2. Require the user to specify `height` and `width` explicitly, and handle missing / excess elements in the initializer by use of default value if missing or ignoring any excess values.
 3. Require the user to specify the width only, and calculate the height based on the length of the initializer.  This is how native C/C++ 2-D arrays work.  In the case where the number of initializers is not an exact multiple of the width, this can either throw or use default values to make up to a full row.

 The `rectangular` class takes the first approach - insist on exactly the right number of initializer elements.  This may be a bit less flexible, but will catch a class of errors that other approaches may paper over.  If client code knows there is the possibility that the initializer length is not correct, then this can be explicitly coded for, something like this:
 ```C++
    rectangular<int> r{3, 3, 0};
    std::copy_n(source.begin(), std::min(source.size(), r.size()), r.begin());
```

## C++ compatibility

The main version of templates work with C++11, C++14 and C++17.  This has been tested on Clang (versions 6, 9 & 10) and g++ (versions 5.4, 8.4 and 9.3) on a variety of hosts (though host OS should really not matter).

For `checked_rectangular` we would like to prohibit any use of the RowProxy object other than immediate dereference via `[]`.  Mostly, this works as written for C++11 and C++14, but for C++17 the following will actually compile, even though it is an error on C++14.  I think this is because of the changed Copy Elision requirements, see [cppreference.com](https://en.cppreference.com/w/cpp/language/copy_elision):

    gnb::checked_rectangular<int> cr{3,3};
    auto row{cr[1]};

This causes a false-positive in the `test_nc_checked_proxy.cpp` test, so that test has to be disabled for C++17. Alas, XCode 10 clang supports enough of C++17 to make this compile, but not enough (missing the `__cpp_guaranteed_copy_elision` macro) to work around this problem in the official manner, so the unit test has some Mac-specific knowledge. 

There is a somewhat simpler (and wordier, and uglier) version in the `c++03` directory that should work with C++98 and C++03, for those stuck on old compilers. This has been tested with Clang (versions 6 and 9) and GCC (version 8.4).

## Building 

This is a single header file containing class templates, no code to be compiled and no binary to be distributed.  Just copy `rectangular.hpp` into your project and `#include` it as necessary.  There are no dependencies outside the standard library.


## Unit testing

### C++11 version

Unit tests are in the `tests` directory.  Tests are written using [Catch2](http://catch-lib.net/) test framework, which is a single (large!! 650k) header file.  The `catch.hpp` file is included directly in this project (and makes up 98% of the linecount).  One downside is that the `test_main.o` file can take a long time to compile (500Mb process size, 30-60 CPU seconds), so take care if you are running the tests inside a smallish Docker container.

Filenames starting with `test_` are the unit tests. Filenames starting with `test_nc_` are code snippets that should not compile.  A Makefile is included, and is needed only for the unit tests.   `make check` will compile and run the unit tests and confirm the `test_nc` code does not compile.  Hint: when porting to a new compiler, it's worth manually checking that the `test_nc_` tests fail for the reason expected, not because of some other unexpected system dependency!

### C++03 version

The C++03 version cannot be tested with Catch2, as Catch2 only supports C++11.  So the `c++03` directory contains some simple macros for unit testing and a distinct set of test cases for the C++03 version of `rectangular.hpp`. `make check` works there as well.

Interesting note:  `decltype()` is a C++11 feature, but XCode 10 clang (purported to be clang 6) accepts it even in C++03 mode, without even a compatibility warning.  Clang 6 (& gcc) in C++03 mode on linux correctly flag this as an error.

## Test Results

### C++11 version

System | Compiler | C++ standard | Result 
------ | -------- | ------------ | ------
MacOs 10.13| XCode 10.0 (Clang 6) | -std=c++11 | Pass
MacOs 10.13| XCode 10.0 (Clang 6) | -std=c++14 | Pass
MacOs 10.13| XCode 10.0 (Clang 6) | -std=c++17 | Pass
FreeBSD 10.4 | Clang 3.4 | -std=c++11 | Pass
FreeBSD 11.4 | Clang 10.0 | -std=c++11 | Pass
FreeBSD 11.4 | Clang 10.0 | -std=c++14 | Pass
FreeBSD 11.4 | Clang 10.0 | -std=c++17 | Pass
FreeBSD 11.4 | g++ 9.3 | -std=c++11 | Pass
FreeBSD 11.4 | g++ 9.3 | -std=c++14 | Pass
FreeBSD 11.4 | g++ 9.3 | -std=c++17 | Pass
Ubuntu 16 | g++ 5.4 | -std=c++11 | Pass
Ubuntu 16 | g++ 5.4 | -std=c++14 | Pass
Ubuntu 16 | g++ 5.4 | -std=c++17 | Pass
GitHub CI / Ubuntu 18 | g++ 8.4 | -std=c++11| ![GCC / C++11](https://github.com/gnbond/Rectangular/workflows/GCC%20/%20C++11/badge.svg)
GitHub CI / Ubuntu 18 | g++ 8.4 | -std=c++14| ![GCC / C++14](https://github.com/gnbond/Rectangular/workflows/GCC%20/%20C++14/badge.svg)
GitHub CI / Ubuntu 18 | g++ 8.4 | -std=c++17| ![GCC / C++17](https://github.com/gnbond/Rectangular/workflows/GCC%20/%20C++17/badge.svg)
GitHub CI / Ubuntu 18 | clang 9.0 | -std=c++11| ![Clang / C++11](https://github.com/gnbond/Rectangular/workflows/Clang%20/%20C++11/badge.svg)
GitHub CI / Ubuntu 18 | clang 9.0 | -std=c++14| ![Clang / C++14](https://github.com/gnbond/Rectangular/workflows/Clang%20/%20C++14/badge.svg)
GitHub CI / Ubuntu 18 | clang 9.0 | -std=c++17| ![Clang / C++17](https://github.com/gnbond/Rectangular/workflows/Clang%20/%20C++17/badge.svg)

### C++03 version
System | Compiler | C++ standard | Result 
------ | -------- | ------------ | ------
MacOs 10.13| XCode 10.0 (Clang 6) | -std=c++98 | Pass
MacOs 10.13| XCode 10.0 (Clang 6) | -std=c++03 | Pass
GitHub CI / Ubuntu 18 | g++ 8.4 | -std=c++03| ![GCC / C++03](https://github.com/gnbond/Rectangular/workflows/GCC%20/%20C++03/badge.svg)
GitHub CI / Ubuntu 18 | clang 9.0 | -std=c++03| ![Clang / C++03](https://github.com/gnbond/Rectangular/workflows/Clang%20/%20C++03/badge.svg)


# Class Documentation

## `rectangular`

### Synopsis
```C++
    template <typename T, typename Allocator = std::allocator<T> >
    class rectangular {
        rectangular();
        explicit rectangular(size_t height, size_t width, T value = T());
        template <typename Iter> explicit rectangular(size_t height, size_t width, Iter begin, Iter end); // may throw
        explicit rectangular(size_t height, size_t width, std::initializer_list<T> il); // may throw
        rectangular(size_type height, size_type width, std::vector<value_type>& vec); // may throw

        // Destructor, copy & move constructors and assignment operators are default

        // Iterators
        iterator begin();
        iterator end();
        const_iterator cbegin();
        const_iterator cend();

        // Mutators
        void fill(const T&);
        void resize(size_t new_height, size_t new_width, const T& new_value = T());

        // Accessors
        T* operator[](size_t y);
        T& at(size_t y, size_t x); // may throw

    }
```

### Constructors

`rectangular()`
 - default constructor, height() == width() == size() = 0

`explicit rectangular(size_t height, size_t width, T value = T())`
  - Create with the given size, and initialise with copies of the given value (default T() i.e. 0 for numeric types)

`template <typename Iter> explicit rectangular(size_t height, size_t width, Iter begin, Iter end)`
  - May be constructed from an iterator range.  Will throw `std::out_of_range` if there are not exactly (height*width) entries in the range. 

`explicit rectangular(size_t height, size_t width, std::initializer_list<T> il)`
 - May be constructed from an initializer list. Will throw `std::out_of_range` if there are not exactly (height*width) entries in the initialzer_list.  Usage like this:
 ```
    rectangular<int> R{2, 2, {5, 6, 7, 8}};
 ``` 
`rectangular(size_type height, size_type width, std::vector<value_type>& vec)`
 - Effciently create a `rectagular` by moving the contents of the given `vector` into the newly-constructed `rectangular`.  Contents of the vector are *swapped* with a default-constructed vector, so `vec` is left empty. If the vector does not have exactly (height * width) elements, throw `std::out_of_range`; source vector is untouched in this case.
 - `value_type` must match between the `rectangular` and the `vector`, else the swap will not compile.
 - Usage:
```C++
    std::vector<int> vec();
    // fill the vector somehow with exactly 9 entries
    rectangular<int> R{3,3, vec};
    // now: vec.size() == 0
```
 - NB this alters the vector, If this is not desired, or the value_types do not match, use the iterator range constructor instead:
```C++
    rectangular<int> R{3,3, vec.begin(), vec.end());
```

### Iterators

Iterators are `RandomAccessIterators`, forwarded from `std::vector<>`.  Both const and non-const iterators are provided.  Reverse iterators are not currently provided (no obvious use-case), but easily added as forward to the underlying `std::vector<>`.

The intent of exposing the iterators is to allow use of `std::copy()` and other algorithms to operate on the data in bulk. The iterators traverse the underlying data (as a one-dimensional array) in **row-major** order.  I.e. `begin()` points to `r[0][0]`, `begin()+1` points to `r[0][1]` and so on.

Iterators are possibly invalidated by `resize()` operation, see below.

### Mutators

`fill(T value)`
 - Set every element to the given value

`resize(size_t new_height, size_t new_width, T value = T() )`
 - Change the size and/or shape of the `rectangular` in-place.  
 - If new size() (i.e. new_height * new_width) is less than existing size(), excess elements are destroyed at the back (i.e. starting with `r[height()-1][width()-1]`).  
 - If new size() is greater than existing size(), then new elements are constructed at the back (as if by `std::vector::emplace_back()`) as copies of the given value.
    - In both these cases, contents may be copied/moved; `end()` iterator will certainly be invalidated and other iterators may also be invalidated.
 - If new size() equals old size(), no contents are created or destroyed, all existing data is retained but the "shape" of the `rectangular` changes.  No data is copied/moved and this runs in constant time.  Iterators are not invalidated in this case.

### Accessors

Const and non-const accessors are provided, accessors of const `rectangular` objects return const pointers/references and cannot be used to change the contained data.

`T* operator[](size_t y)`
 - Return a pointer to the start of the y'th row.  This allows contents to be accessed via the familiar idiom `r[y][x]`. These accesses are (like `std::vector<>`) **NOT** bounds-checked, so use with caution.  The returned pointer may be invalidated by `resize()`.

`T& at(size_t y, size_t x)`
 - Bounds-checked access, throws `std::out_of_range` if y >= height() or x >= width().

## `checked_rectangular`

A `checked_rectangular` IS-A `rectangular` and they can be used interchangably.  `checked_rectangular` overrides the `operator[]()` to return a proxy object so that accesses written as `r[y][x]` will also be bounds-checked and throw `std::out_of_range` if required.

The constructors of `rectangular` are inherited, and the default destructor and copy/move constructor/assignment are used.   All other public members are inherited directly from `rectangular`.

### Synopsis
```C++
    template <typename T, typename Allocator = std::allocator<T> >
    class checked_rectangular : public rectangular<T, Allocator> {

    public:
        // Inherit the base class constructors
        using rectangular<T, Allocator>::rectangular;

        // Destructor, copy & move constructors and assignment operators are default

        // Accessor
        Proxy operator[](size_t y);
    };
```

### Accessor

The `operator[]()` returns an opaque proxy object.  The only supported operation on this proxy object is to call its own `operator[]()`.  This function in turn calls `rectangular.at()` to access the underlying data, which may throw `std::out_of_range`. This allows a the familiar idiom `r[y][x]` but still offers strong bounds checking. 

Note that the exception is thrown from the proxy object, so `r[MAX_INT]` will create a proxy and not throw until the proxy `operator[]()` is called and bounds are checked.

Proxy objects should never be stored or used for an purpose other than immediately applying `[]` to the returned temporary.  The implementation attempts to ensure this by marking the constructors and assignment operators of the proxy class as deleted, but this protection is not perfect.

Both const and non-const proxy objects are provided, and underlying data of `const checked_rectangular` objects is safe from modification.

## C++03 version

The interface for the C++03 version is more or less the same.  Notable differences are:
- C++03 does not support `std::initializer_list` constructors 
- C++03 does not support move contructor/assignment
- C++03 `std::vector<>` does not support `cbegin()` and `cend()`
- The `RowProxy` class in `checked_rectangular` is wordier and clumsier, and offers less protection against dangling references.
