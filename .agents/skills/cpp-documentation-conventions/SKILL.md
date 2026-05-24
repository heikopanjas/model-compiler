---
name: cpp-documentation-conventions
description: General C++ documentation conventions for comments, public API docs, READMEs, examples, and implementation accuracy. Load when writing, reviewing, or updating C++ documentation.
license: MIT
metadata:
  author: Heiko Panjas
  version: "1.0"
---

# C++ Documentation Conventions

Read this skill before writing, reviewing, or updating documentation for C++ code.
It covers implementation accuracy, comments, public API documentation, user guides, examples, and change notes.

---

## C++ Documentation Conventions

**General Principles:**

- Keep documentation synchronized with the implementation
- Prefer concise, accurate guidance over aspirational or fictional examples
- Verify public examples against real class names, function signatures, command-line options, and generated APIs
- Document stable behavior and public contracts; avoid describing incidental implementation details as guarantees
- Update documentation in the same change as the behavior it describes whenever practical

**Code Comments:**

- Write self-documenting C++ first, then add comments only where they clarify intent
- Comment non-obvious decisions, invariants, ownership expectations, platform behavior, algorithms, and generated-code assumptions
- Explain why the code does something, not what each statement mechanically does
- Keep comments close to the code they explain and remove stale comments when behavior changes
- Use `//` for implementation comments unless the project has a stricter local convention

**Public API Documentation:**

- Document public C++ APIs in header files with Doxygen-style `///` comments when the API is intended for reuse
- Include purpose, parameters, return values, side effects, ownership expectations, and exceptions where applicable
- Document preconditions, postconditions, and lifetime requirements when callers must understand them
- Keep API documentation focused on caller-facing behavior rather than internal implementation steps

**README and User Guides:**

- Update user-facing documentation when changing build commands, configuration, command-line options, file formats, public APIs, generated output, or example workflows
- Keep quick-start instructions short and verified
- Prefer one complete, working example over many partial snippets
- Separate current behavior from planned or future work

**Examples and Generated Artifacts:**

- Ensure examples compile, run, or validate with the current code when practical
- Keep generated examples synchronized with the generator or tool that produces them
- Avoid hand-written example output that looks generated unless it is checked against the real generator
- Use realistic names and inputs without coupling the documentation to private or environment-specific data

**Change Notes:**

- When a repository maintains a changelog, release notes, or decision log, update it for user-visible behavior changes and project conventions
- Capture the reason for important documentation or convention changes, not just the edit itself
- Do not duplicate long-form documentation across multiple files; link or reference the canonical source instead
