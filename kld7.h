#pragma once

#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "radarevent.h"

namespace esphome {
namespace kld7 {

const int DETECTION_TIMEOUT = 1000;
const float DETECTION_SPEED_DIFFERENCE = 12.5;
const int PROCESS_MIN_POINTS = 5;
const int REQUEST_INTERVAL = 100;

class Kld7Sensor : public Component, public sensor::Sensor {
 public:
  //Kld7Sensor(){};
  //void publish_raw_event(const RawRadarEvent &raw){};
  //void publish_processed_event(const ProcessedRadarEvent &processed) {};
};

class Kld7 : public Component, public uart::UARTDevice {
 public:
  Kld7() {};
  void register_speed_sensor(Kld7Sensor* sensor) { this->_speed_sensors.emplace_back(sensor); };
  void register_raw_speed_sensor(Kld7Sensor* sensor) { this->_raw_speed_sensors.emplace_back(sensor); };
  void loop() override;
  void setup() override;
  void dump_config() override;
  std::vector<Kld7Sensor*> _speed_sensors;
  std::vector<Kld7Sensor*> _raw_speed_sensors;


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