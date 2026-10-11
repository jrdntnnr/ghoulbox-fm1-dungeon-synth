# Integration in progress

This working branch combines GHOULBOX microtuning with Felucca main at
`a4d7ca2e5d52a4c35e2468401f00d883cadbda03` (1.5.1). Validation is ongoing;
this checkpoint is not a hardware-tested release.

On 2026-10-11 GHOULBOX origin advanced to `b61a931` (1.4). Its changes are
being merged next. The previous test package predates those updates.

Completed so far: Linux JieLi compiler provisioning, verified SDK assets,
GBM2 compact project storage, GBM1/preset migration, octave Scala import,
40 factory scales plus a custom scale and per-key cent tuning. A trial
profile without the hidden SLICE engine builds in the original flash slot.

Outstanding: GHOULBOX 1.4 integration, remaining regression and editor
fixes, final package rebuild, physical FM-1 validation. No device has been
flashed. Test logs and provisional artifacts live under build/assessment.
