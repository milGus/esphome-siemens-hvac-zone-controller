#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone_controller";

void SiemensHVACZoneController::setup() {
  // Setup logic remains untouched
}

// FIX: Add the missing dump_config definition to satisfy the linker
void SiemensHVACZoneController::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Controller");
}

void SiemensHVACZoneController::loop() {
  while (this->available()) {
    // ... rest of your loop code remains exactly the same ...
void SiemensHVACZoneController::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    this->rx_buffer_.push_back(byte);

    // Keep your exact packet parsing frame checking logic:
    if (this->rx_buffer_.size() >= 5) {
      if (this->rx_buffer_[0] == 0x02 && this->rx_buffer_.back() == 0x03) {
        uint8_t received_mask = this->rx_buffer_[2];
        uint8_t checksum = this->rx_buffer_[3];

        if (checksum == (this->rx_buffer_[0] ^ this->rx_buffer_[1] ^ received_mask)) {
          
          // FIX: Compare and update using your actual parsed 'received_mask' variable
          if (received_mask != current_zone_mask_) {
            current_zone_mask_ = received_mask;

            if (zone_1_ != nullptr) zone_1_->publish_state((current_zone_mask_ & 0x01) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (zone_2_ != nullptr) zone_2_->publish_state((current_zone_mask_ & 0x02) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (zone_3_ != nullptr) zone_3_->publish_state((current_zone_mask_ & 0x04) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (zone_4_ != nullptr) zone_4_->publish_state((current_zone_mask_ & 0x08) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (zone_5_ != nullptr) zone_5_->publish_state((current_zone_mask_ & 0x10) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (zone_6_ != nullptr) zone_6_->publish_state((current_zone_mask_ & 0x20) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
          }
        }
        this->rx_buffer_.clear();
      } else if (this->rx_buffer_.size() > 10) {
        this->rx_buffer_.erase(this->rx_buffer_.begin());
      }
    }
  }
}

// Optimistic State Forcing on Action Call
void SiemensHVACZoneController::send_button_press(uint8_t button_idx) {
  // 1. Send the raw UART pulse transmission to toggle the relay hardware
  uint8_t transmit_frame[5] = {0x02, 0xB5, button_idx, (uint8_t)(0x02 ^ 0xB5 ^ button_idx), 0x03};
  this->write_array(transmit_frame, 5);

  // 2. Compute an immediate optimistic state flip so Home Assistant reflects the toggle instantly
  uint8_t target_bit = (1 << button_idx);
  bool is_now_open = !(current_zone_mask_ & target_bit); // Invert the tracked bit state
  
  // 3. Force update the front-end entity right away
  valve::Valve* target_valve = nullptr;
  switch(button_idx) {
    case 0: target_valve = zone_1_; break;
    case 1: target_valve = zone_2_; break;
    case 2: target_valve = zone_3_; break;
    case 3: target_valve = zone_4_; break;
    case 4: target_valve = zone_5_; break;
    case 5: target_valve = zone_6_; break;
  }
  
  if (target_valve != nullptr) {
    target_valve->publish_state(is_now_open ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
  }
}

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
