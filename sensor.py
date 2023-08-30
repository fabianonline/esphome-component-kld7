import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, CONF_TYPE

from . import CONF_KLD7_ID, kld7_ns, Kld7

AUTO_LOAD = ["kld7"]
CONF_TYPE_SPEED = "speed"
CONF_TYPE_RAW_SPEED = "raw_speed"

Kld7Sensor = kld7_ns.class_("Kld7Sensor", sensor.Sensor, cg.Component)

CONFIG_SCHEMA = sensor.sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(Kld7Sensor),
        cv.GenerateID(CONF_KLD7_ID): cv.use_id(Kld7),
        cv.Required(CONF_TYPE): cv.one_of(CONF_TYPE_SPEED, CONF_TYPE_RAW_SPEED, lower=True)
    }
)


async def to_code(config):
    print(config)
    var = cg.new_Pvariable(
        config[CONF_ID],
    )
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    kld7 = await cg.get_variable(config[CONF_KLD7_ID])
    if config[CONF_TYPE] == CONF_TYPE_SPEED:
        cg.add(kld7.register_speed_sensor(var))
    if config[CONF_TYPE] == CONF_TYPE_RAW_SPEED:
        cg.add(kld7.register_raw_speed_sensor(var))
        
