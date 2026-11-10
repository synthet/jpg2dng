# Agent infrastructure inventory — ${PROJECT_NAME}

**Last reviewed:** 2026-07-10. Machine-readable mirror: [`AGENT_INFRA_STATUS.json`](AGENT_INFRA_STATUS.json).

Keep this table current as the single catalog of every agent-facing asset. `.claude/` is canonical;
Cursor and Codex mirrors are generated from it.

| Path | Purpose | Scope | Status |
|------|---------|-------|--------|
| [CLAUDE.md](../CLAUDE.md) | Human + agent orientation, backlog, architecture | project | active |
| [AGENTS.md](../AGENTS.md) | MCP config, tool surface, coding-agent contract | agents, MCP | active |
| [.agent/SAFETY.md](SAFETY.md) | Secrets, artifacts, git hygiene, MCP/external-review safety | governance | active |
| [.agent/PROJECT_GUIDE.md](PROJECT_GUIDE.md) | Navigation for `.agent/` | docs | active |
| [.agent/INFRA_QUICKSTART.md](INFRA_QUICKSTART.md) | One-page checklist | diagnostics | active |
| [.agent/COMMANDS.md](COMMANDS.md) | Verified command quick reference | testing | active |
| [.agent/SKILL_CHANGE_AST10_REVIEW.md](SKILL_CHANGE_AST10_REVIEW.md) | PR checklist guarding skill drift | governance | active |
| [.agent/SKILL_INVENTORY.md](SKILL_INVENTORY.md) | Skill/agent catalog, risk tiers, frontmatter invariants | governance | active |
| [.agent/ai_edit_spec.md](ai_edit_spec.md) | Minimal-diff editing conventions | coding | active |
| [.agent/subagents/README.md](subagents/README.md) | Logical roles ↔ subagents; allowed/forbidden edits | coding | active |
| [.agent/workflows/*.md](workflows/) | Reusable SDLC + safety playbooks | workflow | active |
| [.claude/commands/*.md](../.claude/commands/) | Slash commands | workflow | active |
| [.claude/skills/*/SKILL.md](../.claude/skills/) | Skills | coding | active |
| [.claude/agents/*.md](../.claude/agents/) | Subagents | coding | active |
| [.claude/rules/*.md](../.claude/rules/) | Always-on rules | governance | active |
| [.cursor/](../.cursor/) | Generated mirror of `.claude/` + `mcp.example.json` | mirror | generated |
| [.agents/skills/](../.agents/skills/) | Generated Codex repository skills | mirror | generated |
| [.codex/](../.codex/) | Codex project config + generated custom subagents | codex | active |
| [.agent-memory/](../.agent-memory/) | Project memory (log → dream → promote) | memory | active |
| [docs/CANONICAL_SOURCES.md](../docs/CANONICAL_SOURCES.md) | Authority map | governance | active |
| [docs/WIKI_SCHEMA.md](../docs/WIKI_SCHEMA.md) | Wiki taxonomy & maintenance | docs | active |
| [docs/ai-workflow/README.md](../docs/ai-workflow/README.md) | Asset map + SDLC loop + phase gates | workflow | active |
| [.github/workflows/ci.yml](../.github/workflows/ci.yml) | CI: sync check, OKF lint, frontmatter, secrets, tests | ci | active |
| [scripts/ci/](../scripts/ci/) | Stdlib validators (frontmatter, secrets) + changed-docs lint | ci | active |
| [tests/](../tests/) | Bootstrap combinatorial test suite | testing | active |
| [CHANGELOG.md](../CHANGELOG.md) | Keep-a-Changelog release history | docs | active |

## Drift watchlist

- **Cursor/Codex mirrors:** regenerate via `python scripts/sync_assistant_trees.py` after editing `.claude/`.
- **MCP tool inventory:** regenerate the `<!-- BEGIN/END MCP TOOL INVENTORY -->` block in `AGENTS.md` when tools change.
- **Skills:** any skill/command/agent change ships every generated mirror in the same PR.
