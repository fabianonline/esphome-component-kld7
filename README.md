# esphome-component-kld7

## Description

This is a esphome component to use a KLD7 doppler radar module with ESPhome.

## Config

The whole config needed to use this component looks somewhat like this:


```yaml
external_components:
  - source: github://fabianonline/esphome-component-kld7@main
    components: [ rtttl ]

uart:
  id: uart_bus
  tx_pin: GPIO13
  rx_pin: GPIO16
  baud_rate: 115200 # Don't change this
  parity: even # Don't change this
  

kld7:
  uart_id: uart_bus
  #invert_angle: false

sensor:
  - platform: kld7
    name: K-LD7 Sensor
    speed:
      name: Speed
    
    avg_speed:
      name: Avg speed
    points:
      name: Points
    raw_speed:
      name: Raw speed
    raw_angle:
      name: Raw angle
    raw_distance:
      name: Raw distance
    raw_magnitude:
      name: Raw magnitude

binary_sensor:
  - platform: kld7
    name: K-LD7
    detection:
      name: Detection
    raw_detection:
      name: Raw Detection
    raw_direction_away_from_sensor:
      name: Raw direction away from sensor
    filtered_detection:
      name: Filtered detection
      min_distance: 0
      max_distance: 500
      min_angle: -90
      max_angle: -10
      min_points: 5
      timeout: 1000

text_sensor:
  - platform: kld7
    name: K-LD7
    raw_json:
      name: Raw JSON
    json:
      internal: true
      name: JSON
```

Basically, you start by defining an `uart`. You can set `tx_pin` and `rx_pin` to whichever pins you've connected the K-LD7 to. `baud_rate` has to be set to `115200` and `parity` to `even`.

After that, you define the K-LD7 module. It needs to know the `uart_id` of the UART you just defined. If your K-LD7 is mounted upside down, set `invert_angle` to true.

### Sensors

Then you can define the values you want to receive. The available sensors are split into two groups: Raw sensors (every sensor starting with `raw_`) and processed sensors. The difference between them is as follows:

A **raw sensor** returns data more or less just as it's received from the KLD-7. There can be quite a lot of data in a short time; if there is enough movement, 10 updates per second aren't uncommon. So you should really think about which raw sensors you really need.

A **processed** sensor on the other hand corresponds to one single object seen by the radar. As long as, say, a car is driving towards the radar, it is seen as a single object and its dara is collected. Only if the car isn't seen for a short time or another object (for example a car driving in the opposite direction) is reported by the radar, the data for the first car is returned and the new car is tracked.

| Sensor | Class | Type | Example | Description |
| --- | --- | --- | --- | --- |
| `speed` | sensor | Processed | 34.1 | Returns the highest speed detected for the current object |
| `avg_speed` | sensor | Processed | 22.1 | Returns the average of all speeds detected for the current object |
| `points` | sensor | Processed | 12 | Returns the number of detections for the current object. Objects with less than 6 detections are discarded, so you should never see a lower number here. |
| `raw_speed` | sensor | Raw | 33.8 | Raw speed detected. |
| `raw_angle` | sensor | Raw | -3.8 | Raw angle detected. Positive values are to the right of the center axis of the sensor, negative values to the left. Inverted if `invert_angle` is set to `true`. |
| `raw_distance` | sensor | Raw | 2371 | Raw detection distance in cm. |
| `raw_magnitude` | sensor | Raw | 31.7 | Raw magnitude detected. |
| `detection` | binary_sensor | Processed | - | Shortly becomes `true` whenever a valid object was processed. |
| `raw_detection` | binary_sensor | Raw | - | Is `true` whenever the radar detects something. |
| `raw_direction_away_from_sensor` | binary_sensor | Raw | - | `true` if the currently detected object is moving away from the sensor, `false` otherwise. Only valid if `raw_detecttion` is true. |
| `filtered_detection` | binary_sensor | Processed | - | Only true, if an object is detected within certain parameters: It has to be between `min_distance` and `max_distance` cm from the radar and at an angle of between `min_angle` and `max_angle` degrees. Also it has to be detected for at least `min_points` times without more than `timeout` ms without an detection. |
| `json` | text_sensor | Processed | `{"speed":34.6,"avg_speed":21.9,"points":43}` | Returns all processed data as a single JSON string. |
| `raw_json` | text_sensor | Raw | `{"speed":-3.2,"angle":43.5,"distance":715,"magnitude":31.9}` | Returns all raw data as a single JSON string. |