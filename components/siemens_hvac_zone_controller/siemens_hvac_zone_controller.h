#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/valve/valve.h"
#include <vector>

namespace esphome {
namespace siemens_hvac_zone_controller {

class SiemensHVACZoneController;

class SiemensHVACZoneValve : public valve::Valve {
 public:
  SiemensHVACZoneValve(SiemensHVACZoneController *parent, uint8_t zone_idx) 
      : parent_(parent), zone_idx_(zone_idx) {}

 protected:
  valve::ValveTraits get_traits() override {
    auto traits = valve::ValveTraits();
    traits.set_supports_position(false);
    traits.set_supports_stop(false);
    return traits;
  }

  void control(const valve::ValveCall &call) override;

  SiemensHVACZoneController *parent_;
  uint8_t zone_idx_;
};

class SiemensHVACZoneController : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void send_zone_command(uint8_t zone_idx);

  void set_zone_1(valve::Valve *v) { zones_[0] = v; }
  void set_zone_2(valve::Valve *v) { zones_[1] = v; }
  void set_zone_3(valve::Valve *v) { zones_[2] = v; }
  void set_zone_4(valve::Valve *v) { zones_[3] = v; }
  void set_zone_5(valve::Valve *v) { zones_[4] = v; }
  void set_zone_6(valve::Valve *v) { zones_[5] = v; }

 protected:
  std::vector<uint8_t> rx_buffer_;
  uint8_t current_zone_mask_{0x08}; // Your exact original default init state (Zone 4 ON)
  valve::Valve *zones_[6]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
