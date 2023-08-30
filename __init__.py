import re

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

CODEOWNERS = ["@fabianonline"]

DEPENDENCIES = ["uart"]

kld7_ns = cg.esphome_ns.namespace("kld7")
Kld7 = kld7_ns.class_("Kld7", cg.Component, uart.UARTDevice)
MULTI_CONF = True

CONF_KLD7_ID = "kld7_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Kld7),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
