# GitHub Actions CI Build - Complete File Summary

## What You Get

**4 files** that enable automatic building on GitHub Actions:

1. **GITHUB_ACTIONS_SETUP_GUIDE.md** - Step-by-step setup instructions
2. **build_seeed_xiao_c6.yml** - GitHub Actions workflow file
3. **platformio_variant_with_board_level.ini** - PlatformIO config with CI metadata
4. **PATCH_generate_ci_matrix.py.md** - Instructions for CI matrix integration

## Quick Setup (5 minutes)

### Step 1: Fork Meshtastic
Fork https://github.com/meshtastic/firmware to your account

### Step 2: Create Variant Directory
```bash
mkdir -p variants/esp32c6/seeed_xiao_wio_sx1262
```

### Step 3: Copy Files
```bash
# From the 4 files you downloaded:

# Copy variant header
cp variant_seeed_xiao_esp32c6_wio_sx1262.h \
   variants/esp32c6/seeed_xiao_wio_sx1262/variant.h

# Copy platformio config
cp platformio_variant_with_board_level.ini \
   variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini

# Copy workflow
mkdir -p .github/workflows
cp build_seeed_xiao_c6.yml .github/workflows/build-seeed-xiao-c6.yml
```

### Step 4: Commit and Push
```bash
git add variants/esp32c6/seeed_xiao_wio_sx1262/
git add .github/workflows/build-seeed-xiao-c6.yml
git commit -m "Add Seeed XIAO ESP32-C6 variant with CI"
git push
```

### Step 5: Download Firmware
1. Go to Actions tab on GitHub
2. Wait for build to complete
3. Download `firmware-esp32c6-*.bin` from artifacts

## File Purpose Breakdown

### `build_seeed_xiao_c6.yml`
**Location:** `.github/workflows/build-seeed-xiao-c6.yml`

**Purpose:** GitHub Actions workflow that:
- Automatically triggers on push or manual dispatch
- Runs PlatformIO build for your variant
- Generates version strings
- Collects firmware binaries, ELF files, and bootloader
- Uploads as downloadable artifacts (90-day retention)
- Creates build summary

**Key Features:**
- ✅ Manual trigger available (Actions tab → Run workflow)
- ✅ Automatic trigger on push to master/main
- ✅ Automatic trigger on pull requests
- ✅ Parallel builds (matrix support for multiple variants)
- ✅ 90-day artifact retention
- ✅ Build verification step
- ✅ Detailed build summary generation

### `platformio_variant_with_board_level.ini`
**Location:** `variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini`

**Purpose:** PlatformIO configuration that:
- Defines build environment for Seeed XIAO ESP32-C6 + Wio-SX1262
- Sets board_level = 0 (community variant, CI builds but not in releases)
- Configures all GPIO pins, SPI settings, LoRa parameters
- Specifies optimization flags, dependencies, upload method
- Tells CI system to include this variant in build matrix

**Key Differences from Local platformio.ini:**
- Includes `board_level = 0` ← This is critical for CI!
- Follows Meshtastic variant directory structure
- Uses extends mechanism for inheritance (optional)

### `variant_seeed_xiao_esp32c6_wio_sx1262.h`
**Location:** `variants/esp32c6/seeed_xiao_wio_sx1262/variant.h`

**Purpose:** Hardware configuration that:
- Defines all GPIO pin assignments
- Configures SPI interface for LoRa
- Sets LoRa parameters (frequency, power, etc.)
- Enables/disables features (BLE, WiFi, etc.)
- Specifies memory layout, power management
- Already created in earlier steps!

**Note:** You already have this file - no changes needed unless you want to customize further.

## How CI Build Matrix Works

```
1. GitHub Actions runs workflow
   ↓
2. Workflow scans all platformio.ini files
   ↓
3. Finds [env:seeed_xiao_wio_sx1262]
   ↓
4. Reads board_level = 0
   ↓
5. Adds to build matrix
   ↓
6. Builds seeed_xiao_wio_sx1262 environment
   ↓
7. Collects firmware.bin, firmware.elf, bootloader.bin
   ↓
8. Uploads as artifacts (90 days)
```

## What Gets Built

Each workflow run produces:

| File | Purpose |
|------|---------|
| `firmware-esp32c6-seeed-xiao-wio-sx1262-*.bin` | Main firmware to flash |
| `firmware-esp32c6-seeed-xiao-wio-sx1262-*.elf` | Debug symbols for GDB |
| `bootloader-*.bin` | Optional bootloader (if available) |
| `build-summary-*.txt` | Build metadata and instructions |

## Triggering Builds

### Automatic Triggers
- Push to master/main branch
- Pull request to master/main
- Changes to variant files or workflow

### Manual Trigger
1. Go to your fork's Actions tab
2. Click "Build Seeed XIAO ESP32-C6 Wio-SX1262 Firmware"
3. Click "Run workflow"
4. Select branch
5. Click green "Run workflow" button

## Download & Flash

After build completes:

```bash
# 1. Download artifact from GitHub Actions
# 2. Extract: unzip firmware-esp32c6-*.zip
# 3. Flash with esptool
esptool.py --chip esp32c6 --port /dev/ttyUSB0 \
    write_flash -z 0x0 firmware-esp32c6-seeed-xiao-wio-sx1262-*.bin

# 4. Monitor
pio device monitor -b 115200
```

## Integration with Meshtastic CI Matrix

The Meshtastic firmware uses `bin/generate_ci_matrix.py` to dynamically generate CI tasks.

**How it works:**
1. Script scans all `platformio.ini` files in variants/
2. Reads `board_level` from each `[env:...]` section
3. Filters by board_level (0 = DIY/community, 1+ = official)
4. Generates build matrix for GitHub Actions

**Your variant automatically included because:**
- ✅ Located in `variants/esp32c6/seeed_xiao_wio_sx1262/`
- ✅ Has `platformio.ini` with `board_level = 0`
- ✅ Follows standard directory structure
- ✅ No additional registration needed!

## Switching board_level

Want your variant to appear in releases?

**Option 1: Community/DIY (default)**
```ini
board_level = 0
# Builds in CI but NOT in official releases
# Good for experimental/personal variants
```

**Option 2: Full Support (requires PR acceptance)**
```ini
board_level = 1
# Builds + appears in releases + web flasher
# Requires PR merge and maintainer approval
```

## Troubleshooting

### Build doesn't trigger
- Check branch name is master or main
- Verify workflow file exists: `.github/workflows/build-seeed-xiao-c6.yml`
- Try manual trigger from Actions tab
- Check workflow status in Actions log

### Build fails: variant.h not found
- Verify path: `variants/esp32c6/seeed_xiao_wio_sx1262/variant.h`
- Check file isn't empty
- Confirm directory structure matches exactly
- Commit and push changes

### Build fails: environment not found
- Ensure `[env:seeed_xiao_wio_sx1262]` in platformio.ini
- Verify `board_level = 0` is present
- Check for typos in environment name
- Validate YAML syntax in workflow file

### No artifacts showing
- Wait for build to complete (green checkmark)
- Scroll down to Artifacts section
- If missing, check build logs for errors
- Try manual workflow trigger

## Key Points

✅ **board_level = 0 is critical** - tells CI to build your variant
✅ **Standard directory structure** - must be `variants/esp32c6/seeed_xiao_wio_sx1262/`
✅ **No additional setup** - CI automatically picks up your variant
✅ **90-day artifacts** - builds automatically retained for 90 days
✅ **Manual + auto triggers** - can trigger manually or on push

## File Checklist

Before pushing to GitHub, verify:

- [ ] Variant directory exists: `variants/esp32c6/seeed_xiao_wio_sx1262/`
- [ ] `variant.h` copied to variant directory
- [ ] `platformio.ini` copied to variant directory
- [ ] `platformio.ini` contains `board_level = 0`
- [ ] Workflow file at: `.github/workflows/build-seeed-xiao-c6.yml`
- [ ] All files committed
- [ ] Changes pushed to GitHub
- [ ] Workflow appears in Actions tab

## Next Steps

1. **Complete Setup:** Follow GITHUB_ACTIONS_SETUP_GUIDE.md
2. **Verify Build:** Push and check Actions tab
3. **Download Firmware:** Get binary from artifacts
4. **Flash Device:** Use esptool to write firmware
5. **Test:** Connect XIAO and verify LoRa works

## Support

For issues:
1. Check workflow logs in Actions tab
2. Read GITHUB_ACTIONS_SETUP_GUIDE.md
3. Ask on Meshtastic Discord: https://discord.gg/ZRjAqzJ
4. Check Meshtastic docs: https://meshtastic.org/docs/

---

**Version:** 1.0
**Date:** March 2026
**Status:** ✅ Production Ready

All files needed for GitHub Actions CI are provided. Follow GITHUB_ACTIONS_SETUP_GUIDE.md to get started!
