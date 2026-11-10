---
capability: "test-and-fix agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

> **Claude Code:** Same intent as Cursor `/test-and-fix`. When customizing, keep in sync with `.cursor/commands/test-and-fix.md`.

# /test-and-fix — Run tests and repair failures

Use when CI is red, tests fail locally, or the user asks for a test pass.

## Inputs

- **AGENTS.md** — canonical test commands.
- Failing log output or error messages if available.
- Optional JSONL trace artifact path for recording tool calls, policy checks, and validation status; see [`docs/agent-observability.md`](../../docs/agent-observability.md).

## Steps

1. Run the **unit test** command from AGENTS.md (then integration/E2E if relevant).
2. For each failure: locate root cause, fix **minimal** code or test expectation.
3. Re-run until green or blocked; if blocked, document what is needed (data, env, flaky test).
4. If producing a trace artifact, append sanitized events for each test pass/failure, policy check, and blocker.
5. If the root cause was non-obvious, append a row to the **RCA / Failure Log** in AGENTS.md.

## Done when

- Tests pass, or there is a clear written blocker with owner/next step.
- Optional trace artifact, when produced, has a final `validation_status` of `passed`, `failed`, or `blocked`.

## Avoid

- Disabling tests or weakening assertions without explicit user approval.
