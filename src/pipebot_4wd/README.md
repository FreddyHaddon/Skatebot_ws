# Pipebot 4 with 4 wheel drive

This is the 4th different type of robot built for the Pipebots project.  Its main purpose is to provide a reliable platform to perform scientific experiments on in a 300mm pipe.

The first version of this series of robots is called SkateBot and it looks like this.

![SkateBot Picture](SkateBotInPipeSmall.jpg)

These are the [setup instructions](setup/setup.md).

[This document](wiring.md) records some of the wiring used.

This is a very high level package diagram of the robot.  Most of the topics and actions are not shown otherwise it would be very complicated to understand!

![SkateBot High Level Package Diagram](SkateBotPackageDiagram.png)

This is a very high level package diagram of the base station.  The base station is normally a laptop and listens to all of the ROS 2 messages in the system, so no message flows are shown.

![Base Station High Level Package Diagram](BaseSationPackageDiagram.png)

[This document](wiring.md) records some of the wiring used.

NOTE: The original diagrams are on One Drive here: `TEAM - ESPRC Pipebots > Documents > General > SkateBots`.

A basic simulation in Gazebo has been created.  Start using `ros2 launch pipebot_4wd simulation.launch.py`.

[This document](diagnostics.md) describes how the diagnostics work on this robot.

## Acknowledgments

This work is supported by the UK's Engineering and Physical Sciences Research Council (EPSRC) Programme Grant EP/S016813/1

© 2023-2024, University of Leeds.

The authors, A. Blight & L. Mudrich, have asserted their moral rights.
