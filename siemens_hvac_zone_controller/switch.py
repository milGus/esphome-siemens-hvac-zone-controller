import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ID
from . import SiemensHVACZoneController, siemens_hvac_zone_controller_ns

SiemensHVACZoneSwitch = siemens_hvac_zone_controller_ns.class_("SiemensHVACZoneSwitch", switch.Switch)

CONF_SIEMENS_ID = "siemens_hvac_zone_controller_id"
CONF_ZONE_NUMBER = "zone_number"

CONFIG_SCHEMA = switch.SWITCH_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(SiemensHVACZoneSwitch),
    cv.Required(CONF_SIEMENS_ID): cv.use_id(SiemensHVACZoneController),
    cv.Required(CONF_ZONE_NUMBER): cv.int_range(min=1, max=6),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_SIEMENS_ID])
    # Map layout indexes 0x00 to 0x05 internally for C++ processing
    btn_idx = config[CONF_ZONE_NUMBER] - 1 
    
    var = cg.new_Pvariable(config[CONF_ID], parent, btn_idx)
    await switch.register_switch(var, config)
    
    cg.add(getattr(parent, f"set_zone_{config[CONF_ZONE_NUMBER]}")(var))
