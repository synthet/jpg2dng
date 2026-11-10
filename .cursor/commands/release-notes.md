---
capability: "release-notes agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

> **Claude Code:** Same intent as Cursor `/release-notes`. When customizing, keep in sync with `.cursor/commands/release-notes.md`.

# /release-notes — User-facing changelog slice

Use before tagging a release or publishing notes.

## Inputs

- Commit range, merged PRs, or closed issues since last release.
- Product name and version (or “next”).

## Output

1. **Highlights** — 3–7 bullets for end users (behavior, not implementation).
2. **Breaking changes** — Clear migration notes.
3. **Deprecations** — What to stop using and by when.
4. **Contributors** — Optional thank-you / attribution if the project does that.

## Tone

- Present tense or “Now …”; avoid internal jargon.

## Done when

- A release manager could paste this into GitHub Releases or CHANGELOG.md with minimal edits.
