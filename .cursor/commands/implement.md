---
capability: "implement agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

> **Claude Code:** Same intent as Cursor `/implement`. When customizing, keep in sync with `.cursor/commands/implement.md`.

# /implement — Execute an approved plan

Use when the user has approved a plan or given a small, explicit task.

## Inputs

- Approved plan or task list.
- **AGENTS.md** for lint/test/build commands.

## Steps

1. Write the failing test stubs from the plan **before** implementation; confirm they fail.
2. Implement in **minimal diffs** until the stubs pass; match existing style.
3. Run **lint** and **tests** from AGENTS.md; fix failures.
4. Summarize what changed and where.

## Done when

- All agreed items are implemented.
- Tests written-and-failing before code, now passing (or failures explained with next steps).

## Checklist

- [ ] Test stubs written and **failing** before implementation began
- [ ] Tests pass after implementation
- [ ] No unrelated refactors
- [ ] No secrets committed
- [ ] AGENTS.md commands run (or documented why not)
