# SmartLamp Mongoose OS firmware, see https://mongoose-os.com/

build:
mos build --platform exp32 --local --verbose

flash:
mos flash --port /dev/ttyUSB0
mos gcp-iot-setup --gcp-project iotlamp-kqwpbb --gcp-region us-central1 --gcp-registry iot-registry
mos wifi WIRELESS_SSID ****KEY*
