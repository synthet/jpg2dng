---
name: commit-conventions
description: Use when drafting commit messages, PR titles, branch names, changelog entries, or release notes. Always apply for Conventional Commit wording, scoped commit subjects, breaking-change footers, or PR-ready messaging.
capability: "commit-conventions agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

# Commit conventions

## When to use

- User asks for a **commit message**, **PR title**, or **branch name**.
- User runs `/pr-ready` and wants aligned messaging.

## Conventional Commits (summary)

Format: `type(scope optional): description`

Common **types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`, `ci`, `perf`, `build`.

Rules:

- Description is **imperative**, lowercase, no trailing period: `fix auth redirect`, not `Fixed.`
- **Breaking change**: footer `BREAKING CHANGE: ...` or `feat!:` / `fix!:`.
- **Scope** is optional; use the area touched (`api`, `cli`, `deps`).

## PR titles

- Same as commit subject line for single-commit PRs.
- For multi-commit PRs, summarize the **user outcome**: `feat(search): add fuzzy matching`.

## Body (optional)

- What changed and why (not how every line moved).
- Link issues: `Fixes #123`.

## Project overrides

If **AGENTS.md** or `CONTRIBUTING.md` defines different rules, follow those first.

Resolve helper paths from this skill directory. Run project commands from the target workspace.
