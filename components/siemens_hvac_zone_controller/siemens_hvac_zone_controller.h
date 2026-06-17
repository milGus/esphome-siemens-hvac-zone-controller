#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/valve/valve.h"
#include <vector>

namespace esphome {
namespace siemens_hvac_zone_controller {

class SiemensHVACZoneController;

// Custom Valve implementation that behaves exactly like a native Template Valve
class SiemensHVACZoneValve : public valve::Valve, public Component {
 public:
  SiemensHVACZoneValve(SiemensHVACZoneController *parent, uint8_t zone_idx) 
      : parent_(parent), zone_idx_(zone_idx) {}

  void setup() override;
  void dump_config() override;
  
  // This is the missing link: We force the component to report absolute states
  valve::ValveType get_valve_type() { return valve::VALVE_TYPE_OPEN_CLOSE; }

 protected:
  void control(const valve::ValveCall &call) override;

  valve::ValveTraits get_traits() override {
    auto traits = valve::ValveTraits();
    traits.set_supports_position(false);
    traits.set_supports_stop(false);
    return traits;
  }

  SiemensHVACZoneController *parent_;
  uint8_t zone_idx_;
};

class SiemensHVACZoneController : public Component, public uart::UARTDevice {
 public:
  void setup() override {}
  void loop() override;
  void dump_config() override;

  void send_zone_command(uint8_t zone_idx);
  void set_lock_timeout(uint32_t duration_ms) { this->lock_timeout_ms_ = millis() + duration_ms; }

  void set_zone_1(SiemensHVACZoneValve *v) { zones_[0] = v; }
  void set_zone_2(SiemensHVACZoneValve *v) { zones_[1] = v; }
  void set_zone_3(SiemensHVACZoneValve *v) { zones_[2] = v; }
  void set_zone_4(SiemensHVACZoneValve *v) { zones_[3] = v; }
  void set_zone_5(SiemensHVACZoneValve *v) { zones_[4] = v; }
  void set_zone_6(SiemensHVACZoneValve *v) { zones_[5] = v; }

 protected:
  std::vector<uint8_t> rx_buffer_;
  uint8_t current_zone_mask_{0x08}; 
  uint32_t lock_timeout_ms_{0};
  SiemensHVACZoneValve *zones_[6]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
};

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
