# Workspace setup

This document explains how to create the ROS2 workspace needed to build and run the TankBot software.  It assumes that you using Ubuntu 22.04LTS and that you have run all the ROS2 setup scripts.

To setup and do the first build, run the following

```bash
cd ~/git/pipebot_4wd/setup/workspace
./setup_ws.bash
```

Note: This build can take several minutes and can overheat a RPi4B.  Make sure a fan is fitted!

Subsequent builds can be done using the the script [build.bash](build.bash).

## Base station

TODO

~~To setup the base station, run the script [setup_base_station.bash](setup_base_station.bash).~~

## TO DO

* Test and fix `release*.bash` files.
