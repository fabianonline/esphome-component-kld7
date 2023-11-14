#include "kld7.h"
#include "esphome/core/log.h"

namespace esphome {
namespace kld7 {
	
static const char* TAG = "KLD7";

void Kld7::setup() {
	ESP_LOGI(TAG, "Starting initialization of K-LD7...");
	ESP_LOGD(TAG, "Sending INIT");
	write_array((std::array<uint8_t, 12>){'I', 'N', 'I', 'T', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending RSPI");
	write_array((std::array<uint8_t, 12>){'R', 'S', 'P', 'I', 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00}); // Maximum speed = 100km/h
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending RRAI");
	write_array((std::array<uint8_t, 12>){'R', 'R', 'A', 'I', 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00}); // Maximum range = 30m
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending THOF");
	write_array((std::array<uint8_t, 12>){'T', 'H', 'O', 'F', 0x04, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00}); // Threshold offset = 20dB
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending TRFT");
	write_array((std::array<uint8_t, 12>){'T', 'R', 'F', 'T', 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00}); // Tracking filter type = Fast detection
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending MARA");
	write_array((std::array<uint8_t, 12>){'M', 'A', 'R', 'A', 0x04, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00}); // Maximum range = 100%
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending RATH");
	write_array((std::array<uint8_t, 12>){'R', 'A', 'T', 'H', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}); // Range threshold = 0%
	_wait_for_ok();
	ESP_LOGD(TAG, "Sending SPTH");
	write_array((std::array<uint8_t, 12>){'S', 'P', 'T', 'H', 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}); // Speed threshold = 0%
	_wait_for_ok();
	ESP_LOGI(TAG, "Initialization complete");
}

bool Kld7::_wait_for_ok() {
	unsigned long start = millis();
	while(true) {
		if (millis() - start > 500) {
			ESP_LOGE(TAG, "K-LD7 failed to respond within 500ms.");
			mark_failed();
			return false;
		}
		if (available()>=9) {
			uint8_t data[9];
			read_array(data, 9);
			if (data[0]=='R' && data[1]=='E' && data[2]=='S' && data[3]=='P' &&
				data[4]==0x01 && data[5]==0x00&& data[6]==0x00 && data[7]==0x00 &&
				data[8]==0x00) return true;
			ESP_LOGE(TAG, "K-LD7 sent an unexpected answer during setup.");
			return false;
		}
	}
}

void Kld7::loop() {
	if (_waiting_for_data == false && (_last_request > millis() || _last_request + REQUEST_INTERVAL < millis())) {
		//ESP_LOGD(TAG, "Sending GNFD");
		write_array((std::array<uint8_t, 12>){'G', 'N', 'F', 'D', 0x04, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00});
		_waiting_for_data = true;
		_last_request = millis();
	}
	if (available() >= 8) {
		uint8_t head[8];
		read_array(head, 8);
		//ESP_LOGD(TAG, "Recieved data: %02X %02X %02X %02X %02X %02X %02X %02X", head[0], head[1], head[2], head[3], head[4], head[5], head[6], head[7]);
		char command[5];
		memcpy(command, head, 4);
		command[4] = '\0';
		uint32_t length = head[4] | (head[5]<<8) | (head[6]<<16) | (head[7]<<24);
		uint8_t payload[length];
		read_array(payload, length);
		//ESP_LOGD(TAG, "Received command %s with a payload of %d bytes.", command, length);
		if (strcmp(command, "RESP")==0) {
			if (length != 1) {
				ESP_LOGE(TAG, "Received RESP with wrong payload length (expected: 1, was: %d)", length);
				return;
			}
			switch (payload[0]) {
				case 0: break; // OK
				case 1: ESP_LOGE(TAG, "RESP: Unknown command"); break;
				case 2: ESP_LOGE(TAG, "RESP: Invalid parameter value"); break;
				case 3: ESP_LOGE(TAG, "RESP: Invalid RPST version"); break;
				case 4: ESP_LOGE(TAG, "RESP: Uart error (parity, framing, noise)"); break;
				case 5: ESP_LOGE(TAG, "RESP: Sensor busy"); break;
				case 6: ESP_LOGE(TAG, "RESP: Timeout error"); break;
				default: ESP_LOGE(TAG, "RESP: Unknown error code %02X", payload[0]); break;
			}
		} else if(strcmp(command,"TDAT")==0) {
			if (length != 8 && length!=0) {
				ESP_LOGE(TAG, "Received TDAT with wrong payload length (expected: 0 or 8, was: %d)", length);
				return;
			}
			_last_raw = RawRadarEvent(payload, length, this->_invert_angle);
			if (_last_raw.detection) {
				//ESP_LOGD(TAG, "Raw data: %d cm, %.1f km/h, %.1f°, %.1fdB", _last_raw.distance, _last_raw.speed, _last_raw.angle, _last_raw.magnitude);
				if (_raw_speed_sensor != NULL) _raw_speed_sensor->publish_state(_last_raw.speed);
				if (_raw_angle_sensor != NULL) _raw_angle_sensor->publish_state(_last_raw.angle);
				if (_raw_distance_sensor != NULL) _raw_distance_sensor->publish_state(_last_raw.distance);
				if (_raw_direction_sensor != NULL) _raw_direction_sensor->publish_state(_last_raw.speed > 0);
				if (_raw_json_sensor != NULL) {
					char buff[64];
  					snprintf(buff, sizeof(buff), "{\"speed\":%.1f,\"angle\":%.1f,\"distance\":%d}", _last_raw.speed, _last_raw.angle, _last_raw.distance);
					_raw_json_sensor->publish_state(buff);
				}
			 } else {
				//ESP_LOGD(TAG, "Raw data: No detection");
			 }
			 if (_raw_detection_sensor != NULL) _raw_detection_sensor->publish_state(_last_raw.detection);
			 
			 if (_filtered_detection_sensor != NULL) {
				if (_last_raw.detection &&
					_last_raw.angle >= _filtered_sensor_min_angle && _last_raw.angle <= _filtered_sensor_max_angle &&
					_last_raw.distance >= _filtered_sensor_min_distance && _last_raw.distance <= _filtered_sensor_max_distance)
				{
					_filtered_sensor_points++;
					_filtered_sensor_last_ts = millis();
					if (_filtered_sensor_points >= _filtered_sensor_min_points) {
						_filtered_detection_sensor->publish_state(true);
					}
				} else {
					if (_filtered_sensor_last_ts > millis() || _filtered_sensor_last_ts < millis() - _filtered_sensor_timeout) {
						_filtered_sensor_points = 0;
						_filtered_detection_sensor->publish_state(false);
					}
				}
			 }
			_process_detection();
			_waiting_for_data = false;
		} else {
			ESP_LOGW(TAG, "Received unexpected command %s. Ignoring.", command);
		}
	}
}

void Kld7::_process_detection() {
	if (_last_raw.detection == false) {
		if (_current_process.active && (_current_process.timestamp + DETECTION_TIMEOUT < millis() || _current_process.timestamp > millis())) {
			// Object is gone. Stop processing.
			_finish_processing();
		}
	} else {
		bool direction_away_from_radar = _last_raw.speed > 0;
		if (_current_process.timestamp > millis() || millis() - _current_process.timestamp > DETECTION_TIMEOUT ||
			direction_away_from_radar != _current_process.direction_away_from_radar ||
			fabs(_last_raw.speed - _current_process.last_speed)>DETECTION_SPEED_DIFFERENCE) {
			// Finish detection and start a new one
			_finish_processing();
		}

		// TODO save the data
		_current_process.active = true;
		_current_process.timestamp = millis();
		_current_process.points++;
		_current_process.direction_away_from_radar = direction_away_from_radar;
		_current_process.last_speed = _last_raw.speed;
		float abs_speed = fabs(_last_raw.speed);
		_current_process.speed_sum += _last_raw.speed;
		if (abs_speed > fabs(_current_process.max_speed)) {
			_current_process.not_max_speed = _current_process.max_speed;
			_current_process.max_speed = _last_raw.speed;
		} else if (abs_speed > fabs(_current_process.not_max_speed)) {
			_current_process.not_max_speed = _last_raw.speed;
		}
	}
	_previous_raw = _last_raw;
}

void Kld7::_finish_processing() {
	_current_process.active = false;
	if (_current_process.points > PROCESS_MIN_POINTS) {
		float avg_speed = _current_process.speed_sum / _current_process.points;
		if (_speed_sensor != NULL) _speed_sensor->publish_state(avg_speed);
		if (_points_sensor != NULL) _points_sensor->publish_state(_current_process.points);
		if (_max_speed_sensor != NULL) _max_speed_sensor->publish_state(_current_process.not_max_speed);
		if (_detection_sensor != NULL) _detection_sensor->publish_state(true);
		if (_json_sensor != NULL) {
			char buffer[64];
			snprintf(buffer, sizeof(buffer), "{\"speed\":%.1f,\"max_speed\":%.1f,\"points\":%d}", avg_speed, _current_process.not_max_speed, _current_process.points);
			_json_sensor->publish_state(buffer);
		}
		ESP_LOGD(TAG, "_finish_processing. %d points, maximum %f.1 km/h, average %f.1 km/h, direction_away_from_radar %d", _current_process.points, _current_process.not_max_speed, avg_speed, _current_process.direction_away_from_radar ? 1 : 0);
	} else {
		ESP_LOGD(TAG, "_finish_processing: Too little data. Ignoring event.");
	}
	_current_process = ProcessedRadarEvent();
	if (_detection_sensor != NULL) _detection_sensor->publish_state(false);
}
void Kld7::dump_config() { 
	ESP_LOGCONFIG(TAG, "KLD7:"); 
}
} // namespace kld7
} // namespace esphome