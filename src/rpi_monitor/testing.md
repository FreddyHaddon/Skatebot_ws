# Testing

These are ROS2 commands that can be used to verify the correct operation of the Raspberry Pi monitor.

## Tests

Open a terminal on the Raspberry Pi and launch the monitor program.

```bash
cd ~/pipebot_4wd_ws
. ./install/setup.bash
ros2 launch rpi_monitor monitor.launch.py
```

In a second terminal window, verify that the `/diagnostics` topic is present using:

```bash
$ cd ~/pipebot_4wd_ws
$ . ./install/setup.bash
$ ros2 topic list
/diagnostics
/parameter_events
/rosout
```

Echo the `/diagnostics` topic and verify that you see something like this:

```bash
$ ros2 topic echo /diagnostics
header:
  stamp:
    sec: 1685286347
    nanosec: 31482782
  frame_id: ''
status:
- level: "\0"
  name: 'rpi_monitor: CPU Temperature'
  message: OK
  hardware_id: RPI Monitor
  values:
  - key: CPU 0
    value: 35.5 DegC
- level: "\0"
  name: 'rpi_monitor: CPU Usage'
  message: OK
  hardware_id: RPI Monitor
  values:
  - key: Number CPUs
    value: '4'
  - key: All CPUs
    value: 6.00%
- level: "\0"
  name: 'rpi_monitor: Memory Usage'
  message: OK
  hardware_id: RPI Monitor
  values:
  - key: RAM used
    value: 11.54%
---
```

Verify that the following items are present:

* CPU temperature.
* CPU usage.
* RAM usage.

Also verify that the reported values are correct. `top` can be used for the CPU and memory usage and `sensors` (installed using `sudo apt install lm-sensors`) for the temperature.
