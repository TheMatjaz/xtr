Xtrings - Extendable strings for C
==============================================================================

Xtrings (pronounced _ex-string_) is an advanced string library for C.

Yes, another string library, but bear with me...

Features
--------

- A simple, clear, well-documented API
- Cross-platform, written in ISO C99
- Compatible with C standard library functions like `printf()`
- String length and string itself in same buffer, thus no memory fragmentation

Internal structure
------------------

Heavily inspired by [Simple Dynamic Strings](https://github.com/antirez/sds),
the core idea in Xtrings is that strings _and_ their metadata (string length,
buffer length) are allocated in a contiguous buffer, rather than having a
struct point to the string itself. On top a xtring always has a null
terminator.

```text
Typical strings

Var        Metadata struct     Data buffer
           +------------+      +----------------+
str ------>| buffer ---------->| "Hello world!" |
           | buffer_len |      +----------------+
           | string_len |
           +------------+


Xtrings

Var        Metadata & data struct
           +----------------+
xtr ------>| buffer_len     |
           | strlen         |
           | "Hello world!" |
           | '\0'           |
           +----------------+
```

```c
// Not this
char some_buffer[10];

struct mystring {
    size_t length;
    char* buffer;
}

// But this
struct xtr {
    size_t capacity;
    size_t length;
    char* buffer[1]; // Length decided during heap-allocation
}
```

The trick used is called _flexible array member_, where the last member of a
struct can be interpreted as an array of arbitrary length of that same data
type, as long as enough memory is heap-allocated for it. For details, see the
ISO/IEC 9899 standard (C99), section 6.7.2.1 "Structure and union specifiers",
paragraph 16.

```c
// Instead of
malloc(sizeof(struct xtr))

// We do
malloc(sizeof(struct xtr) + desired_string_length)
```
