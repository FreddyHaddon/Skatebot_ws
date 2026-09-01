# Wiring for the SkateBot

## Raspberry Pi Protoboard

### I2C connector for the IMU

| I2C connector | RPi Pin | GPIO |
|---|---|---|
| 1 | GND | |
| 2 | 3V3 | |
| 3 (white) | 27 | SDA0 |
| 4 (green) | 28 | SCL0 |

NOTE: To enable this on the Pi, you need to modify add the following to the `/boot/firmware/config.txt` file:

```text
dtparam=i2c_vc=on
```

Verify that the IMU is detected using `i2cdetect -y 0`.

## Acoustic board

This was a prototype board.

This is the view from the underside of the Vero-board.

```text
     I2S AMP            CABLE                     MIC1         MIC2
                       BLACK(3V3)
VIN  *-------------------*---------------------*---*------------*  3V3
                       WHITE(GND)              |
GND  *-------------------*---------------------|---*--------*---*  GND
                       GREY(BLCK)              |            |
NC   *-------X-*---------*---------------------|---*--------|---*  BLCK
        |------|       PURPLE(LRCL)            |            |
NC   *--|----X-----*-----*----------*----X-*---|---*--------|---*  DOUT
        |          |   BLUE(DOUT)   |  |---|   |            |
DIN  *--|-*--X--- -|-----*----------|--*-X--*--|---*--------|---*  LRCL
        | |-----|  |   GREEN(DIN)   |-------|  |            |
BCLK *--*----X--*--|-----*---------------X-----*---*-----X--*---*  SEL
           |--------
LRCL *-----*-X

```

The connector looks like this

```text
        _     _
     __| |___| |__
    | 1 2 3 4 5 6 |
    |_____________|

```

and was wired as follows:

| Pin | Connected to | Cable Colour |
|---|---|---|
| 1 | GPIO21 | GREEN |
| 2 | GPIO20 | BLUE |
| 3 | GPIO19 | PURPLE |
| 4 | GPIO18 | GREY |
| 5 | 3.3V | BLACK |
| 6 | Ground | WHITE |

NOTE: The black and white wires of the cable are reversed at the connector end.

### Testing

If the software has been set up correctly, you should be able to play an audio file using these commands:

```bash
cd ~/pipebots_4wd_ws
aplay -t wav ./src/sprint3-t2-ros2/sensing_subsystem/sensing_subsystem/acoustic_data/chirp_wav_16k_5s.wav
```

After a short delay, you should hear a continuous tone that ramps up in pitch.
