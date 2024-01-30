# Fuel tank DroneCAN application

> Developers docs: [DroneCAN template application](https://github.com/RaccoonlabDev/mini_v2_node/wiki/dronecan).

### 1. Key features

- You can run the software in SITL mode (don't need to have any hardware),

### 2. Brief hardware info

| View | Drw |
| ---- | ------ |
| <img src="assets/view.png" alt="drawing" height="200"> | <img src="assets/drw.png" alt="drawing" height="200"> |

### 3. Application interface

The node measures fuel tank level with [AS5600](https://github.com/ZilantRobotics/libperiph/tree/master/sensors/encoder) and sends [FuelTankStatus](https://dronecan.github.io/Specification/7._List_of_standard_data_types/#fueltankstatus).

The node measure fuel level with 10 Hz rate and publish data with 1 Hz rate.

The node has the following parameters:
| №  | Register name           | Description |
| -- | ----------------------- | ----------- |
|  1 | uavcan.node.id          | Defines a node-ID. Allowed values [0,127]. |
|  2 | system.name             | Defines custom node name. If empty, the node will use the default name. |
|  3 | fuel.empty_tank_enc_deg | Defines an angle that corresponds to the empty fuel tank [degrees]. |
|  4 | fuel.full_tank_enc_deg  | Defines an angle that corresponds to the full fuel tank [degrees]. |
|  5 | fuel.volume             | Defines a volume of the fuel tank [cm3]. |
|  6 | fuel.tank_id            | Defines an id of the fuel tank. |

### 4. CAN Messages
- Default application messages: FuelTankStatus data
![Alt text](/assets/dronecan_gui_tool.png "Normal application messages")
- If there is a problem with I2C connection to AS5600 or the encoder is not connected, the node is sending the Log message **NO_DATA** once a second.
![Alt text](/assets/dronecan_gui_tool_no_data.png "If there is a problem with I2C connection to AS5600 or the encoder is not connected")

For the application builded with *DEBUG* flag:
