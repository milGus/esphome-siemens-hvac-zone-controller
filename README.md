# ESPHome Siemens HVAC Zone Controller Component

An external component for ESPHome that allows you to cleanly interface and control a 6-zone Siemens HVAC zone control system over a custom 9600-baud serial bus.

Unlike traditional implementations that try to "hijack" or act as the main master bus controller, this component acts as a **passive sub-controller (Extra Pad)**. It passively listens to the network to keep Home Assistant updated instantly and mimics secondary keypad button presses only when a state toggle is requested.

## Features

- **True Local Fail-Safe:** If Home Assistant goes offline, your physical wall controller continues to work perfectly without error codes or interruptions.
- **Passive State Syncing:** Automatically monitors the active system mask broadcast by the Master panel to update your Home Assistant entities instantly.
- **Zero Bus Contention:** Does not blast persistent heartbeat loops; only writes to the UART lines during a physical or virtual toggle event.
- **Native ESPHome Entities:** Exposes the zones as clean, individual `switch` entities without requiring complex YAML lambdas.

---

## Repository Structure

To use this component, ensure your repository layout matches the official ESPHome external component directory structure:

```text
esphome-siemens-hvac-zone-controller/
├── components/
│   └── siemens_hvac_zone_controller/
│       ├── __init__.py
│       ├── switch.py
│       ├── siemens_hvac_zone_controller.h
│       └── siemens_hvac_zone_controller.cpp
└── README.md
