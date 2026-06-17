#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/valve/valve.h" // 1. Change switch to valve header
#include <vector>

namespace esphome {
namespace siemens_hvac_zone_controller {

class SiemensHVACZoneController : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  // 2. Change setters to point to valve pointers
  void set_zone_1(valve::Valve *vl) { zone_1_ = vl; }
  void set_zone_2(valve::Valve *vl) { zone_2_ = vl; }
  void set_zone_3(valve::Valve *vl) { zone_3_ = vl; }
  void set_zone_4(valve::Valve *vl) { zone_4_ = vl; }
  void set_zone_5(valve::Valve *vl) { zone_5_ = vl; }
  void set_zone_6(valve::Valve *vl) { zone_6_ = vl; }

  void send_button_press(uint8_t button_idx);

 protected:
  std::vector<uint8_t> rx_buffer_;
  uint8_t current_zone_mask_{0x20};

  // 3. Update storage types to valve
  valve::Valve *zone_1_{nullptr};
  valve::Valve *zone_2_{nullptr};
  valve::Valve *zone_3_{nullptr};
  valve::Valve *zone_4_{nullptr};
  valve::Valve *zone_5_{nullptr};
  valve::Valve *zone_6_{nullptr};
};

// Intermediary class passing frontend UI valve toggles into the core hardware driver
class SiemensHVACZoneValve : public valve::Valve {
 public:
  SiemensHVACZoneValve(SiemensHVACZoneController *parent, uint8_t button_idx) 
      : parent_(parent), button_idx_(button_idx) {}

 protected:
  // 1. FIX: Implement get_traits() to tell ESPHome this is a standard open/close damper
  valve::ValveTraits get_traits() override {
    auto traits = valve::ValveTraits();
    traits.set_supports_position(false); // We don't support partial position percentages (e.g. 45% open)
    traits.set_supports_stop(false);     // We don't support intermediate stop commands
    return traits;
  }

  // Whenever an Open or Close command comes through, trigger our bus toggle mechanism
  void control(const valve::ValveCall &call) override {
    parent_->send_button_press(button_idx_);
  }

  SiemensHVACZoneController *parent_;
  uint8_t button_idx_;
};

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
