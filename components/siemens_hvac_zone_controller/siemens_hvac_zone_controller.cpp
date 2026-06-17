#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone_controller";

void SiemensHVACZoneValve::control(const valve::ValveCall &call) {
  if (call.get_position().has_value()) {
    float target_position = *call.get_position();
    
    // 1. Instantly snap the dashboard entity switch into place locally
    this->publish_state(target_position);
    
    // 2. Lock the parent's UART state processor to prevent immediate packet echoes from overriding us
    this->parent_->set_lock_timeout(1200);
    
    // 3. Dispatch the transmission frame array
    this->parent_->send_zone_command(this->zone_idx_);
  }
}

void SiemensHVACZoneController::setup() {
  // Initialization entry point
}

void SiemensHVACZoneController::dump_config() {
  ESP_LOGCONFIG(TAG, "Siemens HVAC Zone Controller External Component Activated");
}

void SiemensHVACZoneController::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    this->rx_buffer_.push_back(byte);
    
    // Protect the buffer length constraints strictly
    if (this->rx_buffer_.size() > 100) {
      this->rx_buffer_.erase(this->rx_buffer_.begin());
    }
    
    while (this->rx_buffer_.size() >= 13) {
      // Validate the definitive 3-byte master HVAC state stream header
      if (this->rx_buffer_[0] == 0x02 && this->rx_buffer_[1] == 0x00 && this->rx_buffer_[2] == 0x12) {
        
        // Assert the footer byte alignment
        if (this->rx_buffer_[12] == 0x03) {
          uint8_t master_zone_mask = this->rx_buffer_[11];
          
          // Only sync state with the hardware if our optimistic control window lock has cleared
          if (millis() > this->lock_timeout_ms_) {
            if (master_zone_mask != this->current_zone_mask_) {
              this->current_zone_mask_ = master_zone_mask;
              
              // Publish standard native ESPHome float scaling states (1.0f = OPEN, 0.0f = CLOSED)
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
        // Pop single byte and slide vector pointer ahead
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
