# GitHub Actions CI Build Setup for Seeed XIAO ESP32-C6 + Wio-SX1262

## Overview

This guide explains how to set up GitHub Actions to automatically build your custom Meshtastic firmware for the Seeed XIAO ESP32-C6 whenever you push to GitHub.

**Benefits:**
- ✅ Automatic builds on every push
- ✅ Download pre-built binaries from GitHub
- ✅ No need to build locally (though you still can)
- ✅ 90-day artifact retention
- ✅ Separate debug files available

## Step 1: Fork Meshtastic Firmware

1. Go to: https://github.com/meshtastic/firmware
2. Click **Fork** (top right)
3. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR-USERNAME/firmware.git
   cd firmware
   git submodule update --init --recursive
   ```

## Step 2: Create Variant Directory Structure

Create this exact directory structure in your fork:

```
firmware/
├── variants/
│   └── esp32c6/
│       └── seeed_xiao_wio_sx1262/
│           ├── variant.h
│           └── platformio.ini
└── .github/
    └── workflows/
        └── build-seeed-xiao-c6.yml
```

### 2a. Create the Variant Directory

```bash
mkdir -p variants/esp32c6/seeed_xiao_wio_sx1262
```

### 2b. Copy Files to Your Variant

Copy these files (provided separately) to the variant directory:

- `variant_seeed_xiao_esp32c6_wio_sx1262.h` → `variants/esp32c6/seeed_xiao_wio_sx1262/variant.h`
- `platformio_variant_with_board_level.ini` → `variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini`

```bash
# Copy variant header
cp variant_seeed_xiao_esp32c6_wio_sx1262.h \
   variants/esp32c6/seeed_xiao_wio_sx1262/variant.h

# Copy platformio config
cp platformio_variant_with_board_level.ini \
   variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini
```

### 2c. Set Up Workflow File

Copy the GitHub Actions workflow to:

```bash
mkdir -p .github/workflows
cp build_seeed_xiao_c6.yml .github/workflows/build-seeed-xiao-c6.yml
```

## Step 3: Commit and Push

```bash
git add variants/esp32c6/seeed_xiao_wio_sx1262/
git add .github/workflows/build-seeed-xiao-c6.yml
git commit -m "Add Seeed XIAO ESP32-C6 + Wio-SX1262 variant with CI support"
git push origin master
```

## Step 4: Verify Workflow Setup

1. Go to your fork on GitHub
2. Click **Actions** tab
3. You should see "Build Seeed XIAO ESP32-C6 Wio-SX1262 Firmware"
4. Click on the workflow run to see build progress

## Step 5: Download Firmware

Once the build completes:

1. Click on the completed workflow run
2. Scroll to **Artifacts** section
3. Download `firmware-esp32c6-*` zip file
4. Extract and flash to your XIAO

## File Locations & What Goes Where

```
Your Fork (GitHub)
├── variants/
│   └── esp32c6/
│       └── seeed_xiao_wio_sx1262/
│           ├── variant.h
│           │   └── Hardware defines (GPIO pins, features, etc.)
│           └── platformio.ini
│               └── Build config + board_level = 0
│
└── .github/workflows/
    └── build-seeed-xiao-c6.yml
        └── GitHub Actions workflow (auto-triggers on push)
```

## Workflow Trigger Events

The workflow automatically builds on:

- **Push to master/main**: When you push code changes
- **Pull requests**: Before merging to master/main
- **Manual trigger**: Actions tab → "Run workflow"

## Build Output

After each build, you'll get:

- `firmware-esp32c6-seeed-xiao-wio-sx1262-*.bin` - Main firmware to flash
- `firmware-esp32c6-seeed-xiao-wio-sx1262-*.elf` - Debug symbols
- `bootloader-*.bin` - Bootloader (if included)
- `build-summary-*.txt` - Build metadata

## Flashing Firmware from GitHub Build

1. Download the firmware binary from GitHub Actions artifacts
2. Connect XIAO ESP32-C6 via USB-C
3. Flash using esptool:

```bash
# Install esptool if needed
pip install esptool

# Flash the firmware
esptool.py --chip esp32c6 --port /dev/ttyUSB0 \
    write_flash -z 0x0 firmware-esp32c6-seeed-xiao-wio-sx1262-*.bin

# Monitor output
pio device monitor -b 115200 --port /dev/ttyUSB0
```

## Troubleshooting CI Builds

### Build Not Triggering

**Problem:** Workflow doesn't run when I push

**Solution:**
1. Verify `.github/workflows/build-seeed-xiao-c6.yml` exists in your fork
2. Check the Actions tab - you should see the workflow listed
3. Try manual trigger: Actions → Select workflow → "Run workflow"
4. Check branch name (workflow targets master/main)

### Build Fails: "variant.h not found"

**Problem:** Build fails with variant.h missing

**Solution:**
1. Verify file exists: `variants/esp32c6/seeed_xiao_wio_sx1262/variant.h`
2. Check it's not empty
3. Verify directory structure exactly matches
4. Commit and push again

### Build Fails: "PlatformIO environment not found"

**Problem:** Environment "seeed_xiao_wio_sx1262" not found

**Solution:**
1. Check `platformio.ini` has this exact section:
   ```ini
   [env:seeed_xiao_wio_sx1262]
   board_level = 0
   platform = espressif32 @ ^6.7.0
   board = esp32-c6-devkitc-1
   # ... rest of config
   ```
2. Verify filename is exactly `platformio.ini` (lowercase)
3. Check for typos in environment name

### Build Fails: "No space left on device"

**Problem:** GitHub Actions runner out of space

**Solution:**
1. Reduces build is usually a temporary issue
2. Retry the build
3. Comment out optional features in variant.h
4. Check artifact cleanup is working (90-day retention)

### Artifacts Not Showing

**Problem:** No download button for artifacts

**Solution:**
1. Wait for build to complete (shows checkmark)
2. Scroll down to Artifacts section
3. If still missing, check build logs for errors
4. Artifacts are retained for 90 days

## Making Changes and Rebuilding

To rebuild with changes:

1. Edit variant files locally
2. Commit and push:
   ```bash
   git add variants/esp32c6/seeed_xiao_wio_sx1262/
   git commit -m "Update Seeed variant config"
   git push
   ```
3. Build automatically triggers
4. Download new firmware from artifacts

## Advanced: Custom Build Matrix

To modify the build matrix (add more variants, change targets, etc.):

1. Edit `.github/workflows/build-seeed-xiao-c6.yml`
2. Look for the `matrix:` section
3. Add or modify entries:
   ```yaml
   matrix:
     include:
       - { pio_env: "seeed_xiao_wio_sx1262", arch: "esp32c6" }
       - { pio_env: "other_variant", arch: "esp32s3" }  # Add more
   ```
4. Push changes - workflow updates automatically

## Integration with Official Meshtastic CI

To potentially get your variant into official Meshtastic CI:

1. Create a Pull Request to meshtastic/firmware
2. Add your variant directory structure
3. Maintainers will review and potentially accept
4. If accepted, appears in official releases

**Note:** Community variants (board_level=0) won't appear in web flasher, but will build in official CI if PR is merged.

## GitHub Actions Concepts

### What is board_level?

- `board_level = 0`: Community variant - builds but not in releases
- `board_level = 1`: Full support - builds + in releases + web flasher
- `board_level = 2`: Special/experimental - special CI handling

For your custom board, keep `board_level = 0`.

### How CI Matrix Works

1. Workflow runs → finds `platformio.ini` files
2. Reads `board_level` from each `[env:...]` section
3. Adds matching envs to build matrix
4. Builds each in parallel
5. Uploads artifacts

### Artifact Storage

- Artifacts stored 90 days by default
- Each build creates new artifacts
- Old artifacts auto-deleted after 90 days
- Can be configured in workflow YAML

## Reference Files

You need these 3 files:

1. **variants/esp32c6/seeed_xiao_wio_sx1262/variant.h**
   - Hardware configuration and GPIO defines
   - 300+ lines of commented config

2. **variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini**
   - PlatformIO build settings
   - Includes `board_level = 0`

3. **.github/workflows/build-seeed-xiao-c6.yml**
   - GitHub Actions workflow
   - Handles versioning, building, artifact upload

## Files Provided

All three files are provided separately:
- `variant_seeed_xiao_esp32c6_wio_sx1262.h`
- `platformio_variant_with_board_level.ini`
- `build_seeed_xiao_c6.yml`

Copy these to the correct locations in your fork.

## Testing the Setup

### Test 1: Local Build (Verify Locally First)

```bash
# Before setting up CI, verify it builds locally
pio run -e seeed_xiao_wio_sx1262

# Should produce: .pio/build/seeed_xiao_wio_sx1262/firmware.bin
```

### Test 2: Workflow Trigger

1. Push to your fork
2. Go to Actions tab
3. Verify workflow runs

### Test 3: Download and Flash

1. Download artifact
2. Flash to XIAO with esptool
3. Verify device works

## Support & Help

If you run into issues:

1. Check workflow logs (Actions tab → failed run → logs)
2. Verify file structure matches exactly
3. Check for typos in config files
4. Ask on Meshtastic Discord: https://discord.gg/ZRjAqzJ

## Next Steps

Once CI is working:

1. Test local flashing from GitHub binaries
2. Create multiple builds (add GPS, screen, etc.)
3. Consider contributing to official Meshtastic
4. Share your setup with community

---

**Version:** 1.0
**Last Updated:** March 2026
**Status:** ✅ Ready to Use
