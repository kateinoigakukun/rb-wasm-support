#!/bin/sh
':' //; exec /usr/bin/env node --experimental-wasm-eh --experimental-wasi-unstable-preview1 "$0" "$@"

const fs = require("fs");
const { WASI } = require("wasi");

const args = process.argv.slice(2);

async function runTest(filename, args) {
  const wasi = new WASI({
    args,
    env: process.env,
    preopens: {
      "/tmp": "./"
    },
  });
  const buf = fs.readFileSync(filename);
  const importObject = { wasi_snapshot_preview1: wasi.wasiImport };
  const { module, instance } = await WebAssembly.instantiate(buf, importObject);
  wasi.start(instance)
}
(async function () {
  const filename = args[0];
  await runTest(filename, args);
})()
