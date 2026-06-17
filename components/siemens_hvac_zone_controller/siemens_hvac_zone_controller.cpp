#include "siemens_hvac_zone_controller.h"
#include "esphome/core/log.h"

namespace esphome {
namespace siemens_hvac_zone_controller {

static const char *const TAG = "siemens_hvac_zone_controller";

void SiemensHVACZoneValve::control(const valve::ValveCall &call) {
  if (call.get_position().has_value()) {
    bool want_open = (*call.get_position() == valve::VALVE_OPEN);
    
    // 1. Force the UI to update instantly on your screen
    this->publish_state(want_open ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
    
    // 2. Transmit your exact 12-byte payload frame straight to the hardware
    this->parent_->send_zone_command(this->zone_idx_, want_open);
  }
}

void SiemensHVACZoneController::setup() {
  // Initialization routine
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
          
          if (master_zone_mask != this->current_zone_mask_) {
            this->current_zone_mask_ = master_zone_mask;
            
            // 3. Match your original working bit evaluation exactly
            if (this->zones_[0] != nullptr) this->zones_[0]->publish_state((master_zone_mask & 0x01) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (this->zones_[1] != nullptr) this->zones_[1]->publish_state((master_zone_mask & 0x02) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (this->zones_[2] != nullptr) this->zones_[2]->publish_state((master_zone_mask & 0x04) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (this->zones_[3] != nullptr) this->zones_[3]->publish_state((master_zone_mask & 0x08) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (this->zones_[4] != nullptr) this->zones_[4]->publish_state((master_zone_mask & 0x10) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
            if (this->zones_[5] != nullptr) this->zones_[5]->publish_state((master_zone_mask & 0x20) ? valve::VALVE_OPEN : valve::VALVE_CLOSED);
          }
        }
        this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + 13);
      } else {
        this->rx_buffer_.erase(this->rx_buffer_.begin());
      }
    }
  }
}

void SiemensHVACZoneController::send_zone_command(uint8_t zone_idx, bool open) {
  // Fire your original 12-byte command array sequence
  std::vector<uint8_t> frame = {0x02, 0x00, 0x22, 0x00, 0x1F, 0x01, 0x00, 0x01, 0x0C, 0x51, zone_idx, 0x03};
  this->write_array(frame.data(), frame.size());
}

}  // namespace siemens_hvac_zone_controller
}  // namespace esphome
