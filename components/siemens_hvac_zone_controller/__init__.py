import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

siemens_hvac_zone_controller_ns = cg.esphome_ns.namespace("siemens_hvac_zone_controller")
SiemensHVACZoneController = siemens_hvac_zone_controller_ns.class_(
    "SiemensHVACZoneController", cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SiemensHVACZoneController),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
