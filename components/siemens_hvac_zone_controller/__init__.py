import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, valve
from esphome.const import CONF_ID

CODEOWNERS = ["@milGus"]
DEPENDENCIES = ["uart"]

siemens_ns = cg.esphome_ns.namespace("siemens_hvac_zone_controller")
SiemensController = siemens_ns.class_("SiemensHVACZoneController", cg.Component, uart.UARTDevice)
SiemensValve = siemens_ns.class_("SiemensHVACZoneValve", valve.Valve)

CONF_UART_ID = "uart_id"
CONF_ZONES = ["zone_1", "zone_2", "zone_3", "zone_4", "zone_5", "zone_6"]

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SiemensController),
    cv.Required(CONF_UART_ID): cv.use_id(uart.UARTComponent),
}).extend(cv.COMPONENT_SCHEMA)

# Inject the 6 individual valve entities into the schema automatically
for idx, zone_name in enumerate(CONF_ZONES):
    CONFIG_SCHEMA = CONFIG_SCHEMA.extend({
        cv.Optional(zone_name): valve.valve_schema(SiemensValve).extend()
    })

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # Instantiate each of the 6 valves and pass them to the controller
    for idx, zone_name in enumerate(CONF_ZONES):
        if zone_name in config:
            val_conf = config[zone_name]
            val_var = cg.new_Pvariable(val_conf[CONF_ID], var, idx)
            await valve.register_valve(val_var, val_conf)
            cg.add(getattr(var, f"set_zone_{idx + 1}")(val_var))
