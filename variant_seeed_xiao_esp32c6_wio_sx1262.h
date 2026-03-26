/*
  Meshtastic Firmware - Seeed XIAO ESP32-C6 + Wio-SX1262
  
  Board: Seeed Studio XIAO ESP32-C6
  LoRa: Wio-SX1262 (SX1262 chip, SPI interface)
  
  The Wio-SX1262 connects via SPI (not UART) to the XIAO ESP32-C6.
  Supports 868/915 MHz ISM bands with 2km+ range.
  
  XIAO ESP32-C6 Pinout:
  - Ultra-compact: 21 x 17.8mm form factor
  - 14-pin standard XIAO connector
  - Built-in USB-C, battery charging, WiFi 6, BLE 5.3
*/

#pragma once

// Board and variant identification
#define BOARD_MODEL "Seeed XIAO ESP32-C6 + Wio-SX1262"
#define HW_VENDOR VENDOR_SEEED
#define VARIANT_NAME "seeed_xiao_esp32c6_wio_sx1262"

// ============================================
// LORA RADIO - SX1262 via SPI
// ============================================

#define USE_SX1262

// SPI Interface (NOT UART - the WIO-SX1262 uses SPI)
#define LORA_INTERFACE_TYPE LORA_SPI

// SPI Bus: ESP32-C6 has SPI0, SPI1, SPI2
// Default SPI (SPI1/VSPI) pins on XIAO ESP32-C6:
#define LORA_SPI_NUM SPI1         // Use primary SPI bus

// SPI Signal Pins (Standard XIAO SPI)
// These match the XIAO pinout for SPI compatibility
#define LORA_SCK_PIN GPIO_NUM_8   // D8 - SPI Clock (SCLK)
#define LORA_MOSI_PIN GPIO_NUM_10 // D10 - SPI Data Out (MOSI)
#define LORA_MISO_PIN GPIO_NUM_9  // D9 - SPI Data In (MISO)
#define LORA_CS_PIN GPIO_NUM_3    // D3 - Chip Select (NSS)

// LoRa Control Pins (GPIO)
#define LORA_RESET_PIN GPIO_NUM_4   // D4 - Reset pin (active low pulse)
#define LORA_DIO1_PIN GPIO_NUM_5    // D5 - Interrupt pin (data available)
#define LORA_BUSY_PIN GPIO_NUM_6    // D6 - Busy pin (module status)

// DIO2 and DIO3 are used internally by SX1262:
// - DIO2: RF switch control (LNA on/off)
// - DIO3: TCXO voltage supply (1.8V)
// These are typically wired on the Wio-SX1262 module

// ============================================
// FREQUENCY CONFIGURATION
// ============================================

// Choose based on your region and Wio-SX1262 variant:
#define FREQ_US 915.0    // Americas (915 MHz) - Default
// #define FREQ_EU 868.0    // Europe (868 MHz)
// #define FREQ_CN 470.0    // China (470 MHz)

#define DEFAULT_FREQ FREQ_US

// ============================================
// XIAO ESP32-C6 FEATURES
// ============================================

// Wireless: Built-in WiFi 6 + Bluetooth 5.3
#define WANT_BLUETOOTH 1
#define BT_DEVICE_NAME "Meshtastic-XIAO-C6"

#define WANT_WIFI 1
#define WIFI_ENABLED_BY_DEFAULT 0  // Start disabled to save power

// Power management (battery charging on XIAO)
#define BATTERY_SENSE_PIN GPIO_NUM_A0    // Analog pin for battery voltage
#define BATTERY_SENSE_RESOLUTION ADC_RESOLUTION_12_BIT
#define BATTERY_SENSE_MULTIPLIER 2.0     // Voltage divider adjustment

// ============================================
// GPIO ASSIGNMENTS (XIAO Standard Pinout)
// ============================================

// User LED (built-in on XIAO)
#define LED_PIN GPIO_NUM_2  // LED pin (if available)
#define LED_INVERTED 1      // Active low

// User Button (if you connect one)
#define BUTTON_PIN GPIO_NUM_1
#define BUTTON_INVERTED 1   // Active low

// ============================================
// MEMORY AND STORAGE
// ============================================

// XIAO ESP32-C6 has 4MB Flash, 512KB SRAM
#define RAM_AVAILABLE RAM_SIZE_NORMAL
#define FLASH_SIZE 4194304  // 4MB

// ============================================
// DEBUGGING & LOGGING
// ============================================

#define LOG_DEBUG_ENABLED 1
#define USE_LOG_TIMESTAMPS 1

// ============================================
// BOOT CONFIGURATION
// ============================================

// ESP32-C6 boot mode pins (GPIO2, GPIO8, GPIO9 control boot behavior)
// Most are handled by ESP-IDF, but be aware:
// GPIO2 (D2) - UART download mode if pulled low at boot
// GPIO8 (D8) - Used for SPI clock, so don't use for boot strapping

// ============================================
// INITIALIZATION
// ============================================

static inline void initBoard() {
    // Set LoRa control pins as outputs
    gpio_set_direction((gpio_num_t)LORA_RESET_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)LORA_DIO1_PIN, GPIO_MODE_INPUT);
    gpio_set_direction((gpio_num_t)LORA_BUSY_PIN, GPIO_MODE_INPUT);
    
    // Reset LoRa module: pulse low for 10ms
    gpio_set_level((gpio_num_t)LORA_RESET_PIN, 0);
    vTaskDelay(10 / portTICK_PERIOD_MS);
    gpio_set_level((gpio_num_t)LORA_RESET_PIN, 1);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    
    // Set LED pin if available
    #ifdef LED_PIN
    gpio_set_direction((gpio_num_t)LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level((gpio_num_t)LED_PIN, LED_INVERTED ? 1 : 0);
    #endif
}

// ============================================
// POWER MANAGEMENT
// ============================================

#define BATTERY_TYPE BAT_LIPO
#define CHARGE_CURRENT 100  // mA

// XIAO has integrated battery charging - no external charger needed
// To use battery:
// - Solder GND and BATT terminals to 3.7V LiPo
// - USB-C provides charging
// - Built-in protection included

// ============================================
// PIN MAPPING REFERENCE
// ============================================

/*
XIAO ESP32-C6 Standard Pin Mapping (14-pin):

         [USB-C]
    ┌──────────────┐
  1 │ D0 (GPIO0)   │ - Can use for GPIO
  2 │ D1 (GPIO1)   │ - UART RX (Serial)
  3 │ D2 (GPIO2)   │ - Can use, but boot sensitive
  4 │ D3 (GPIO3)   │ - CS for SPI (LoRa chip select)
  5 │ D4 (GPIO4)   │ - LoRa RESET
  6 │ D5 (GPIO5)   │ - LoRa DIO1 (interrupt)
  7 │ D6 (GPIO6)   │ - LoRa BUSY
  8 │ D7 (GPIO7)   │ - Can use for GPIO
  9 │ D8 (GPIO8)   │ - SPI Clock (SCK)
 10 │ D9 (GPIO9)   │ - SPI MISO
 11 │ D10 (GPIO10) │ - SPI MOSI
 12 │ GND          │ - Ground
 13 │ 5V           │ - USB 5V
 14 │ 3V3          │ - 3.3V regulated
    │ A0-A3        │ - Analog inputs (on side)
    └──────────────┘

Wio-SX1262 Connection (SPI):
────────────────────────────
Wio-SX1262  ←→  XIAO ESP32-C6
─────────────────────────────
VCC         ←→  3V3
GND         ←→  GND
NSS (CS)    ←→  D3 (GPIO3)
SCK         ←→  D8 (GPIO8)
MOSI        ←→  D10 (GPIO10)
MISO        ←→  D9 (GPIO9)
RST         ←→  D4 (GPIO4)
DIO1        ←→  D5 (GPIO5)
BUSY        ←→  D6 (GPIO6)
ANT         ←→  U.FL antenna
*/

// ============================================
// SPI CONFIGURATION
// ============================================

// SPI Speed: SX1262 supports up to 10MHz
// Use 5MHz for reliability with long wires
#define LORA_SPI_SPEED 5000000  // 5 MHz

// SPI Mode: SX1262 uses mode 0
#define LORA_SPI_MODE SPI_MODE0

// ============================================
// TX POWER CONFIGURATION
// ============================================

// Wio-SX1262 max: 22 dBm (160mW)
// For Meshtastic, use lower for longer battery life
#define LORA_TX_POWER 20  // dBm (100mW) - balance power vs range

// ============================================
// ADVANCED: CUSTOM MODULES
// ============================================

// Uncomment to add GPS support (if connected to UART1):
// #define WANT_GPS 1
// #define GPS_SERIAL UART1
// #define GPS_TX_PIN GPIO_NUM_0   // D0
// #define GPS_RX_PIN GPIO_NUM_1   // D1
// #define GPS_BAUD 9600

// Uncomment to add OLED display (if using SSD1306):
// #define WANT_SCREEN 1
// #define SCREEN_TYPE SSD1306
// #define SCREEN_I2C_ADDR 0x3C
// #define SCREEN_SDA_PIN GPIO_NUM_1  // D1
// #define SCREEN_SCL_PIN GPIO_NUM_0  // D0

// ============================================
// END VARIANT DEFINITION
// ============================================

/*
HARDWARE ASSEMBLY NOTES:
────────────────────────

1. XIAO ESP32-C6 board orientation:
   - USB-C port faces outward
   - GPIO pins labeled D0-D10 on sides
   - 3V3 and GND near the USB

2. Wio-SX1262 module connection:
   - 9 pins total: VCC, GND, NSS, SCK, MOSI, MISO, RST, DIO1, BUSY
   - Typically comes as a breakout with pins
   - SPI lines MUST be short (< 5cm ideal)

3. Soldering / Assembly:
   - Solder female headers to XIAO if not pre-soldered
   - Solder male headers to Wio-SX1262 breakout
   - Use breadboard or custom PCB for connections
   - Keep SPI lines together and away from antenna

4. Antenna:
   - Attach U.FL antenna to Wio-SX1262 ANT port
   - Or connect SMA antenna via adapter
   - Keep antenna away from MCU and SPI traces

5. Battery (Optional):
   - Solder GND to battery negative (left pad, near D8 label)
   - Solder BATT to battery positive (right pad, near D5 label)
   - Use 3.7V LiPo (1000-2000 mAh recommended)
   - USB-C provides charging automatically

6. Bypass Capacitors (Recommended):
   - 100nF ceramic cap between Wio-SX1262 VCC and GND (close to module)
   - 10µF ceramic cap near XIAO 3V3 pin
   - Reduces noise, improves SPI communication

EXPECTED SERIAL OUTPUT ON BOOT:
────────────────────────────────
[00:00:00] Meshtastic 2.7.x starting
[00:00:01] XIAO ESP32-C6 board detected
[00:00:02] SPI initialized at 5MHz
[00:00:03] LoRa radio (SX1262) initialized
[00:00:04] Frequency: 915.0 MHz
[00:00:05] Mesh stack starting
[00:00:06] BLE advertising as "Meshtastic-XIAO-C6"
[00:00:07] Ready for connections

If you see errors, check:
- SPI pin connections match LORA_*_PIN defines above
- Wio-SX1262 has stable 3.3V power
- Antenna connected or module won't initialize
- Try serial monitor at 115200 baud for debug output

BUILD COMMAND:
──────────────
pio run -e seeed_xiao_esp32c6_wio_sx1262

UPLOAD COMMAND:
───────────────
pio run -e seeed_xiao_esp32c6_wio_sx1262 --target upload

MONITOR:
────────
pio device monitor -e seeed_xiao_esp32c6_wio_sx1262 -b 115200
*/
