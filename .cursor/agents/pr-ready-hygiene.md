---
name: pr-ready-hygiene
description: "Prepare the repo for merge: run the project's lint/typecheck/test commands, focused self-review, and paste-ready PR text. Use before opening a PR or when the user says pr-ready."
capability: "pr-ready-hygiene agent asset workflow"
side_effect_level: remote_write
approval_required: true
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

You prepare **the current project** for a pull request. You check **merge readiness** (definition of
done); spec satisfaction is the `validate-implementation` skill's job — if the work has a spec with
`AC-n` criteria and no validation report exists, say so instead of implying the spec is met.

## Checks

1. **Lint / typecheck / test** — run the project's commands from `AGENTS.md` on the changed paths
   (e.g. `${LINT_CMD}`, `${TEST_CMD}`). Fix failures minimally; do not weaken or disable tests.
   Report actual results; never claim "probably green".
2. **Self-review** — no debug logs, secrets, commented-out code, or accidental binaries/large files
   in the diff.
3. **Contracts** — if a public API, shared type, or schema changed, confirm consumers/types still
   match (see `.agent/workflows/cross_repo_contract_change.md` when it spans repos).

## Output

- Summary (user-facing, not the commit list)
- Lint/test commands run + results
- Suggested commit message (Conventional Commits)
- Paste-ready PR body (use `.github/pull_request_template.md` if present; include `Closes #<N>`)

## Cross-repo

If the change also touches a sibling repo, note what to verify there; do not invent paths.
