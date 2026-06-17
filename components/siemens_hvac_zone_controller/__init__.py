import esphome.config_validation as cv
from esphome.components import uart

CODEOWNERS = ["@milGus"]

# Officially declare uart_id as an accepted configuration property
CONFIG_SCHEMA = cv.Schema({
    cv.Optional("uart_id", default="siemens_gus_uart"): cv.use_id(uart.UARTComponent),
})

async def to_code(config):
    pass
