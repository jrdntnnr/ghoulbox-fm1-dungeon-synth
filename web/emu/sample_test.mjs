// SPDX-License-Identifier: GPL-3.0-only
// Real Wasm sample import: validation, occupied-slot protection, sound and persistence.
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {storeSectors, restoreSectors} from './worklet.js';

const wasm = fs.readFileSync(process.argv[2] || 'build/emu/felucca.wasm');
const header = fs.readFileSync('assets/desert/tar.hdr');
const data = fs.readFileSync('assets/desert/tar.bin');
const bank = new Uint8Array(512 + data.length).fill(255);
bank.set(header); bank.set(data, 512);
async function boot(saved) {
  const {instance} = await WebAssembly.instantiate(wasm, {});
  const e = instance.exports;
  e._initialize(); e.web_nor_erase();
  const nor = new Uint8Array(e.memory.buffer, e.web_nor(), e.web_nor_size());
  restoreSectors(nor, saved);
  e.web_boot();
  render(e, 44100);
  return {e, nor};
}
function render(e, frames) {
  let peak = 0;
  for (let n = 0; n < frames; n += 128) {
    const count = Math.min(128, frames - n);
    e.web_render(count);
    for (const x of new Float32Array(e.memory.buffer, e.web_out_l(), count)) peak = Math.max(peak, Math.abs(x));
  }
  return peak;
}
function stage(e, bytes = bank) {
  new Uint8Array(e.memory.buffer, e.web_sample_buffer(), bytes.length).set(bytes);
}
const {e, nor} = await boot();
const original = nor.slice();
stage(e);
assert.equal(e.web_sample_install(3, bank.length, 0), 1);
assert.equal(e.web_sample_install(0, bank.length - 1, 0), 1);
const broken = bank.slice(); broken[600] ^= 1;
stage(e, broken);
assert.equal(e.web_sample_install(0, bank.length, 1), 1);
assert.deepEqual(nor, original, 'bad data must not mutate saved flash');
stage(e);
const initialWrites = e.web_flash_writes_count();
assert.equal(e.web_sample_install(1, bank.length, 0), 0);
assert(e.web_flash_writes_count() > initialWrites);
const offset = 0xA0000 + 0x14000;
assert.deepEqual(nor.subarray(offset, offset + bank.length), bank);
assert.deepEqual(nor.subarray(0, offset), original.subarray(0, offset));
assert.deepEqual(nor.subarray(offset + 0x14000), original.subarray(offset + 0x14000));
const imported = nor.slice();
assert.equal(e.web_sample_install(1, bank.length, 0), 2);
assert.deepEqual(nor, imported, 'occupied slot stays untouched without replacement approval');
assert.equal(e.web_sample_install(1, bank.length, 1), 0);
render(e, 4410);
e.web_keys(1 << 7);
const peak = render(e, 22050);
e.web_keys(0); render(e, 4410);
assert(peak > .01, `Tar must render audible samples: ${peak}`);
const saved = storeSectors(nor);
const restarted = await boot(saved);
assert.deepEqual(restarted.nor.subarray(offset, offset + bank.length), bank);
stage(restarted.e);
assert.equal(restarted.e.web_sample_install(1, bank.length, 0), 2);
console.log(`emu samples: Tar renders (peak ${peak.toFixed(3)}), invalid imports rejected, occupied slots protected, bank survives restart`);
