import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor
from esphome.const import CONF_ID, CONF_TYPE, STATE_CLASS_MEASUREMENT, CONF_STATE_CLASS, DEVICE_CLASS_MOTION

from . import CONF_KLD7_ID, kld7_ns, Kld7

AUTO_LOAD = ["kld7"]
CONF_TYPE_RAW_DETECTION = "raw_detection"
CONF_TYPE_RAW_DIRECTION_AWAY_FROM_SENSOR = "raw_direction_away_from_sensor"

Kld7BinarySensor = kld7_ns.class_("BinarySensor", binary_sensor.BinarySensor, cg.Component)

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema().extend(
    {
        cv.GenerateID(): cv.declare_id(Kld7BinarySensor),
        cv.GenerateID(CONF_KLD7_ID): cv.use_id(Kld7),
        cv.Optional(CONF_TYPE_RAW_DETECTION): binary_sensor.binary_sensor_schema(
            device_class= DEVICE_CLASS_MOTION
        ),
        cv.Optional(CONF_TYPE_RAW_DIRECTION_AWAY_FROM_SENSOR): binary_sensor.binary_sensor_schema(
        )
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    kld7 = await cg.get_variable(config[CONF_KLD7_ID])

    if sensor_config := config.get(CONF_TYPE_RAW_DETECTION):
        sens = await binary_sensor.new_binary_sensor(sensor_config)
        cg.add(kld7.register_raw_detection_sensor(sens))
    if sensor_config := config.get(CONF_TYPE_RAW_DIRECTION_AWAY_FROM_SENSOR):
        sens = await binary_sensor.new_binary_sensor(sensor_config)
        cg.add(kld7.register_raw_direction_sensor(sens))
        
