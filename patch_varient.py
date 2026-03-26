import re, pathlib, sys

variant_path = pathlib.Path(sys.argv[1])
variant_dir = variant_path.parent
firmware_dir = variant_path.parent.parent.parent

print("Variant dir:", variant_dir)
print("Firmware dir:", firmware_dir)
print("Files in variant dir:", list(variant_dir.iterdir()))

# ============================================
# PIN CONFIGURATION FOR SEEED XIAO ESP32-C6
# + WIO-SX1262 LORA MODULE (SPI)
# ============================================
#
# Wio-SX1262 to XIAO ESP32-C6 wiring:
# - NSS (CS):   GPIO3  (D3)
# - SCK:        GPIO8  (D8)
# - MOSI:       GPIO10 (D10)
# - MISO:       GPIO9  (D9)
# - RESET:      GPIO4  (D4)
# - DIO1:       GPIO5  (D5)
# - BUSY:       GPIO6  (D6)

PIN_BLOCK = """
// ============================================
// SEEED XIAO ESP32-C6 + WIO-SX1262 PIN CONFIG
// ============================================
// Auto-patched by build system

#undef LORA_MOSI
#undef LORA_MISO
#undef LORA_SCK
#undef LORA_CS
#undef LORA_RESET
#undef LORA_DIO1
#undef LORA_DIO2
#undef LORA_BUSY
#undef SX126X_CS
#undef SX126X_DIO1
#undef SX126X_DIO2
#undef SX126X_BUSY
#undef SX126X_RESET
#undef SX126X_DIO2_AS_RF_SWITCH
#undef SX126X_RXEN
#undef SX126X_TXEN
#undef SX126X_DIO3_TCXO_VOLTAGE
#undef PIN_SPI_MOSI
#undef PIN_SPI_MISO
#undef PIN_SPI_SCK
#undef PIN_SPI_SS
#undef GPS_RX_PIN
#undef GPS_TX_PIN
#undef HAS_GPS
#undef I2C_SDA
#undef I2C_SCL
#undef HAS_WIRE

// ============================================
// XIAO ESP32-C6 SPI PINS FOR WIO-SX1262
// ============================================
#define USE_SX1262

// SPI data lines (XIAO standard pinout)
#define LORA_MOSI        10   // D10 = GPIO10
#define LORA_MISO        9    // D9  = GPIO9
#define LORA_SCK         8    // D8  = GPIO8

// LoRa control pins
#define LORA_DIO1        5    // D5  = GPIO5 (interrupt)
#define LORA_RESET       4    // D4  = GPIO4 (active high)
#define LORA_CS          3    // D3  = GPIO3 (chip select)
#define LORA_BUSY        6    // D6  = GPIO6 (busy flag)

// SX1262 chip configuration
#define SX126X_CS        3    // D3  = GPIO3
#define SX126X_DIO1      5    // D5  = GPIO5
#define SX126X_DIO2      RADIOLIB_NC
#define SX126X_BUSY      6    // D6  = GPIO6
#define SX126X_RESET     4    // D4  = GPIO4
#define SX126X_DIO2_AS_RF_SWITCH    // Use DIO2 for RF switch
#define SX126X_RXEN      RADIOLIB_NC
#define SX126X_TXEN      RADIOLIB_NC
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

// SPI bus configuration
#define PIN_SPI_MOSI     10
#define PIN_SPI_MISO     9
#define PIN_SPI_SCK      8
#define PIN_SPI_SS       3

// Disable GPS and I2C (not in this variant)
#define HAS_GPS          0
#define HAS_WIRE         0
#define I2C_SDA          RADIOLIB_NC
#define I2C_SCL          RADIOLIB_NC
#define GPS_RX_PIN       RADIOLIB_NC
#define GPS_TX_PIN       RADIOLIB_NC

// ============================================
// END AUTO-PATCHED SECTION
// ============================================
"""

def patch_header(path):
    """Patch a header file with SPI pin configuration"""
    if not path.exists():
        print("Not found, skipping:", path)
        return
    
    content = path.read_text()
    
    # Remove old pin definitions if they exist
    # This prevents duplicate definitions
    content = re.sub(
        r'// ?=+\s*\n// ?(SEEED|XIAO|WIO|AUTO-PATCH|ESP32C6).*?\n// ?=+.*?#define\s+SX126X_DIO3_TCXO_VOLTAGE.*?\n',
        '',
        content,
        flags=re.IGNORECASE | re.DOTALL
    )
    
    # Append the new pin block
    content = content.rstrip() + "\n" + PIN_BLOCK + "\n"
    path.write_text(content)
    print(f"✓ Patched: {path}")
    print(f"  Size: {len(content)} bytes")

def patch_platformio_ini(pio_path):
    """Remove conflicting -D flags from platformio.ini"""
    if not pio_path.exists():
        print(f"⚠ Not found: {pio_path}")
        return
    
    content = pio_path.read_text()
    
    # List of defines to remove from build_flags
    pins_to_remove = [
        "LORA_MOSI", "LORA_MISO", "LORA_SCK", "LORA_CS",
        "LORA_RESET", "LORA_DIO1", "LORA_DIO2", "LORA_BUSY",
        "SX126X_CS", "SX126X_DIO1", "SX126X_DIO2", "SX126X_BUSY", 
        "SX126X_RESET", "SX126X_TXEN", "SX126X_RXEN",
        "PIN_SPI_MOSI", "PIN_SPI_MISO", "PIN_SPI_SCK", "PIN_SPI_SS"
    ]
    
    modified = False
    for pin in pins_to_remove:
        pattern = r'\s*-D' + pin + r'=[^\s]+'
        if re.search(pattern, content):
            content = re.sub(pattern, '', content)
            modified = True
            print(f"  Removed: -D{pin}")
    
    if modified:
        # Clean up multiple spaces
        content = re.sub(r' {2,}', ' ', content)
        pio_path.write_text(content)
        print(f"✓ Patched: {pio_path}")
    else:
        print(f"  No changes needed: {pio_path}")

def find_variant_inis(firmware_dir):
    """Find variant platformio.ini files"""
    variants = []
    variants_dir = firmware_dir / "variants"
    
    if variants_dir.exists():
        for ini_file in variants_dir.rglob("platformio.ini"):
            content = ini_file.read_text()
            if "seeed" in content.lower() and ("xiao" in content.lower() or "esp32c6" in content.lower()):
                variants.append(ini_file)
                print(f"Found: {ini_file}")
    
    return variants

# ============================================
# MAIN PATCH EXECUTION
# ============================================

print("\n" + "="*60)
print("SEEED XIAO ESP32-C6 + WIO-SX1262 PIN PATCHER")
print("="*60 + "\n")

# 1. Patch variant.h
print("Step 1: Patching variant.h")
print("-" * 60)
patch_header(variant_path)
print()

# 2. Patch pins_arduino.h if it exists
print("Step 2: Patching pins_arduino.h (if exists)")
print("-" * 60)
pins_arduino = variant_dir / "pins_arduino.h"
patch_header(pins_arduino)
print()

# 3. Patch platformio.ini to remove conflicting flags
print("Step 3: Patching platformio.ini files")
print("-" * 60)
main_pio = firmware_dir / "platformio.ini"
patch_platformio_ini(main_pio)
print()

# 4. Find and display variant platformio.ini files
print("Step 4: Scanning for variant-specific ini files")
print("-" * 60)
variant_inis = find_variant_inis(firmware_dir)
for ini_file in variant_inis:
    print(f"  Found variant ini: {ini_file}")
print()

# ============================================
# VERIFICATION
# ============================================

print("="*60)
print("VERIFICATION")
print("="*60 + "\n")

print("Final variant.h content (first 50 lines with pin config):")
print("-" * 60)
final_content = variant_path.read_text()
lines = final_content.split('\n')

# Show lines around pin definitions
pin_start = -1
for i, line in enumerate(lines):
    if 'SEEED' in line or 'WIO' in line or 'SX126X_CS' in line:
        pin_start = max(0, i - 2)
        break

if pin_start >= 0:
    for line in lines[pin_start:pin_start+40]:
        if line.strip():
            print(line)
print()

print("✓ Patching complete!")
print()
print("Summary:")
print(f"  - Patched: {variant_path}")
print(f"  - SPI pins configured for Wio-SX1262")
print(f"  - GPIO3 (D3):  CS/NSS")
print(f"  - GPIO4 (D4):  RESET")
print(f"  - GPIO5 (D5):  DIO1")
print(f"  - GPIO6 (D6):  BUSY")
print(f"  - GPIO8 (D8):  SCK")
print(f"  - GPIO9 (D9):  MISO")
print(f"  - GPIO10 (D10): MOSI")
print()
print("Ready to build with: pio run -e seeed_xiao_wio_sx1262")
