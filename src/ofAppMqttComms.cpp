//
//  ofAppMqttComms.cpp
//  orgb
//
//  Created by Kevin Katz on 3/23/22.
//

#include <ofApp.h>

#ifdef HAS_MQTT

#include <chrono>
#include <thread>

#include "json.hpp"

// for convenience
using json = nlohmann::json;

// https://github.com/256dpi/ofxMQTT/blob/master/example-ofxMQTT/src/ofApp.h

// ====================
// Connection Setup
// ====================

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
        ofLogWarning("MQTT") << "MQTT Connection failed. Restarting...";
        ofSleepMillis(5000);
        ofExit(0);
    }
}

// ====================
// Threading Functions
// ====================

void ofApp::startMQTTThread() {
    if (!mqttClientConnectedSuccessfully) {
        ofLogNotice("MQTT") << "MQTT not connected, skipping thread start";
        return;
    }

    if (mqttThreadRunning) {
        ofLogWarning("MQTT") << "MQTT thread already running";
        return;
    }

    mqttThreadRunning = true;
    mqttThread = std::thread(&ofApp::mqttThreadFunction, this);
    ofLogNotice("MQTT") << "MQTT thread started";
}

void ofApp::stopMQTTThread() {
    if (!mqttThreadRunning) {
        return;
    }

    ofLogNotice("MQTT") << "Stopping MQTT thread...";
    mqttThreadRunning = false;

    if (mqttThread.joinable()) {
        mqttThread.join();
    }

    ofLogNotice("MQTT") << "MQTT thread stopped";
}

void ofApp::mqttThreadFunction() {
    ofLogNotice("MQTT") << "MQTT background thread running";

    while (mqttThreadRunning) {
        // client.update() triggers callbacks including mqttOnMessage
        // which will push messages to the queue
        client.update();

        // Small sleep to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ofLogNotice("MQTT") << "MQTT background thread exiting";
}

// Called from background thread during client.update()
void ofApp::mqttOnMessage(ofxMQTTMessage & msg) {
    // Just push to queue - don't process here (we're in background thread)
    MQTTQueueMessage queueMsg;
    queueMsg.topic = msg.topic;
    queueMsg.payload = msg.payload;
    mqttMessageQueue.push(queueMsg);
}

// Called from main thread to process queued messages
void ofApp::processQueuedMQTTMessages() {
    if (!mqttClientConnectedSuccessfully) {
        return;
    }

    int processedCount = 0;
    const int maxMessagesPerFrame = 100;  // Limit processing per frame

    MQTTQueueMessage msg;
    while (processedCount < maxMessagesPerFrame && mqttMessageQueue.tryPop(msg)) {
        processedCount++;
        processMQTTMessage(msg);
    }

    // Check queue status
    size_t queueSize = mqttMessageQueue.size();
    if (queueSize > 50) {
        ofLogWarning("MQTT") << "Queue backing up: " << queueSize << " messages pending";
    }

    size_t droppedCount = mqttMessageQueue.getAndResetDroppedCount();
    if (droppedCount > 0) {
        ofLogError("MQTT") << "Dropped " << droppedCount << " messages due to queue overflow!";
    }
}

// Actual message processing (runs in main thread)
void ofApp::processMQTTMessage(const MQTTQueueMessage & msg) {
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

// ====================
// Utility Functions
// ====================

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

void ofApp::mqttOnOnline() {
    ofLogNotice("MQTT") << "Online.";
    client.subscribe("midi");
    client.subscribe("midi-guitar");
    client.subscribe(SETTINGS_MQTT_TOPIC);
    client.subscribe(ORGB_PARAM_MQTT_TOPIC);
    client.subscribe(CLASSIFIER_MQTT_TOPIC);
    client.subscribe(ORGB_OFPARAMETER_MQTT_TOPIC);
}

void ofApp::mqttOnOffline() { ofLogNotice("MQTT") << "Offline."; }

#endif  // HAS_MQTT
