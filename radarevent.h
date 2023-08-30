#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace kld7 {
	class RawRadarEvent {
		public:
		RawRadarEvent() {};
		RawRadarEvent(uint8_t* data, uint32_t length);
		bool detection = false;
		uint16_t distance;
		float speed;
		float angle;
		float magnitude;
		unsigned long timestamp;
	};

	class ProcessedRadarEvent {
		public:
		uint16_t points = 0;
		bool active = false;
		float speed_sum = 0.0;
		float max_speed = 0.0;
		float not_max_speed = 0.0;
		float avg_speed = 0.0;
		bool direction_away_from_radar;
		unsigned long long timestamp;
		float last_speed;
	};


} // namespace kld7
} // namespace esphome