//
//  ofAppMqttComms.cpp
//  orgb
//
//  Created by Kevin Katz on 3/23/22.
//

#include <ofApp.h>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

// https://github.com/256dpi/ofxMQTT/blob/master/example-ofxMQTT/src/ofApp.h

void ofApp::mqttConnectHandler() {
    std::string mqttHost = getEnv("MOSQUITTO_HOST", "");
    std::string mqttUser = getEnv("MOSQUITTO_USER", "");
    std::string mqttPassword = getEnv("MOSQUITTO_PASSWORD", "");
    std::string mqttClientId = getEnv("MOSQUITTO_CLIENT_ID", "");

    if (mqttHost.empty() || mqttUser.empty() || mqttPassword.empty() || mqttClientId.empty()) {
        ofLogNotice("MQTT") << "MQTT configuration not found in environment variables. MQTT disabled.";
        mqttClientConnectedSuccessfully = false;
        return;
    }

    client.begin(mqttHost, 1883);
    std::string ranString = "-" + std::to_string((int)ofRandom(99999));
    mqttClientConnectedSuccessfully = client.connect(mqttClientId + ranString, mqttUser, mqttPassword);
    if (mqttClientConnectedSuccessfully) {
        ofAddListener(client.onOnline, this, &ofApp::mqttOnOnline);
        ofAddListener(client.onOffline, this, &ofApp::mqttOnOffline);
        ofAddListener(client.onMessage, this, &ofApp::mqttOnMessage);
    } else if (requireMQTT) {
        ofLogWarning("MQTT") << "MQTT Conenction failed. Restarting...";
        ofSleepMillis(5000);
        ofExit(0);
    }
}

void ofApp::dumpSettingsToMqtt() {
    if (!mqttClientConnectedSuccessfully) {
        ofLogNotice("MQTT") << "MQTT Client not connected, skipping dumpSettingsToMqtt.";
        return;
    } else {
        ofLogNotice("MQTT") << "Writing settings to " << SETTINGS_MQTT_TOPIC;   
    }
    ofJson json;
    gui.saveTo(json);
    client.publish(SETTINGS_MQTT_TOPIC, json.dump());
}

void ofApp::pollForMQTTMessages() {
    if (!mqttClientConnectedSuccessfully) {
        return;
    }

    double t0 = getSystemTimeSecondsPrecise();

    // Check for twenty messages a second... WHAT? Why necessary. Hmm.

    while (true) {
        mqttMessageEncountered = false;
        // onMessage will set midiMessage encountered to true if there is a message.
        client.update();
        if (monitorFrameRateMode) {
            warnOnSlow("MQTT IO Update", t0,
                       TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_MQTT_IO,
                       ofGetFrameNum(), ofGetElapsedTimef());  // Check every update frame
        }
        // If there isn't a message at this point, the onMessage handler _wasn't invoked_, queue is empty.
        if (mqttMessageEncountered == false) {
            break;
        }
    }

    warnOnSlow("MQTT IO", t0, TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_MQTT_IO, ofGetFrameNum(), ofGetElapsedTimef());  // Check every update frame
    
}

void ofApp::mqttOnMessage(ofxMQTTMessage& msg) {
    // Update loop will set this to false

    mqttMessageEncountered = true;

    ofLogNotice("MQTT") << "Received message: [" << msg.topic << "] " << msg.payload;
        
    try {
        if (msg.topic == MIDI_GUITAR_MQTT_TOPIC) {
            ofLogError() << "MIDI Guitar over MQTT not implemented";
        } else if (msg.topic == CLASSIFIER_MQTT_TOPIC) {
            auto j = json::parse(msg.payload);
            jsonHandlerClassification(j);
        } else if (msg.topic == "midi" || msg.topic == "midi-piano") {
            auto j = json::parse(msg.payload);
            jsonHandlerMidiMessage(j);
        } else if (msg.topic == ORGB_PARAM_MQTT_TOPIC) {
            auto j = json::parse(msg.payload);
            jsonHandlerParamMessage(j);
        } else if (msg.topic == ORGB_OFPARAMETER_MQTT_TOPIC) {
            auto j = json::parse(msg.payload);
            jsonHandlerOfParamMessage(j);
        } else if (msg.topic == SETTINGS_MQTT_TOPIC) {
            loadSettingsFromJsonString(msg.payload);
        }
    } catch (nlohmann::detail::parse_error e) {
        ofLogWarning("MQTT") << "Skipping parse error: " << e.what();
    }
}

void ofApp::mqttOnOnline() {
    ofLogNotice("MQTT") << "Online.";
    client.subscribe("midi");
    client.subscribe("midi-guitar");
    client.subscribe(SETTINGS_MQTT_TOPIC);
    client.subscribe(ORGB_PARAM_MQTT_TOPIC);
    client.subscribe(CLASSIFIER_MQTT_TOPIC);
    client.subscribe(ORGB_OFPARAMETER_MQTT_TOPIC);
}

void ofApp::mqttOnOffline() { ofLogNotice("MIDI") << "Offline."; }
