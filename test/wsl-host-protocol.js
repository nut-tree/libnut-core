// Protocol-only smoke test: no desktop capture, clipboard changes or input.
const assert = require("node:assert/strict");
const { spawnSync } = require("node:child_process");
const { join } = require("node:path");

const helper = process.argv[2] || join(__dirname, "..", "build", "Release", "libnut-wsl-host.exe");
const commands = [
  { id: 1, command: "hello", args: {} },
  { id: 2, command: "unknown", args: {} },
  { id: 3, command: "keys", args: { keys: ["unsupported-key"], down: true } },
  { id: 4, command: "moveMouse", args: { x: "not-a-number", y: 0 } },
  { id: 5, command: "hello", args: {} }
];
const result = spawnSync(helper, [], { input: commands.map(value => JSON.stringify(value)).join("\n") + "\n", encoding: "utf8", timeout: 10000 });
assert.ifError(result.error);
assert.equal(result.status, 0, result.stderr);
const responses = result.stdout.trim().split("\n").map(value => JSON.parse(value));
assert.equal(responses.length, commands.length);
assert.deepEqual(responses.map(value => value.id), commands.map(value => value.id));
assert.equal(responses[0].result.protocol, 1);
assert.equal(responses[4].result.protocol, 1);
assert.equal(responses[0].result.pid, responses[4].result.pid);
assert.ok(responses[0].result.pid > 0);
for (const response of responses) assert.equal(response.byteLength, 0);
for (const response of responses.slice(1, 4)) assert.equal(typeof response.error, "string");
console.log("Windows host protocol smoke test passed (5 requests, one process)");
