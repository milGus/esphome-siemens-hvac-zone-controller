#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/switch/switch.h"
#include <vector>

namespace esphome {
namespace siemens_hvac_zone_controller {

class SiemensHVACZoneController : public Component, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  // Setters for the 6 tracking switches
  void set_zone_1(switch_::Switch *sw) { zone_1_ = sw; }
  void set_zone_2(switch_::Switch *sw) { zone_2_ = sw; }
  void set_zone_3(switch_::Switch *sw) { zone_3_ = sw; }
  void set_zone_4(switch_::Switch *sw) { zone_4_ = sw; }
  void set_zone_5(switch_::Switch *sw) { zone_5_ = sw; }
  void set_zone_6(switch_::Switch *sw) { zone_6_ = sw; }

  // Sends the transient toggle request onto the bus
  void send_button_press(uint8_t button_idx);

 protected:
  std::vector<uint8_t> rx_buffer_;
  uint8_t current_zone_mask_{0x20};

  switch_::Switch *zone_1_{nullptr};
  switch_::Switch *zone_2_{nullptr};
  switch_::Switch *zone_3_{nullptr};
  switch_::Switch *zone_4_{nullptr};
  switch_::Switch *zone_5_{nullptr};
  switch_::Switch *zone_6_{nullptr};
};

// Intermediary class passing frontend UI switch toggles into the core hardware driver
class SiemensHVACZoneSwitch : public switch_::Switch {
 public:
  SiemensHVACZoneSwitch(SiemensHVACZoneController *parent, uint8_t button_idx) 
      : parent_(parent), button_idx_(button_idx) {}
 protected:
  void write_state(bool state) override {
    parent_->send_button_press(button_idx_);
  }
  SiemensHVACZoneController *parent_;
  uint8_t button_idx_;
};

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
