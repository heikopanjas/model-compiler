---
name: cpp-testing-conventions
description: General C++ testing conventions for unit, integration, regression, and behavior tests. Load when writing, reviewing, or refactoring C++ tests.
license: MIT
metadata:
  author: Heiko Panjas
  version: "1.0"
---

# C++ Testing Conventions

Read this skill before writing, reviewing, or refactoring C++ tests.
It covers deterministic tests, unit tests, integration tests, regression coverage, and test organization.

---

## C++ Testing Conventions

**General Principles:**

- Keep tests small, deterministic, and focused on observable behavior
- Prefer tests that exercise public APIs and stable contracts over implementation details
- Name tests after the behavior they verify, not the method they happen to call
- Make failures easy to diagnose with clear assertions and minimal setup
- Avoid sleeps, wall-clock assumptions, random inputs without fixed seeds, network access, and hidden shared state
- Keep test data minimal while still expressing the important edge case

**Unit Tests:**

- Add unit tests for public APIs, reusable helpers, and behavior with non-trivial branching
- Cover success cases, boundary values, invalid inputs, and ownership or lifetime behavior where relevant
- Use plain C++ test executables or the repository's chosen test framework consistently
- Return a non-zero exit code or report a framework failure when any check fails
- Keep unit tests isolated from filesystem, process, and network dependencies unless those are the behavior under test

**Behavior and Integration Tests:**

- Use integration tests for end-to-end flows across multiple components or process boundaries
- Exercise the same public entry points users or downstream systems rely on
- Include both positive tests that must succeed and negative tests that must fail with an expected diagnostic or status
- Prefer fixture files or structured input data when they make complex behavior easier to understand
- Keep integration fixtures stable and intentionally small; add new fixtures for regressions instead of overloading one large case

**Regression Tests:**

- Add a focused regression test for every bug fix when the behavior can be reproduced reliably
- Put the test near similar coverage and name it after the observable scenario
- Assert the fixed behavior directly; avoid merely checking that a crash no longer happens unless that is the contract
- Preserve failing inputs that represent meaningful user or API behavior

**Coverage Expectations:**

- Cover new public APIs, meaningful edge cases, invalid inputs, and changed behavior
- Increase test breadth when touching shared infrastructure, serialization, parsing, code generation, concurrency, memory ownership, or platform-specific code
- Do not chase numeric coverage at the expense of useful assertions
- If a change is not tested, document the reason and any residual risk

**CTest and Build Integration:**

- When a project uses CMake, prefer registering tests with CTest so local and CI runs use the same entry points
- Keep test target names descriptive and stable
- Make tests runnable from a clean build without relying on local editor state or developer-specific paths
- Keep Debug builds as the default for development testing unless release behavior is specifically under test
