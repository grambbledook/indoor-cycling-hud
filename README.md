# indoor cycling hud
A simple GUI app that displays a HUD (Heads-Up Display) on the screen. 
It is intended to be used during indoor cycling on top of other full screen apps or in addition to watching youtube videos.

# Disclaimer
This is a hobby project and my first GUI app. It is not guaranteed to be accurate or reliable. Use at your own risk.

## Supported Features

### Supported Devices
[x] Heart Rate Monitors (Bluetooth LE) \
[x] Cycling Speed and Cadence Sensors (Bluetooth LE) \
[x] Power Meters (Bluetooth LE) \
[ ] Indoor Cycling Trainers (FE-C over BLE) \
[ ] Indoor Cycling Trainers (Bluetooth LE) 

### Data Fields
[x] Instant and average heart rate \
[x] Instant and average cycling speed \
[x] Instant and average cycling cadence \
[x] Instantaneous cycling power \
[ ] 3s average cycling power \
[ ] Total time and distance 

### In-door training features
[ ] ERG Mode \
[ ] Set and update target power \
[ ] Set and update target grade \
[ ] Structured workouts

### UI features
[x] Overlay mode \
[x] Switching between dark/bright color schemes

# Screenshots
### Select trainer view
<img src="https://github.com/grambbledook/indoor-cycling-hud/blob/89a8229f3a72081b729f71e90409470b3024999e/_docs/select_trainer_view.png" width="150" height="150"/>

### Select sensors view
<img src="https://github.com/grambbledook/indoor-cycling-hud/blob/89a8229f3a72081b729f71e90409470b3024999e/_docs/select_sensors_view.png" width="250" height="250"/>

### Workout view
<img src="https://github.com/grambbledook/indoor-cycling-hud/blob/89a8229f3a72081b729f71e90409470b3024999e/_docs/workout_view.png" width="150" height="400"/>

# Useful links
Tacx Trainers supported protocols: https://developer.garmin.com/tacx-trainer/overview/ 

Heart Rate Service: https://www.bluetooth.com/specifications/specs/heart-rate-service-1-0/ \
GATT Specification Supplement 5: https://www.bluetooth.com/specifications/specs/gatt-specification-supplement-5/ \
FTSM Protocol: https://www.bluetooth.com/specifications/specs/fitness-machine-service-1-0/ \
FE-C over BLE: https://raw.githubusercontent.com/jedla22/BleTrainerControl/master/How-to%20FE-C%20over%20BLE%20v1_0_0.pdf
