// SPDX-License-Identifier: GPL-3.0-only
// Optional banks are prepared in memory. Upload uses the existing explicit action.
export const DESERT_BASE = new URL('../assets/desert/', import.meta.url);
export const DESERT_NAMES = ['Oud', 'Qanun', 'Ney Air', 'Tar'];
export async function prepareDesertBank(k, decode, fetcher = fetch) {
  if (!Number.isInteger(k) || k < 0 || k >= DESERT_NAMES.length) throw new Error('Unknown desert bank');
  const get = async (file) => {
    const response = await fetcher(new URL(file, DESERT_BASE));
    if (!response.ok) throw new Error(`Cannot load ${file} (${response.status})`);
    return response;
  };
  const banks = await (await get('banks.json')).json(), bank = banks[k];
  const files = [];
  for (const zone of bank.files) {
    const bytes = await (await get(zone.file)).arrayBuffer();
    const f = await decode({ name: zone.file, arrayBuffer: async () => bytes });
    f.root = zone.root; // authoritative roots, independent of filename parsing
    files.push(f);
  }
  return { name: bank.name, files };
}
