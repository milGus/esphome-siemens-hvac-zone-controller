# Using the Siemens HVAC Zone Controller Component

To integrate this component into your ESPHome node, you will need to reference this GitHub repository as an external source, configure your UART bus, and define your zone entities.

---

## 1. Quick Start YAML Template

Copy and paste this configuration profile into your device's configuration file (`.yaml`). Replace `YOUR_GITHUB_USERNAME` with your actual GitHub username.

```yaml
# ---------------------------------------------------------
# 1. Pull the Custom Component from GitHub
# ---------------------------------------------------------
external_components:
  - source:
      type: git
      url: [https://github.com/YOUR_GITHUB_USERNAME/esphome-siemens-hvac-zone-controller](https://github.com/YOUR_GITHUB_USERNAME/esphome-siemens-hvac-zone-controller)
    components: [ siemens_hvac_zone_controller ]

# ---------------------------------------------------------
# 2. Configure Hardware Serial Communication (UART)
# ---------------------------------------------------------
uart:
  id: bus_uart
  rx_pin: GPIO35
  tx_pin: 
    number: GPIO19
    inverted: true
  baud_rate: 9600

# ---------------------------------------------------------
# 3. Instantiate the Core Component Hub
# ---------------------------------------------------------
siemens_hvac_zone_controller:
  id: siemens_hub
  uart_id: bus_uart

# ---------------------------------------------------------
# 4. Map the Zone Switches to Home Assistant Toggles
# ---------------------------------------------------------
switch:
  - platform: siemens_hvac_zone_controller
    name: "Zone 1 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 1

  - platform: siemens_hvac_zone_controller
    name: "Zone 2 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 2

  - platform: siemens_hvac_zone_controller
    name: "Zone 3 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 3

  - platform: siemens_hvac_zone_controller
    name: "Zone 4 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 4

  - platform: siemens_hvac_zone_controller
    name: "Zone 5 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 5

  - platform: siemens_hvac_zone_controller
    name: "Zone 6 Control"
    siemens_hvac_zone_controller_id: siemens_hub
    zone_number: 6
