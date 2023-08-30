#pragma once

#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "radarevent.h"

namespace esphome {
namespace kld7 {

const int DETECTION_TIMEOUT = 1000;
const float DETECTION_SPEED_DIFFERENCE = 12.5;
const int PROCESS_MIN_POINTS = 5;
const int REQUEST_INTERVAL = 100;

class Sensor : public Component, public sensor::Sensor {};
class BinarySensor : public Component, public binary_sensor::BinarySensor {};

class Kld7 : public Component, public uart::UARTDevice {
 public:
  Kld7() {};
  void register_speed_sensor(sensor::Sensor* sensor) { this->_speed_sensor = sensor; };
  void register_raw_speed_sensor(sensor::Sensor* sensor) { this->_raw_speed_sensor = sensor; };
  void register_raw_detection_sensor(binary_sensor::BinarySensor* sensor) { this->_raw_detection_sensor = sensor; };
  void register_raw_direction_sensor(binary_sensor::BinarySensor* sensor) { this->_raw_direction_sensor = sensor; };

  void loop() override;
  void setup() override;
  void dump_config() override;
  sensor::Sensor* _speed_sensor = NULL;
  sensor::Sensor* _raw_speed_sensor = NULL;
  binary_sensor::BinarySensor* _raw_detection_sensor = NULL;
  binary_sensor::BinarySensor* _raw_direction_sensor = NULL;

 private:
  RawRadarEvent _last_raw;
  RawRadarEvent _previous_raw;
  ProcessedRadarEvent _current_process;
  bool _wait_for_ok();
  bool _waiting_for_data = false;
  void _process_detection();
  void _finish_processing();
  unsigned long long _last_request = 0;
};

}  // namespace kld7
}  // namespace esphome