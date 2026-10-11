// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import { readFile } from 'node:fs/promises';
import { createHash } from 'node:crypto';
import vm from 'node:vm';
import { prepareDesertBank, DESERT_BASE } from './desert.js';
const html = await readFile(new URL('./editor.html', import.meta.url), 'utf8');
const proto = html.slice(html.indexOf('/*PROTO-BEGIN*/'),html.indexOf('/*PROTO-END*/'));
const {parseWav,takeSample,buildSlot} = vm.runInNewContext(proto+';({parseWav,takeSample,buildSlot})',{setTimeout,clearTimeout,setInterval,clearInterval,console});
let calls=0;
const fetcher = async url => {
  calls++;
  const b=await readFile(url);
  return {ok:true,json:async()=>JSON.parse(b),arrayBuffer:async()=>b.buffer.slice(b.byteOffset,b.byteOffset+b.byteLength)};
};
const decode = async f => {
  const {x,sr}=parseWav(await f.arrayBuffer());assert.equal(sr,22050);
  return {fname:f.name,s:takeSample(x,0,x.length),root:60};
};
const banks=JSON.parse(await readFile(new URL('banks.json',DESERT_BASE)));
for(let k=0;k<3;k++) {
  const draft=await prepareDesertBank(k,decode,fetcher);
  assert.equal(draft.name,banks[k].name);
  assert.equal(draft.files.length,banks[k].files.length);
  const built=buildSlot(draft.name,draft.files);
  assert.equal(built.data.length,banks[k].bytes);
  assert(built.data.length<=81408);
  for(let i=0;i<draft.files.length;i++) {
    const z=banks[k].files[i], bytes=await readFile(new URL(z.file,DESERT_BASE));
    assert.equal(createHash('sha256').update(bytes).digest('hex'),z.sha256);
    assert.equal(draft.files[i].root,z.root);
    assert(Math.abs(z.median_error_cents)<3);
  }
}
assert.equal(calls,15); // 3 manifests + 12 note files
await assert.rejects(prepareDesertBank(3,decode,fetcher),/Unknown/);
await assert.rejects(prepareDesertBank(0,decode,async()=>({ok:false,status:404})),/Cannot load/);
console.log('desert web: three banks, note roots, WAV integrity, slot capacity and fetch failures pass');
