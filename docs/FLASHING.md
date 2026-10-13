# Flashing the current FM-1 development build

Built on 2026-10-13 as **SARDAB 2.1**, combining Ghoulbox 2.1 features with
our Felucca/SLOOP base, maqam tuning and desert instruments. The target build,
audio references, migrations, functional UI, emulator and update-loader
simulations pass. Physical-device validation is pending; the UI layout audit
and a pre-existing PHASER host-cost estimate remain unresolved.
[Full validation status](INTEGRATION-STATUS.md).

## Install from this computer

1. Save any sounds or music you need to keep. On compatible Felucca/Ghoulbox
   firmware, connect the [local editor](http://127.0.0.1:8792/webapp/editor/),
   use **Complete backup → Save backup**, and close the editor afterward.
   Stock M-VAVE firmware does not support this complete-backup protocol.
   The normal Install button does **not** save a backup automatically.
2. Turn the FM-1 on and connect it directly to the computer with a USB **data**
   cable. Close DAWs and other MIDI applications/tabs. Leave it in its normal
   running mode; the installer requests update mode itself.
3. In **Chrome or Edge**, open
   <http://127.0.0.1:8792/webapp/installer/>. This local page contains
   **sardab-2.1-preview**. The same preview is available from the
   [public installer](https://jrdntnnr.github.io/ghoulbox-fm1-dungeon-synth/webapp/installer/).
   Check that its label says **sardab-2.1-preview** before installing.
4. Click **Install** and allow **MIDI / SysEx** access when prompted.
5. Keep USB connected and the computer awake through **Verifying**, **Writing**
   and **Restarting**. Wait for **Done** and the FM-1 to boot before closing
   the installer or disconnecting it.

No separate programmer is needed for a normally running FM-1.

## First checks on the device

Start with a low volume. Open **PRESETS**, turn **knob 4 (LIST)** to
**DESERT**, then browse with the **PRESETS knob**. Try **SANTUR**, **MIJWIZ**
and **ARGHUL**. For a simple tuning check, select **ROOT C**,
**MAQAM RAST**, **MAP CHR**, **CHORD OFF**, **TEMP EQUAL**: E and B should be half-flat.
Try **DIA3** for a microtonal triad. Save a small test project and verify that
it returns after restarting.

The Tar bank is optional and is not automatically written into a sample slot
by flashing firmware. Use the editor's Samples controls to prepare/upload Tar.
The DESERT entries USR1/2/3 play the banks in those slots; an empty slot is silent.

## If the installer cannot connect

- **FM-1 not found:** check the cable supports data, close other MIDI apps,
  and check Chrome's site permission for MIDI/SysEx.
- **Stopped / update mode not found:** only after the installer reports that
  it has stopped, reconnect USB and press Install again to resume. Do not
  interrupt an active write.
- **Installed firmware will not boot normally:** hold OCT− alone at power-on
  for USB rescue, then retry the installer. This recovery feature requires
  this build (or upstream 2.x) to have been installed successfully.
- **Black screen and WL80UBOOT instead of MIDI:** this is a different recovery
  mode; follow the repository's recovery guidance for FM-1 Transporter.

If the local server has stopped, run from the repository root:

```sh
python3 -m http.server 8792 --bind 127.0.0.1 --directory build/device-install
```

## Package identity

- File: `build/device-install/firmware/felucca-sardab-2.1-preview.fwsc`
- Size: 610,086 bytes; updater identity `FM-1_900`
- SHA-256: `2ab4bd41b416f432b0068c2ffc8117da0c023508ba19825403a9f7b94c082e76`
- Application: 579,304 bytes; 2,260 bytes free
- Static RAM: 56,068 / 73,728 bytes; SDRAM pool: 333,172 / 368,640 bytes

The updater identity is shared with earlier development builds. The display
version is SARDAB 2.1; use the package hash to identify this exact build. This target
profile omits the retired SLICE engine; old SLICE tracks will not play.
Validation logs are under `build/assessment/ghoulbox-2/`.
