// Copyright 2019, DIY AGalilov

'use strict';

const uuidv1 = require('uuid/v1');
const functions = require('firebase-functions');
const {WebhookClient} = require('dialogflow-fulfillment');
//const {Card, Suggestion} = require('dialogflow-fulfillment');

const gcloudProject = 'iotlamp-kqwpbb';
const iotRegion = 'us-central1';
const iotRegistry = 'iot-registry';
const deviceId = 'esp32_F4149C';

process.env.GCLOUD_PROJECT = gcloudProject;
process.env.DEBUG = 'dialogflow:debug'; // enables lib debugging statements

const iot = require('@google-cloud/iot');
const client = new iot.v1.DeviceManagerClient();

function generateRequest(configData) {
    const formattedName = client.devicePath(gcloudProject, iotRegion, iotRegistry, deviceId);
    let dataValue = Buffer.from(JSON.stringify(configData)).toString("base64");
    return {
        name: formattedName,
        binaryData: dataValue
    };
}

exports.dialogflowFirebaseFulfillment = functions.https.onRequest((request, response) => {
    const agent = new WebhookClient({request, response});

    async function turnOnHandler(agent) {
        var temperature = 1000;
        switch (agent.parameters.tone) {
            case 'дневной':
                temperature = 4000;
                break;
            case 'тёплый':
                temperature = 2600;
                break;
            case 'холодный':
                temperature = 6000;
                break;
            default:
                break;
        }
        const state = {
            'id': uuidv1(),
            'mode': 'TurnOn',
            'power': agent.parameters.power,
            'temperature': temperature,
        };
        const request = generateRequest(state);
        await client.modifyCloudToDeviceConfig(request);
        agent.add(`Включена лампа с тоном ${agent.parameters.tone} на ${agent.parameters.power}%`);
    }

    async function turnOffHandler(agent) {
        const state = {
            'id': uuidv1(),
            'mode': 'TurnOff',
        };
        const request = generateRequest(state);
        await client.modifyCloudToDeviceConfig(request);
        agent.add('Лампа выключена');
    }

    async function showRainbowHandler(agent) {
        const state = {
            'id': uuidv1(),
            'mode': 'showRainbow',
        };
        const request = generateRequest(state);
        await client.modifyCloudToDeviceConfig(request);
        agent.add('Лампа включена в режиме радуга');
    }

    async function showAdvHandler(agent) {
        const state = {
            'id': uuidv1(),
            'mode': 'showAdv',
        };
        const request = generateRequest(state);
        await client.modifyCloudToDeviceConfig(request);
        agent.add('Включена реклама');
    }

    async function showClockHandler(agent) {
        const state = {
            'id': uuidv1(),
            'mode': 'showClock',
        };
        const request = generateRequest(state);
        await client.modifyCloudToDeviceConfig(request);
        agent.add('Режим часы');
    }

    // Run the proper function handler based on the matched Dialogflow intent name
    let intentMap = new Map();
    intentMap.set('TurnOn', turnOnHandler);
    intentMap.set('TurnOff', turnOffHandler);
    intentMap.set('showRainbow', showRainbowHandler);
    intentMap.set('showAdv', showAdvHandler);
    intentMap.set('showClock', showClockHandler);
    agent.handleRequest(intentMap);
});
