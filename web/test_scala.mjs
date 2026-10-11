// SPDX-License-Identifier: GPL-3.0-only
import assert from 'node:assert/strict';
import {parseScala} from './scala.js';
const scl = (name, values) => `! comment\n${name}\n${values.length}\n! intervals\n${values.join('\n')}\n`;
const rast = parseScala(scl('Rast', ['200.0','350.0','500.0','700.0','900.0','1050.0','2/1']));
assert.equal(rast.mask, 0xab5); assert.equal(rast.cents[4], -50); assert.equal(rast.cents[11], -50);
const just = parseScala(scl('Just major', ['9/8','5/4','4/3','3/2','5/3','15/8','2']));
assert.equal(just.cents[4], -14); assert.equal(just.cents[7], 2); assert(just.maxError <= .5);
const edo = parseScala(scl('12 EDO', Array.from({length:12},(_,i)=>`${100*(i+1)}.0`)));
assert.equal(edo.mask, 4095); assert(edo.cents.every(x=>x===0));
assert.equal(parseScala('\uFEFF! note\r\nOnly tonic\r\n1\r\n2/1 octave\r\n').mask, 1);
assert.equal(parseScala(scl('Collision with alternate key', ['150.0','200.0','1200.0'])).cents[1],50);
for (const values of [['3/1'], ['1/0'], ['0/1'], ['-20.0','1200.0'], ['200.0','100.0','1200.0'], ['100.0','100.0','1200.0'], ['20.0','1200.0'], ['1e2','1200.0'], ['x','1200.0'], Array.from({length:19},(_,i)=>`${1200*(i+1)/19}`)])
  assert.throws(()=>parseScala(scl('Invalid',values)));
assert.throws(()=>parseScala('Short\n7\n1200.0'));
assert.throws(()=>parseScala('x'.repeat(65537)));
console.log('Scala: cents, ratios, quarter-tones, mapping, rounding and invalid files pass');
