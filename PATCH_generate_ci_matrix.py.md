"""
Patch for bin/generate_ci_matrix.py

This patch adds support for building the Seeed XIAO ESP32-C6 + Wio-SX1262 variant
in GitHub Actions CI. 

INSTRUCTIONS:
=============
1. In your Meshtastic firmware fork, find: bin/generate_ci_matrix.py
2. Look for the section with variant definitions or where ESP32 variants are listed
3. Add the following entries to ensure your variant is included in the CI matrix

The variant will automatically be picked up by the CI system because:
- It follows the standard variants/ directory structure
- It has platformio.ini in variants/esp32c6/seeed_xiao_wio_sx1262/
- The generate_ci_matrix.py script scans all platformio.ini files
"""

# ==============================================================================
# ADD THIS SECTION to bin/generate_ci_matrix.py
# ==============================================================================

# Find the section that defines board_level metadata (usually around line 45-72)
# Add this entry to the variant_configs or similar dict:

"""
"esp32c6": {
    "boards": [
        {
            "name": "seeed_xiao_wio_sx1262",
            "board_level": 0,  # 0 = basic build, 1 = full CI, 2 = special
            "description": "Seeed XIAO ESP32-C6 + Wio-SX1262",
        },
        # ... other ESP32-C6 variants
    ],
},
"""

# OR if the script uses a different format, add to variant list:

"""
variant_list = [
    # ... existing variants
    ("esp32c6", "seeed_xiao_wio_sx1262", 0),  # (arch, board, board_level)
]
"""

# The key is that:
# - Arch/Platform: "esp32c6"
# - Board/Variant: "seeed_xiao_wio_sx1262"
# - board_level: 0 (minimum) means it builds but won't appear in releases
#                1+ means full CI treatment (appears in releases/web flasher)

# ==============================================================================
# SIMPLER APPROACH: Create the platformio.ini sections properly
# ==============================================================================

# The generate_ci_matrix.py script reads from platformio.ini by scanning:
# 1. [env:variant_name] sections where variant_name = platform_board_name
# 2. Looks for board_level metadata in each environment

# Make sure your variants/esp32c6/seeed_xiao_wio_sx1262/platformio.ini has:
[env:seeed_xiao_wio_sx1262]
# ... your build flags ...
board_level = 0  # This is the KEY line - tells CI to build it!

# If you want it to appear in releases/web flasher, change to:
# board_level = 1

# ==============================================================================
# MINIMAL FIX: Just ensure platformio.ini in your variant has this line
# ==============================================================================

# The actual minimal change needed is in your variant's platformio.ini file.
# Make sure the [env:seeed_xiao_wio_sx1262] section includes:
#
#   [env:seeed_xiao_wio_sx1262]
#   extends = esp32c6_base
#   board_level = 0
#   # ... rest of config
#
# board_level is what tells the CI matrix generator to include it!
