# Verified commands — ${PROJECT_NAME}

Quick reference of commands known to work in this repo. Keep in sync with `AGENTS.md`.

## Build / test / lint (${STACK})

```bash
${BUILD_CMD}
${TEST_CMD}
${LINT_CMD}
```

## Docs / OKF

```bash
python scripts/okf_lint.py --profile project --exclude-prefix archive/ docs
python scripts/wiki_lint.py --exclude-prefix archive/
python scripts/okf_lint.py --json --fail-on error --exclude-prefix archive/ docs   # CI
```

## Project memory

```bash
python scripts/agent-memory/log_session.py --summary "..." --outcome "..." --candidate "text|working_rule|high"
python scripts/agent-memory/dream.py
python scripts/agent-memory/promote_dream.py --dream .agent-memory/dreams/<timestamp>.md
python scripts/agent-memory/context.py
```

## Agent assets

```bash
python scripts/sync_assistant_trees.py    # regenerate Cursor and Codex mirrors from .claude/
python scripts/sync_assistant_trees.py --check
python scripts/validate_cli_skills.py     # CLI skill headings/structure
python scripts/ci/check_agent_frontmatter.py
```

## Seed a new project from this framework

```bash
python bootstrap.py --target <dir> --name "<Project Name>" --stack <python|node|go|rust|generic>
```
