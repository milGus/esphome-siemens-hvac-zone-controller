import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, valve
from esphome.const import CONF_ID

CODEOWNERS = ["@milGus"]
DEPENDENCIES = ["uart", "valve"]

siemens_hvac_zone_controller_ns = cg.esphome_ns.namespace("siemens_hvac_zone_controller")
SiemensHVACZoneController = siemens_hvac_zone_controller_ns.class_(
    "SiemensHVACZoneController", cg.Component, uart.UARTDevice
)
SiemensHVACZoneValve = siemens_hvac_zone_controller_ns.class_(
    "SiemensHVACZoneValve", valve.Valve, cg.Component
)

CONF_ZONE_1 = "zone_1"
CONF_ZONE_2 = "zone_2"
CONF_ZONE_3 = "zone_3"
CONF_ZONE_4 = "zone_4"
CONF_ZONE_5 = "zone_5"
CONF_ZONE_6 = "zone_6"

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(SiemensHVACZoneController),
        cv.Optional(CONF_ZONE_1): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
        cv.Optional(CONF_ZONE_2): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
        cv.Optional(CONF_ZONE_3): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
        cv.Optional(CONF_ZONE_4): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
        cv.Optional(CONF_ZONE_5): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
        cv.Optional(CONF_ZONE_6): valve.VALVE_SCHEMA.extend({cv.GenerateID(): cv.declare_id(SiemensHVACZoneValve)}),
    }).extend(uart.UART_DEVICE_SCHEMA).extend(cv.COMPONENT_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    zones = [CONF_ZONE_1, CONF_ZONE_2, CONF_ZONE_3, CONF_ZONE_4, CONF_ZONE_5, CONF_ZONE_6]
    for i, zone_key in enumerate(zones):
        if zone_key in config:
            conf = config[zone_key]
            valve_var = cg.new_Pvariable(conf[CONF_ID], var, i)
            await cg.register_component(valve_var, conf)
            await valve.register_valve(valve_var, conf)
            cg.add(getattr(var, f"set_zone_{i+1}")(valve_var))
