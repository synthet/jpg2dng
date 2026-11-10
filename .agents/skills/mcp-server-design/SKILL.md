---
name: mcp-server-design
description: Use when designing, building, reviewing, or extending a Model Context Protocol server, including external API integrations, tools/resources/prompts, transports, input validation, auth, pagination, actionable errors, and safe dispatch surfaces. Always apply when the user mentions MCP server design, MCP builder, or agent integration tools.
capability: "mcp-server-design agent asset workflow"
side_effect_level: local_write
approval_required: false
requires_tools: "See asset body for tool requirements."
output_schema: "Markdown report or documented command output."
risk_class: medium
---

# MCP server design

## Use this skill when

- Creating or extending an MCP server that exposes this project to AI agents
- Adding tools, resources, or prompts
- Configuring `.cursor/mcp.example.json` / `.mcp.json`

## Procedure

1. Read the [MCP specification](https://modelcontextprotocol.io/specification) and your SDK's docs.
2. Pick a clear, stable **server name** and a tool-naming scheme (`<scope>_<verb>`, e.g. `db_query`).
3. **Study the target API before designing tools.** Identify auth, rate limits, pagination,
   filtering, data models, write operations, and the workflows agents are expected to complete.
4. **Transport:** stdio for local servers; streamable HTTP for remote servers. Pass secrets via
   **environment variables**, never CLI args.
5. **Tools** — validate every input with a schema (e.g. Zod / pydantic). Use clear,
   action-oriented names with a consistent prefix (for example `github_create_issue` or
   `notion_search_pages`). Balance comprehensive API coverage with workflow helpers: broad API
   coverage gives agents flexibility, while workflow tools are useful for frequent multi-step tasks.
6. **Context management:** keep tool descriptions concise, support filtering/pagination, and return
   focused data instead of unbounded raw API payloads.
7. **Separate read from write.** Read-only tools are safe-by-default; **write/side-effecting tools**
   must require explicit confirmation (e.g. a `confirmed: true` flag and an allowlist of actions).
8. **Resources** — expose read-only context (status, schema, recent events) — **no secrets**.
9. **Prompts** — ship reusable prompt templates for common workflows where helpful.
10. On a downstream/dependency failure: return **structured diagnostics** with specific recovery
    suggestions; do not throw opaque errors.
11. Test tool handlers with mocked dependencies (no live side effects in unit tests).

## Safety checks

- No raw shell / file / network / arbitrary-code tools without an explicit approval policy.
- All tool inputs validated against a schema.
- Side-effecting tools gated behind confirmation + allowlist; destructive actions need a written
  authorization path. See [.agent/SAFETY.md](references/shared/.agent/SAFETY.md).

## Done criteria

- Tool list matches the published schemas; descriptions are accurate, action-oriented, and current.
- Tool outputs are paginated/filtered where needed and avoid oversized context dumps.
- Error responses include actionable next steps for auth, rate limits, validation, and missing data.
- Secrets only via env; never logged or returned in tool output.
- Build produces the server entrypoint referenced by your MCP config.
- An inventory/reference doc is regenerated when the tool set changes.

References: [MCP docs](https://modelcontextprotocol.io), Cursor / Claude Code MCP configuration guides.

## Diagnosing an existing MCP integration

Discover the actual connected tool schema before choosing a call. For broad domains, use the
server's documented search/dispatch surface and bounded read-only health, job and error queries.
Trace one concrete failure from tool response to the owning service and stored state; distinguish
transport/auth failures from application errors and missing/queued results. Do not invent tools,
repair data, retry side effects or promote a localhost endpoint globally merely because it exists.

Resolve helper paths from this skill directory. Run project commands from the target workspace.
