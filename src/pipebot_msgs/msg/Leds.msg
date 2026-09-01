# LEDs message definition.
# These enumerated values must be kept in step with the values defined on the robot.
# NOTE: If the implementation does not support a requested value, the value is ignored.
# This allows the same software to be used on different hardware implementations.

# These LEDs may or may not be implemented.
int8 HEAD_LIGHT=0
int8 REVERSE_LIGHT=1
int8 LEFT_SIDE_LIGHT=2
int8 RIGHT_SIDE_LIGHT=3
int8 SIDE_LIGHTS=4
int8 DIAGNOSTIC_LIGHT=5
int8 ALL_LIGHTS=6

# LED flash modes
int8 FLASH_OFF=0
int8 FLASH_ON=1
int8 FLASH_ERROR=2
int8 FLASH_1HZ=3
int8 FLASH_2HZ=4
int8 FLASH_5HZ=5
int8 FLASH_10HZ=6

# LED colours
# (names based on https://en.wikipedia.org/wiki/Web_colors#Basic_colors)
# Colours illuminated for RGB or RGBW LEDS indicated in comments
int8 COLOUR_WHITE=0   # (1 1 1) | (0 0 0 1)
int8 COLOUR_RED=1     # (1 0 0) | (1 0 0 0)
int8 COLOUR_GREEN=2   # (0 1 0) | (0 1 0 0)
int8 COLOUR_BLUE=3    # (0 0 1) | (0 0 1 0)
int8 COLOUR_AQUA=4    # (0 1 1) | (0 1 1 0)
int8 COLOUR_YELLOW=5  # (1 1 0) | (1 1 0 0)
int8 COLOUR_FUCHSIA=6 # (1 0 1) | (1 0 1 0)

# The message parameters.
int8 led  # Name of led to be controlled. Use
int8 flash_mode
int8 colour
uint8 brightness   # Brightness 0 (min)- 255 (max)
