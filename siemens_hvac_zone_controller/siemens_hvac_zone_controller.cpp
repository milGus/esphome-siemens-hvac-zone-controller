#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone.component";

void SiemensHVACZoneController::setup() {
  // Setup hooks if required by the hardware layer later
}

void SiemensHVACZoneController::loop() {
  while (available() > 0) {
    uint8_t b;
    read_byte(&b);
    rx_buffer_.push_back(b);
  }

  while (rx_buffer_.size() >= 13) {
    // Match the Siemens Master panel's status stream header
    if (rx_buffer_[0] == 0x02 && rx_buffer_[1] == 0x00 && rx_buffer_[2] == 0x12) {
      if (rx_buffer_[12] == 0x03) {
        uint8_t master_zone_mask = rx_buffer_[11];
        
        if (master_zone_mask != current_zone_mask_) {
          current_zone_mask_ = master_zone_mask;
          
          if (zone_1_ != nullptr) zone_1_->publish_state(master_zone_mask & 0x01);
          if (zone_2_ != nullptr) zone_2_->publish_state(master_zone_mask & 0x02);
          if (zone_3_ != nullptr) zone_3_->publish_state(master_zone_mask & 0x04);
          if (zone_4_ != nullptr) zone_4_->publish_state(master_zone_mask & 0x08);
          if (zone_5_ != nullptr) zone_5_->publish_state(master_zone_mask & 0x10);
          if (zone_6_ != nullptr) zone_6_->publish_state(master_zone_mask & 0x20);
        }
      }
      rx_buffer_.erase(rx_buffer_.begin(), rx_buffer_.begin() + 13);
    } else {
      rx_buffer_.erase(rx_buffer_.begin());
    }
  }
}

void SiemensHVACZoneController::send_button_press(uint8_t button_idx) {
  std::vector<uint8_t> frame = {0x02, 0x00, 0x22, 0x00, 0x1F, 0x01, 0x00, 0x01, 0x0C, 0x51, button_idx, 0x03};
  write_array(frame.data(), frame.size());
}

void SiemensHVACZoneController::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Controller Secondary Device Initialized");
}

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
