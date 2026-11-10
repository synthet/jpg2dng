---
name: security-review
description: Use for a lightweight pre-merge security sanity check or whenever the user asks for security review. Covers secrets, risky patterns, dependency concerns, external input validation, and side-effecting agent/tool surfaces.
capability: "security-review agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

# Security review (lightweight)

## When to use

- User asks for a **security review**, **secret check**, or **pre-merge safety pass**.
- After touching auth, crypto, file paths, or dependencies.

## Checklist (automate what you can)

1. **Secrets** — Scan changed files and recent diffs for API keys, tokens, private keys, connection strings. Flag base64 blobs that look like keys.
2. **Git** — Ensure `.env`, `*.pem`, `id_rsa`, credential files are not staged; verify `.gitignore` coverage.
3. **Dependencies** — If lockfiles changed, note major version jumps and known-critical packages; suggest `npm audit` / `pip audit` / `cargo audit` per stack (see AGENTS.md).
4. **Injection** — SQL string concat, `eval`, `exec`, raw HTML interop, unsanitized `innerHTML`, shell composition from user input.
5. **Path traversal** — User-controlled paths joined to filesystem without normalization.
6. **AuthZ** — New endpoints or tools: confirm authorization is checked, not only authentication.

## Output

- **Findings** ordered by severity (critical / high / medium / low).
- **False positives** called out briefly if uncertain.
- **Fixes** as concrete edits or steps.

## Limits

This is **not** a substitute for professional pentesting or dependency scanners in CI. Recommend enabling org-standard SAST/secret scanning where available.

Resolve helper paths from this skill directory. Run project commands from the target workspace.
