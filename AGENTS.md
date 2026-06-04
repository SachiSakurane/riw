# AGENTS.md

## Tests

- Add or update runtime tests under `test/` for behavior changes.
- Add or update compile-time/static tests under `static_test/` for type traits, concepts, and other static guarantees.
- Run `bazel test //... --test_output=all` before finishing changes when possible.
