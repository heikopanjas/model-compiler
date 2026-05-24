---
name: git-workflow
description: Git commit message format using conventional commits, with mandatory bulleted bodies, character limits, commit types, and HEREDOC examples. Load before making a commit or reviewing git history.
license: MIT
metadata:
  author: Heiko Panjas
  version: "1.1"
---

# Git Workflow Conventions

**This skill is the single source of truth for commit messages and git workflow
details.** When this skill conflicts with summaries in `AGENTS.md` or other
project docs, follow this skill.

Read this skill in full before making a commit.

---

## Commit Protocol (CRITICAL)

- **NEVER commit automatically** — always wait for explicit user confirmation

Whenever asked to commit changes:

1. Read this skill in full
2. Stage the changes
3. Write a commit message that follows every rule below
4. Commit the changes

This is **CRITICAL**!

## Commit Message Guidelines (CRITICAL)

Follow these rules to prevent VSCode terminal crashes and ensure clean git history.

**Message Format (Conventional Commits):**

```text
<type>(<scope>): <subject>

<body>

<footer>
```

**Character Limits:**

- **Subject line**: Maximum 50 characters (strict limit)
- **Body lines**: Wrap at 72 characters per line
- **Total message**: Keep under 500 characters total
- **Blank line**: Always add a blank line between subject and body

**Subject Line Rules:**

- Use conventional commit types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `build`, `ci`, `perf`
- Scope is optional but recommended: `feat(api):`, `fix(build):`, `docs(readme):`
- Use imperative mood: "add feature" not "added feature"
- No period at end of subject line
- Keep concise and descriptive

**Body Rules (required when a body is present):**

- Add a blank line after the subject before the body
- Wrap each line at 72 characters maximum
- Explain what and why, not how
- **CRITICAL: every body line must be a bullet** — start each line with `-` followed by lowercase text
- Use one bullet per distinct change or reason
- Keep it concise — typically 2 to 4 bullets
- A subject-only commit (no body) is allowed for trivial one-line fixes

**Special Character Safety:**

- Avoid nested quotes or complex quoting inside the message text
- Avoid special shell characters in message text: `$`, `` ` ``, `!`, `\`, `|`, `&`, `;`
- Use simple punctuation only
- No emoji or unicode characters
- Hyphens in bullet markers (`-`) and plain ASCII text are allowed

**Best Practices:**

- **Break up large commits**: Split into smaller, focused commits with shorter messages
- **One concern per commit**: Each commit should address one specific change
- **Test before committing**: Ensure code builds and works
- **Reference issues**: Use `#123` format in footer if applicable

## Examples

**Good — body with bullets:**

```text
feat(api): add KStringTrim function

- add trimming function to remove whitespace from
  both ends of string
- support all encodings
```

**Good — docs commit with bullets:**

```text
docs(readme): add FM language reference

- rewrite README to match current compiler behavior
- add comprehensive FM language reference section
- remove duplicated status and language spec content
```

**Good — subject only (trivial change, no body needed):**

```text
fix(build): correct static library output name
```

**Bad — prose paragraph instead of bullets:**

```text
docs(readme): add FM language reference

Rewrite README to fix outdated content and document the full FM
language syntax, semantics, validation rules, usage, and structure.
```

**Bad — too long:**

```text
feat(api): add a new comprehensive string trimming function that handles all edge cases including UTF-8, UTF-16LE, UTF-16BE, and ANSI encodings with proper boundary checking and memory management
```

**Bad — special characters:**

```text
fix: update `KString` with "nested 'quotes'" & $special chars!
```

## Invoking git commit safely (CRITICAL)

Put the **entire message** (subject, blank line, and bulleted body) in one
commit argument. Do not split bullets across multiple `-m` flags.

**Preferred — HEREDOC (zsh / bash):**

```bash
git commit -m "$(cat <<'EOF'
docs(readme): add FM language reference

- rewrite README to match current compiler behavior
- add comprehensive FM language reference section
- remove duplicated status and language spec content
EOF
)"
```

**Alternative — message file:**

```bash
git commit -F /tmp/commit-msg.txt
```

**Wrong — one `-m` per line (adds blank lines between bullets):**

```bash
git commit -m "docs(readme): add FM language reference" \
  -m "- rewrite README to match current compiler behavior" \
  -m "- add comprehensive FM language reference section"
```

**Also wrong — multiple `-m` for subject and body paragraphs:**

Each `-m` creates a separate paragraph. If you must use two `-m` flags, put
the **entire bulleted body** in the second `-m` with embedded newlines. Prefer
HEREDOC instead.
