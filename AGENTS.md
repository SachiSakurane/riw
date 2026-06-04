# AGENTS.md

## Repository Purpose

`riw` is a C++ utility library for pieces that are useful in personally developed applications.
It collects small functions, classes, traits, concepts, and other reusable building blocks that are likely to be needed across those applications.

The library also provides implementations of useful C++ facilities that are not yet available in the compilers or standard libraries targeted by this repository.
Prefer focused, reusable additions that fit naturally under `include/riw/` over application-specific code.

## Tests

- Add or update runtime tests under `test/` for behavior changes.
- Add or update compile-time/static tests under `static_test/` for type traits, concepts, and other static guarantees.
- Run `bazel test //... --test_output=all` before finishing changes when possible.
