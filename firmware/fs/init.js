load('api_config.js');
load('api_events.js');
load('api_gcp.js');
load('api_mqtt.js');
load('api_sys.js');

print('*** START ****');

let showLedColor = ffi('void showLedColor()');
let turnLedColorOn = ffi('void turnLedColorOn()');
let setLedColorTemperature = ffi('void setLedColorTemperature(int)');
let setLedColorPower = ffi('void setLedColorPower(int)');
let hideLeds = ffi('void hideLeds()');
let showRainbow = ffi('void showRainbow()');
let showClock = ffi('void showClock()');
let showAdv = ffi('void showAdv()');

let topicConfig = '/devices/' + Cfg.get('device.id') + '/config';

showAdv();

Event.on(Event.CLOUD_CONNECTED, function () {
    print('Cloud CONNECTED');
}, null);

Event.on(Event.CLOUD_DISCONNECTED, function () {
    print('Cloud DISCONNECTED');
}, null);

let currentState = {id: ''};

MQTT.sub(topicConfig, function (conn, topic, msg) {
    print('Topic:', topic, 'message:', msg);
    let state = JSON.parse(msg);
    if (currentState.id === state.id) {
        print('Duplicate state update -> skip it');
        return;
    }
    currentState = state;
    if (state.mode === 'TurnOff') {
        hideLeds();
    } else if (state.mode === 'TurnOn') {
        setLedColorTemperature(state.temperature);
        setLedColorPower(state.power > 60 ? 60 : state.power);
        turnLedColorOn();
        showLedColor();
    } else if (state.mode === 'showRainbow') {
        showRainbow();
    } else if (state.mode === 'showClock') {
        showClock();
    } else if (state.mode === 'showAdv') {
        showAdv();
    } else {
        print('Command is not supported: ', state.mode);
    }
}, null);
