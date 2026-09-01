# Robot diagnostics

This robot is quite complex so needs to be monitored so we can find out what went wrong.  ROS provides a several tools for doing this, but there was no overview documentation that I could find that explain how to use the tools.  This document aims to show how to use the standard tools and related software.

## Overview

The following diagram shows how the various diagnostics packages are intended to be used.

![ROS 2 Diagnostics Overview](ROS2Diagnostics.png)

Each ROS 2 package can publish on the `/diagnostics` topic.  When I first saw this, I was concerned that many packages publishing on the same topic would confuse the system but it works (I tested it just to be sure).  The diagnostic messages can be used as is or filtered through the Aggregator that groups various messages together before publishing them on the topic `/diagnostics_agg`.  The `/diagnostics_agg` messages can then be shown on in `RQt` using the Robot Monitor plugin.

NOTE: The original diagram are on One Drive here: `TEAM - ESPRC Pipebots > Documents > General > SkateBots`.

## Diagnostics publishers

Each package publishes information on the `/diagnostics` topic using the [DiagnosticArray.msg](https://docs.ros2.org/galactic/api/diagnostic_msgs/msg/DiagnosticArray.html).  Each `DiagnosticArray` message may contain zero or more [key value pairs](https://docs.ros2.org/galactic/api/diagnostic_msgs/msg/KeyValue.html).  An example of these messages being used directly can be found in [here](https://github.com/pipebots/dynamixel-driver/blob/main/dynamixel-driver/src/diagnostics_node.cpp).  These messages are installed by default but are in the package `ros-humble-diagnostic-msgs`.

The next part of the puzzle is the `DiagnosticUpdater` package that needs to be installed using `sudo apt install ros-humble-diagnostic-updater`.  This package is intended to provide a simpler way to implement a diagnostic publisher.  The C++ version took a couple of hours to understand how to use it, and after that it was straightforward to use.  In my opinion, the extra effort needed to use the `DiagnosticUpdater` for C++ is not worth the effort as the `DiagnosticArray` message works in the same way as every other ROS 2 message so is easier than having to learn a new way of doing things.

## Diagnostic Aggregator and

Getting the Diagnostic Aggregator to work was easy.  Install using `sudo apt install ros-humble-diagnostic-aggregator` and run the demo using `ros2 launch diagnostic_aggregator example.launch.py`.  Echo the topic to see what is going on.

At this point, I also installed the Robot Monitor plugin using `sudo apt install ros-humble-rqt-robot-monitor`, started `RQt`, selected the option `Plugins > Robot Tools > Robot Monitor` that along with the aggregator and you can see some nice output.

The aggregation is controlled by a `yaml` file so I needed to create one of those next.

## Customising for Skatebot

For the Skatebot, at the time of writing, there are only two sources of diagnostic information, the dynamixels and the Raspberry Pi.  I created a new [launch file](pipebot_4wd/launch/diagnostics_only.launch.py), added the two packages plus the aggregator package and fired it up to see what happened.  The packages all started but the aggregator needed an input file.

Then I created an analyser file and a launch file to start it up.  After a bit of trial and error, I ended up with this very simple file:

```yaml
analyzers:
  ros__parameters:
    path: Aggregation
    rpi_monitor:
      type: diagnostic_aggregator/GenericAnalyzer
      path: CPU
      contains: [ 'rpi' ]
    servos:
      type: diagnostic_aggregator/GenericAnalyzer
      path: Servos
      contains: [ 'Dynamixel', 'Connected' ]
```

This resulted in the following output:

![Robot Monitor Output](RobotMonitorOutput.png)

From this, it is clear that the value of `path` is the display name for the aggregation, and any messages with `rpi` in them are shown under this heading.  The pattern matching is done on the `name` field of the diagnostics messages.

As Robot Monitor shows OK for everything, this is enough for now, but I've not tested with any failures yet, so I might be back!

## References

* <http://wiki.ros.org/diagnostic_aggregator>
* <http://wiki.ros.org/diagnostics/Tutorials/Using%20the%20Robot%20Monitor>*
