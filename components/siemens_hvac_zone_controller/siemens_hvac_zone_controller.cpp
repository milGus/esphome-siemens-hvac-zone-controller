#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone_controller";

void SiemensHVACZoneValve::setup() {
  this->position = 0.0f;
  this->current_operation = valve::VALVE_OPERATION_IDLE;
}

void SiemensHVACZoneValve::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Valve Configured");
}

void SiemensHVACZoneValve::control(const valve::ValveCall &call) {
  if (call.get_position().has_value()) {
    float target_position = *call.get_position();
    bool open_state = (target_position > 0.5f);
    
    // Snap target positions and publish instantly
    this->position = open_state ? 1.0f : 0.0f;
    this->current_operation = valve::VALVE_OPERATION_IDLE;
    this->publish_state();
    
    this->parent_->set_lock_timeout(1500);
    this->parent_->send_zone_command(this->zone_idx_);
  }
}

void SiemensHVACZoneController::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Controller Active");
}

void SiemensHVACZoneController::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    this->rx_buffer_.push_back(byte);
    
    if (this->rx_buffer_.size() > 100) {
      this->rx_buffer_.erase(this->rx_buffer_.begin());
    }
    
    while (this->rx_buffer_.size() >= 13) {
      if (this->rx_buffer_[0] == 0x02 && this->rx_buffer_[1] == 0x00 && this->rx_buffer_[2] == 0x12) {
        if (this->rx_buffer_[12] == 0x03) {
          uint8_t master_zone_mask = this->rx_buffer_[11];
          
          if (millis() > this->lock_timeout_ms_) {
            if (master_zone_mask != this->current_zone_mask_) {
              this->current_zone_mask = master_zone_mask;
              
              uint8_t masks[6] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
              for (int i = 0; i < 6; i++) {
                if (this->zones_[i] != nullptr) {
                  float new_pos = (master_zone_mask & masks[i]) ? 1.0f : 0.0f;
                  if (this->zones_[i]->position != new_pos) {
                    this->zones_[i]->position = new_pos;
                    this->zones_[i]->current_operation = valve::VALVE_OPERATION_IDLE;
                    this->zones_[i]->publish_state();
                  }
                }
              }
            }
          }
        }
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + 13);
      } else {
        this->rx_buffer_.erase(this->rx_buffer_.begin());
      }
    }
  }
}

void SiemensHVACZoneController::send_zone_command(uint8_t zone_idx) {
  std::vector<uint8_t> frame = {0x02, 0x00, 0x22, 0x00, 0x1F, 0x01, 0x00, 0x01, 0x0C, 0x51, zone_idx, 0x03};
  this->write_array(frame.data(), frame.size());
}

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
