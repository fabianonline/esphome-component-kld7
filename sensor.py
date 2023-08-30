import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart, sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_SPEED,
    STATE_CLASS_MEASUREMENT,
    UNIT_KILOMETER_PER_HOUR
)

DEPENDENCIES = ["uart"]

STR_SPEED = "speed"
STR_RAW_SPEED = "raw_speed"

kld7_ns = cg.esphome_ns.namespace("kld7")

Kld7 = kld7_ns.class_("Kld7", cg.Component, uart.UARTDevice)


CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Kld7),
            cv.GenerateID("kld7_id"): cv.use_id(Kld7),
            cv.Optional(STR_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(STR_RAW_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_KILOMETER_PER_HOUR,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    print(config)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if speed_config := config.get(STR_SPEED):
        sens = await sensor.new_sensor(speed_config)
        cg.add(var.set_speed_sensor(sens))

    if raw_speed_config := config.get(STR_RAW_SPEED):
        sens = await sensor.new_sensor(raw_speed_config)
        cg.add(var.set_raw_speed_sensor(sens))
