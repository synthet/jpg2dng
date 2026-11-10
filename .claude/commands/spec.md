---
capability: "spec agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

> **Claude Code:** Same intent as Cursor `/spec`. When customizing, keep in sync with `.cursor/commands/spec.md`.

# /spec — Feature or change specification

Use when starting non-trivial work. Produce a **spec** the team can review before implementation. This workflow borrows Spec Kit's spec-driven-development stance: capture the product scenario and measurable outcomes first, then let plan/tasks/code follow from that contract.

## Inputs

- Problem statement or feature request (from user message or linked issue).
- Constraints: time, scope, tech stack (see **AGENTS.md**).
- Existing principles, if any, from `.agent/PROJECT_GUIDE.md`, `AGENTS.md`, or project-specific governance docs.

## Output

1. **Summary** — One paragraph.
2. **Users / stakeholders** — Who benefits.
3. **Product scenario** — What user/business outcome changes; avoid implementation details unless they are hard constraints.
4. **Non-goals** — What is explicitly out of scope.
5. **User stories** — Short “As a … I want … so that …” bullets.
6. **Acceptance criteria** — One **EARS-form** sentence per criterion (see below), numbered `AC-1`, `AC-2`, …
7. **Assumptions and dependencies** — External systems, data, policy, migration, or rollout assumptions that must stay true.
8. **Open questions** — Unknowns and decisions needed from humans.

## Acceptance criteria (EARS)

Write each criterion as a single EARS sentence so it is testable without interpreting intent:

- **Ubiquitous:** `The <system> shall <response>.`
- **Event-driven:** `When <trigger>, the <system> shall <response>.`
- **State-driven:** `While <state>, the <system> shall <response>.`
- **Unwanted behavior:** `If <condition>, then the <system> shall <response>.`
- **Optional feature:** `Where <feature is present>, the <system> shall <response>.`

Flag any criterion as **AMBIGUOUS** and ask for a rewrite before approval when it has:

- vague verbs (“handle”, “support”, “improve”) with no observable outcome,
- more than one `shall` (split it into separate criteria), or
- an unclear subject (who/what responds?).

These IDs are the contract later verified by the `validate-implementation` skill.

## Clarification pass

Before presenting the spec as approval-ready, run a short clarification pass inspired by Spec Kit's clarify/analyze quality gates:

- Ask only for decisions that would materially change scope, acceptance criteria, tests, or architecture.
- Prefer listing assumptions over blocking on minor unknowns.
- Check every user story has at least one `AC-n`, and every `AC-n` maps to an observable user/system outcome.
- Mark unresolved issues as `OPEN QUESTION` rather than hiding them in prose.

## Done when

- Every criterion is one EARS sentence with a stable `AC-n` ID; none are marked AMBIGUOUS.
- Non-goals prevent scope creep.
- Assumptions and open questions are explicit enough for `/plan` to resolve or defer.

## Optional

- Save to `specs/<feature-slug>.md` using `templates/spec-feature.md` if the repo uses that layout.
