#!/bin/sh
':' //; exec /usr/bin/env node --experimental-wasm-eh --experimental-wasi-unstable-preview1 "$0" "$@"

const fs = require("fs");
const { WASI } = require("wasi");

const args = process.argv.slice(2);

async function runTest(filename) {
  const wasi = new WASI({});
  const buf = fs.readFileSync(filename);
  const importObject = { wasi_snapshot_preview1: wasi.wasiImport };
  const { module, instance } = await WebAssembly.instantiate(buf, importObject);
  wasi.start(instance)
}
(async function () {
  for (const test of args) {
    console.log(`Running ${test}...`);
    await runTest(test);
  }
})()
