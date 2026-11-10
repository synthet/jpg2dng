---
description: Core SDLC conventions — backlog discipline, minimal diffs, canonical sources.
alwaysApply: true
---

# SDLC core (always on)

- **Work from the backlog provider.** Pick/claim ready work via `/task-claim <item-ref>`; don't invent work or
  skip provider status transitions. See [`docs/project/00-backlog-workflow.md`](../../docs/project/00-backlog-workflow.md).
- **Minimal diffs.** Targeted edits over rewrites; no drive-by refactors; touch one module per task.
- **Tests for behavior changes.** Run lint + the narrowest test scope (per [`AGENTS.md`](../../AGENTS.md)).
- **Don't invent contracts.** Check [`docs/CANONICAL_SOURCES.md`](../../docs/CANONICAL_SOURCES.md)
  before using an API path, config key, schema name, or status value.
- **PRs reference issues** with `Closes #<N>`; keep the written contract and code in agreement.
- **Loop:** `/spec → /plan → /implement → /test-and-fix → /pr-ready`.
