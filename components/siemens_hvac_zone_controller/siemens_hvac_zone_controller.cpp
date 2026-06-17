#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone_controller";

// Global file-level tracker for the optimistic lock timer
static uint32_t global_lock_timeout_ms = 0;

void SiemensHVACZoneValve::control(const valve::ValveCall &call) {
  if (call.get_position().has_value()) {
    float target_position = *call.get_position();
    
    // 1. Force the UI state to update instantly
    this->publish_state(target_position);
    
    // 2. Lock out incoming status frames for 1500 milliseconds
    global_lock_timeout_ms = millis() + 1500;
    
    // 3. Fire the exact 12-byte command array sequence
    this->parent_->send_zone_command(this->zone_idx_);
  }
}

void SiemensHVACZoneController::setup() {
  // Initialization hook
}

void SiemensHVACZoneController::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Controller Component");
}

void SiemensHVACZoneController::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    this->rx_buffer_.push_back(byte);
    
    while (this->rx_buffer_.size() >= 13) {
      if (this->rx_buffer_[0] == 0x02 && this->rx_buffer_[1] == 0x00 && this->rx_buffer_[2] == 0x12) {
        if (this->rx_buffer_[12] == 0x03) {
          uint8_t master_zone_mask = this->rx_buffer_[11];
          
          // FIX: Use the file-level static timer to protect state updates
          if (millis() > global_lock_timeout_ms) {
            if (master_zone_mask != this->current_zone_mask_) {
              this->current_zone_mask_ = master_zone_mask;
              
              if (this->zones_[0] != nullptr) this->zones_[0]->publish_state((master_zone_mask & 0x01) ? 1.0f : 0.0f);
              if (this->zones_[1] != nullptr) this->zones_[1]->publish_state((master_zone_mask & 0x02) ? 1.0f : 0.0f);
              if (this->zones_[2] != nullptr) this->zones_[2]->publish_state((master_zone_mask & 0x04) ? 1.0f : 0.0f);
              if (this->zones_[3] != nullptr) this->zones_[3]->publish_state((master_zone_mask & 0x08) ? 1.0f : 0.0f);
              if (this->zones_[4] != nullptr) this->zones_[4]->publish_state((master_zone_mask & 0x10) ? 1.0f : 0.0f);
              if (this->zones_[5] != nullptr) this->zones_[5]->publish_state((master_zone_mask & 0x20) ? 1.0f : 0.0f);
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
