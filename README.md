# MQTT servo control

Read interval from MQTT servo and move servo according to the value.
Uses smart servo from MemeRobotics.

## 1. mqtt_client

Reads interval from MQTT server and print it.

## 2. servo_ctrl

Reads interval in float syntax in range (0, 10] from stdin, then maps it to servo position.

## How To Build

```SHELL
cd servo_ctrl
mkdir build
cd build
cmake -G"Unix Makefiles" ..
make
```

## 3. Run

```SHELL
python mqtt_client.py|servo_ctrl
```
