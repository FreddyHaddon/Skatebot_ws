# Design Notes

This package publishes a single diagnostics message every second that contains the following data:

* CPU usage
* CPU temperature
* Free memory

## Research

I spent about an hour or so looking for previous work on the internet and found the following:

* <http://wiki.ros.org/pr2_computer_monitor>
* <http://wiki.ros.org/libsensors_monitor>
* <https://tier4.github.io/autoware.iv/tree/main/system/system_monitor/> Docs
* <https://github.com/autowarefoundation/autoware_ai_utilities/tree/master/system_monitor> Code for above.

The `autowarefoundation` code looks the best but is ROS 1 so I can only use it as inspiration.

## Implementation

The Autoware Foundation code uses the Diagnostic Updater method of publishing a try.  I also decided to do away with the base class approach as I on need this to work for the Raspberry Pi only.  I can always refactor later if I need to use it on another CPU.

I created a minimal package by copying most of the files from another package with no nodes, renaming as needed, checked that it built, ran and launched.  Then I added a stubbed version the `RPiMonitorNode` class and then added the `DiagnosticUpdater` code.  The first iteration had a single updater and published the CPU temperature as it was the easiest to do.  Once this worked, I started on the second updater, CPU usage.  I implemented a simple approach that I thought should work.  I implemented the third and final updater, RAM usage, and after a quick test on my PC, I tried it on the RPi.  CPU Temperature and RAM usage worked well but the CPU usage was miles out.  After some more reading, a complete rework for the CPU usage code (I had to keep track of the change between each function call) followed by testing, it finally worked.

When I started on the second updater, I had to re-factor as I was struggling to understand exactly what the update code was doing.  I split the function `CheckCPUTemperature` into two parts.  `CheckCPUTemperature` calls `ReadCPUTemperatureData` then uses `DiagnosticUpdater` functions to fill out a message that is then published.  `ReadCPUTemperatureData` is now responsible for reading the temperature from `/sys/class/thermal/thermal_zone0/temp` file.  Separation of concerns in action :-)

### References

* <https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process>
* <https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine>
