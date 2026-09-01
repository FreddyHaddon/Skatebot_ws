# Communications Subsystem Specification

This document defines the interfaces that the communication subsystem of a given Pipebot will have to other subsystems on the same Pipebot. It **does not** (yet) define how the Pipebot will interface with other Pipebots and external networks and servers.

These interfaces will use ROS2 messages and services. Where possible, the built-in [common interface ones](https://github.com/ros2/common_interfaces/tree/foxy) will be used, however some of the communication subsystem functionality will require custom messages and services.

<a id="assumptions"></a>
This document was prepared with the following assumptions in mind:

- The communication subsystem is not transparent to ROS2. That is, the communication subsystem does not appear as a network interface on the Raspberry Pi that can be used for DDS auto-discovery.
- Furthermore, there is a translation between ROS2 messages to a different communication protocol, such as [CoAP](https://en.wikipedia.org/wiki/Constrained_Application_Protocol).
- Data is transmitted and received asynchronously, and there aren't any strict requirements for latency yet.
- The communication subsystem keeps tracks and decides which interface is best suited for a given transmission, e.g. in-pipe mesh network, emergency pipe-to-ground link, etc.

Every effort is made to make this specification as broad as possible and applicable to both water and sewer networks. However changes can and likely will happen once work on water pipes begins.

***NOTE 08 Feb 2021: The [Receive data](#receive-data) service has been*** removed ***and the way data from the outside world is relayed to other subsystem has changed. From now on, any such data will be directly published using the [Data received](#data-received) topic.***

***NOTE 11 Feb 2021:*** ALL ***Theme 6 node and topic names have been reworked to use the `comms` namespace. For example, instead of [`comms_connected`](#connected), you should now use `comms/connected`.***

## Table of Contents

- [Communications Subsystem Specification](#communications-subsystem-specification)
  - [Table of Contents](#table-of-contents)
  - [Interface message definitions](#interface-message-definitions)
    - [Subscriptions](#subscriptions)
      - [Battery status](#battery-status)
    - [Publishers](#publishers)
      - [Connected](#connected)
      - [RSSI](#rssi)
      - [Self-test result](#self-test-result)
      - [Data received](#data-received)
    - [Servers](#servers)
      - [Send data](#send-data)
      - [Receive data](#receive-data)
      - [Get IP](#get-ip)
      - [Get Network ID](#get-network-id)
    - [Parameters](#parameters)
      - [DEBUG](#debug)
      - [PUBLISH_PERIOD](#publish_period)
      - [DATA_RATE_BPS](#data_rate_bps)

## Interface message definitions

The communication subsystem will likely have three types of interfaces - Publishers, Subscribers, and Servers. Please note that these definitions are a work in progress and as such are subject to changes.

### Subscriptions

#### Battery status

Currently, the only topic that the communication subsystem will subscribe to is the [Battery status](./platform-specification.md#battery-status) one.

The node name for the subscriber is `comms/battery_status_subscriber`.

*Rationale*: This is of interest to the communication subsystem in case an emergency distress signal needs to be sent out. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

### Publishers

The communication subsystem will make the following information available to other subsystems. Rate of publishing is TBC, subject to overall dynamics of the robot, e.g. how quickly it moves, etc.

#### Connected

A bare bones way for other subsystems to find out if there is an active wireless connection to any network. More detailed information, if needed, can be obtained through the [RSSI](#rssi) and [Self-test result](#self-test-result) topics.

| Parameter | Value |
|-|-|
| Topic Name | comms/connected |
| Node Name | comms/connected_publisher |
| Message type | Custom - [draft](../msg/Connected.msg) |
| QoS | Default |
| Rate | TBD |

The custom message will be a simple wrapper for the ROS2 \<bool\> data type. However, following [this advice](https://github.com/ros2/common_interfaces/tree/master/std_msgs#primitive-types) it's better to define custom messages with clear semantic meaning.

| Name | Data type |
|-|-|
| connected_status | \<bool\> |

*Rationale*: Requested during a Sprint 3 discussion as a quick confidence and/or debugging check. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### RSSI

RSSI stands for "Received Signal Strength Indicator". The publisher details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/rssi |
| Node Name | comms/rssi_publisher |
| Message type | Custom - [draft](../msg/Rssi.msg) |
| QoS | Default |
| Rate | TBD |

The message type is TBD, but will likely contain the following fields:

| Name | Data Type |
|-|-|
| network_id | \<string\> or \<uint8\> |
| rssi_value | \<float32\> |
| rssi_units | \<string\> or \<uint8\> |

*Rationale*: This should help the Pipebot control algorithm to always keep the Pipebot within communications range. It is possible for the Pipebot to have several communications interfaces with different RSSI values, and messages for these will be published separately. The `Units` field is there to accommodate different ways of reporting the RSSI, e.g. `0-100` scale, absolute power level, etc. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### Self-test result

The communication subsystem will periodically perform self-tests to check connectivity, network status, latency, etc. The publisher details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/self_test_result |
| Node Name | comms/selftest_publisher |
| Message type | [DiagnosticStatus](https://github.com/ros2/common_interfaces/blob/master/diagnostic_msgs/msg/DiagnosticStatus.msg) |
| QoS | Default |
| Rate | TBD |

*Rationale*: This information can be used together with the [RSSI](#rssi) one to inform the Pipebot control algorithm of any potential issues with the network connectivity, e.g. going out of range, internal subsystem errors, higher than average packet loss, etc. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### Data received

The publisher details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/data_received |
| Node Name | comms/data_received_publisher |
| Message type | Custom - [draft](../msg/DataReceived.msg) |
| QoS | Custom - see below |
| Rate | On-event |

The message type is TBD, but will likely contain the following fields:

| Name | Data Type |
|-|-|
| dest_subsystem | \<string\> or \<uint8\> |
| source_id | \<string\> or \<uint16\> |
| payload | \<byte[]\> |

*Rationale*: Due to the asynchronous nature of communication over a network, the communication subsystem cannot know in advance when and what data will arrive for what other subsystem. Once such data arrives, it will be processed by the communication subsystem and published on this topic. At this stage, all data will be published as an array of bytes, and it will be up to the receiving subsystem to interpret those correctly.

The QoS profile for this publisher is defined explicitly to ensure consistent and stable behaviour in case future ROS2 releases change the default one. The parameters are as follows:

- History Policy = KEEP_LAST
- Depth = 10
- Reliability Policy = RELIABLE
- Durability Policy = VOLATILE

More info about these can be found [here](https://index.ros.org/doc/ros2/Concepts/About-Quality-of-Service-Settings/#qos-policies). <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

### Servers

Most of the functionality of the communication subsystem will be implemented via services. This is based on the [assumptions](#assumptions) at the start of this specification.

#### Send data

The service details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/send_data |
| Node Name | comms/send_data_server |
| Message type | Custom - [draft](../srv/SendData.srv) |
| QoS | Default |

The message type is TBD, but will likely contain the following fields:

- Request:

| Name | Data Type |
|-|-|
| source_id | \<string\> or \<uint16\> |
| source_ref | \<uint16\> |
| destination_id | \<string\> or \<uint16\> or \<byte[16]\> |
| payload | \<byte[]\> |

- Response:

| Name | Data Type |
|-|-|
| Name | Data Type |
| status_code | \<string\> or \<uint16\> |

*Rationale*: This covers the scenario where a particular subsystem wants to send some data to the outside world. The `destination_id` can either be a descriptive name, e.g. "Blockage database" which will then get translated to an IP address, it can be a numeric code, or it can be a straight-up IP address. The `source_ref` will be used when acknowledging that data has been sent, and/or when a response has arrived back.

The response itself will be a status code, such as "OK", "ACK Received", "ACK Not Received", etc. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### Receive data

This service has been removed as of 08 February 2021. Other subsystems are instead asked to subscribe to the [Data received](#data-received) topic. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### Get IP

The service details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/get_ip |
| Node Name | comms/get_ip_server |
| Message type | [Trigger](https://github.com/ros2/common_interfaces/blob/master/std_srvs/srv/Trigger.srv) |
| QoS | Default |

*Rationale*: This is one potentially useful information that the communication subsystem can provide upon request. Even though the response of the `Trigger` service is of type \<string\> it can still be used to return the current IP address of the network interface. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

#### Get Network ID

The service details are as follows:

| Parameter | Value |
|-|-|
| Topic Name | comms/get_ntwk_id |
| Node Name | comms/get_ntwk_id_server |
| Message type | [Trigger](https://github.com/ros2/common_interfaces/blob/master/std_srvs/srv/Trigger.srv) |
| QoS | Default |

*Rationale*: This is one potentially useful information that the communication subsystem can provide upon request. Even though the response of the `Trigger` service is of type \<string\> it can still be used to return the current network ID. <sup><sub>[Back to TOC](#table-of-contents)</sub></sup>

### Parameters

These have the goal to make the communication subsystem code more portable and more flexible.

#### DEBUG

Every node has this parameter, used to turn more verbose info messages on and off. The type of this is `BOOL`.

Due to how the [Data Received](#data-received) publisher operates, i.e. it manually publishes data rather than continuously spinning the node, the DEBUG parameter for it can only be changed at start time.

#### PUBLISH_PERIOD

This is an additional parameter for the three regular publishers, i.e. [RSSI](#rssi), [Connected](#connected), and [Self-Test Result](#self-test-result). It is a `DOUBLE` parameter, with units of `seconds`.

While this cannot be used to change the period while the nodes are running, it is useful for quickly experimenting with different values through launch files rather than having to re-build the entire package.

#### DATA_RATE_BPS

This is specific to the [Send Data](#send-data) service. It is an `INTEGER` parameter, with units of `bits per second`.

Useful when simulating different wireless connectivity options to get an estimate for latency and time required to send a particular amount of data.
<sup><sub>[Back to TOC](#table-of-contents)</sub></sup>
