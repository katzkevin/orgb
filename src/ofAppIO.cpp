//
//  ofAppIO.cpp
//  orgb
//
//  Created by Kevin Katz on 11/26/21.
//

#include <math.h>

#include <unordered_map>

#include "json.hpp"
#include "ofApp.h"

// for convenience
using json = nlohmann::json;

void ofApp::noteOnHandler(int key, float velocityPct, unsigned int messageId, bool ephemeral) {
    updateLastInteractionMoment();

    ofLogVerbose("IO") << "[" << key << "] " << velocityPct;

    if (ephemeral) {
        ks.ephemeralKeyPressedHandler(key, velocityPct, messageId);
    } else {
        std::optional<Press> activePress = ks.getActivePress(key);
        if (activePress.has_value()) {
            Press press = activePress.value();
            forms[currentFormIndex]->pressHandler(press);  // TODO This won't play nice with ephemeral
        } else {
            // This is a new press.
            Press press = ks.newKeyPressedHandler(key, velocityPct, messageId);

            std::optional<int> metaInput = ks.getMetaInput();
            if (metaInput.has_value()) {
                int metaInputIndex = key - (metaInput.value() + NUM_NOTES);

                switch (metaInputIndex) {
                    case 0:  // C
                        ofLogNotice() << "Meta Input: " << metaInputIndex;
                        previousForm();
                        break;
                    case 1:  // C#
                        ofLogNotice() << "Meta Input: " << metaInputIndex;
                        nextForm();
                        break;
                    case 2:  // D
                        ofLogNotice() << "Meta Input: " << metaInputIndex << ", exiting...";
                        ofExit(0);
                        break;
                    default:  //
                        break;
                }
            }

            forms[currentFormIndex]->newPressHandler(clr, press);
            forms[currentFormIndex]->pressHandler(press);  // PressHandler always invoked.
        }
    }
}

void ofApp::noteOffHandler(int key) {
    ofLogVerbose("IO") << "[" << key << "] released";
    ks.keyReleasedHandler(key);
}

std::string ofApp::dumpSettingsToJsonFile() {
    ofLogNotice() << "Dumping settings to JSON.";
    std::string settingsName = "settings_" + ofGetTimestampString() + ".json";

    // There is a bug in ofxBaseGui.cpp saveToFile, file must be created and filled with valid json before saved
    // to if we're using JSON instead of xml.

    ofBuffer dataBuffer;
    dataBuffer.set("{}");
    // fill the buffer with something important
    ofBufferToFile(settingsName, dataBuffer);

    gui.saveToFile(settingsName);
    return settingsName;
}

void ofApp::loadSettingsFromJsonString(std::string payloadString) {
    json j = json::parse(payloadString);
    ofLogNotice() << "Loading JSON settings.";
    gui.loadFrom(j);
}

void ofApp::jsonHandlerEphemeralNote(nlohmann::basic_json<> & j, unsigned int messageId) {
    std::unordered_map<int, float> incomingPresses;
    std::unordered_map<int, unsigned int> messageIds;
    if (incomingPresses.size()) {
        updateLastInteractionMoment();
    }
    for (auto & elt : j.items()) {
        std::string key = elt.key();
        float value = elt.value();
        messageIds[std::stoi(key)] = messageId + std::stoi(key);  // Ultimately the ID is offset from the note.
        incomingPresses.emplace(std::stoi(key), value);
    }

    ks.ephemeralKeyPressMapHandler(incomingPresses, messageIds);
}

void ofApp::jsonHandlerClassification(nlohmann::basic_json<> & j) {
    // {"type": "arousal", "value": 0.5, "time": "2022-05-25T21:15:21.729171+00:00"}

    float messageValue = NAN;
    CLASSIFICATIONTYPE messageTypeEnum = stringToClassificationType(j.at("type"));

    switch (messageTypeEnum) {
        case CLASSIFICATIONTYPE::AROUSAL:
            messageValue = j.at("value");
            ks.setArousalPct(messageValue);
            break;
        default:
            ofLogWarning() << "Unhandled classification type: " << j.at("type");
            break;
    }
}

void ofApp::jsonHandlerMidiMessage(nlohmann::basic_json<> & j) {
    int messageNote = 0;
    int messageVelocity = 0;
    int messageChannel = 0;
    int messageValue = 0;
    int messageControl = 0;
    int messageId = 0;
    bool ephemeral = false;

    std::string messageKey;

    std::string messageForm;
    std::string messageParameter;
    std::string messageParameterType;
    float messageParameterValue = NAN;

    MIDITYPE messageTypeEnum = stringToMidiType(j.at("type"));

    switch (messageTypeEnum) {
        case MIDITYPE::NOTE_ON:
            messageNote = j.at("note");
            messageVelocity = j.at("velocity");
            ephemeral = j.value("ephemeral", false);
            messageId = j.at("id");
            //            if (messageChannel != 0) {
            //                break;  // Legato Piano observed issuing channel 2
            //            }

            if (messageNote == 107) {
                previousForm();
            } else if (messageNote == 108) {
                nextForm();
            } else if (messageNote == 109) {
                ofLogWarning() << "Triggering 100 notes at once...";
                for (int i = 0; i < 100; i++) {
                    noteOnHandler(i, 1.0, messageId + i, false);
                }
            } else if (messageNote == 128) {
                debugModeMoment = getSystemTimeSecondsPrecise();
            } else if (messageNote == 129) {
                amperageTestModeMoment = getSystemTimeSecondsPrecise();
            } else {
                noteOnHandler(messageNote, ofMap(messageVelocity, 0, MIDI_NOTE_MAX, 0, 1), messageId, ephemeral);
            }
            break;
        case MIDITYPE::NOTE_OFF:
            messageNote = j.at("note");
            messageChannel = j.at("channel");
            if (messageChannel != 0) {
                break;  // Legato Piano observed issuing channel 2
            } else if (messageNote == 107) {
                // Corresponds to previousForm();
            } else if (messageNote == 108) {
                // Corresponds to nextForm();
            } else if (messageNote == 109) {
                for (int i = 0; i < 100; i++) {
                    noteOffHandler(i);
                }
            } else {
                noteOffHandler(messageNote);
            }
            break;
        case MIDITYPE::KEYBOARD_ON:
            messageKey = j.at("key");
            keyPressed(messageKey[0]);
            break;
        case MIDITYPE::KEYBOARD_OFF:
            messageKey = j.at("key");
            keyReleased(messageKey[0]);
            break;
        case MIDITYPE::CONTROL_CHANGE:
            messageControl = j.at("control");
            messageValue = j.at("value");
            messageChannel = j.at("channel");
            if (messageControl == 1 && messageValue == 0) {
                // Vmini Mod (Ramp)
                nextForm();
            } else if (messageControl == 14) {
                // Vmini Dial 0
                ks.setArousalPct(ofMap(messageValue, 0, 127, 0, 1));
            } else if (messageControl == 15) {
                // Vmini Dial 1
                ks.setValencePct(ofMap(messageValue, 0, 127, 0, 1));
            } else if (messageControl == 64 && messageChannel == 0) {
                // Sustain
                // (Channel 0 because sustains observed to come in on three channels at once. Only
                // trust the zero channel.)
                if (messageValue >= 64) {
                    ks.sustainOnHandler(getSystemTimeSecondsPrecise());
                } else {
                    ks.sustainOffHandler(getSystemTimeSecondsPrecise());
                }
            }
            break;
        case MIDITYPE::GUI_PARAMETER_CHANGE:
            // TODO Fix with this approach
            // https://forum.openframeworks.cc/t/how-to-get-the-ofparameter-from-the-ofevent-of-parameterchangede/20660/2
            ofLogNotice() << "GUI Parameter Change Received: " << j.dump();
            messageForm = j.at("form");
            messageParameter = j.at("parameter");
            messageParameterType = j.at("parameterType");
            messageParameterValue = j.at("value");
            for (auto form : forms) {
                if (form->name == messageForm) {
                    if (messageParameterType == "float") {
                        auto fFloat = form->parameters.getFloat(messageParameter);
                        if (fFloat) {
                            fFloat.set(messageParameterValue);
                        }
                    } else if (messageParameterType == "int") {
                        auto fInt = form->parameters.getInt(messageParameter);
                        if (fInt) {
                            fInt.set(static_cast<int>(messageParameterValue));
                        }
                    }
                    break;
                }
            }
            break;
        case MIDITYPE::SYSEX:
            break;
        case MIDITYPE::PROGRAMCHANGE:
            break;
        default:
            ofLogWarning() << "Unhandled message type: " << j.at("type");
            break;
    }
}

void ofApp::jsonHandlerParamMessage(nlohmann::basic_json<> & j) {
    float messageValue = NAN;
    std::vector<std::vector<int>> colorRgbsRaw;
    std::vector<ofColor> colors;
    PARAMTYPE messageTypeEnum = stringToParamType(j.at("type"));

    switch (messageTypeEnum) {
        case PARAMTYPE::COLORS:
            colorRgbsRaw = j["value"].get<std::vector<std::vector<int>>>();
            if (colorRgbsRaw.size() == NUM_NOTES) {
                for (auto & it : colorRgbsRaw) {
                    if (it.size() == 3) {
                        colors.push_back(ofColor(it.at(0), it.at(1), it.at(2)));
                    } else {
                        ofLogError() << "Invalid color values, num elements: " << it.size();
                        return;
                    }
                }
                clr.setPalette(colors);
            } else {
                ofLogError() << "Invalid number of colors: " << colorRgbsRaw.size();
            }
            break;
        default:
            ofLogWarning() << "Unhandled classification type: " << j.at("type");
            break;
    }
}

void ofApp::jsonHandlerOfParamMessage(nlohmann::basic_json<> & j) {
    ofLogNotice("IO") << "Received message: " << j;
    //    ofxBaseGui * colorGroup = gui.getControl("Color");
    //    if (!colorGroup) {
    //        ofLogError() << "'Color' not found";
    //        return;
    //    }
    //    ofAbstractParameter * p = (ofAbstractParameter *) colorGroup;
    //    ofLogNotice() << "p is type: " << p->type();
    //    if (p->type() == typeid(ofParameterGroup).name()) {
    //        ofLogNotice() << "Type ID Match";
    //        ofParameterGroup *g = static_cast<ofParameterGroup*>(p);
    //    } else {
    //        ofLogNotice() << "Type ID Mismatch";
    //        return;
    //    }

    //    ofLogNotice() << ofJoinString(gui.getControlNames(), ", ");
    //    ofxBaseGui * colorGroup = gui.getControl("Color");
    //    if (!colorGroup) {
    //        ofLogError() << "'Color' group not found";
    //        return;
    //    }
    //
    //    else if (colorGroup->getParameter().type() == typeid(ofParameter<float>).name()) {
    //        ofLogNotice() << "Type ID Match";
    //        float v = colorGroup->getParameter().cast<float>();
    //        ofLogNotice() << "Got! " << v;
    //    } else {
    //        ofLogNotice() << "Type ID Mismatch";
    //        return;
    //    }
    std::string group;
    std::string name;
    float valuePct = NAN;
    try {
        group = j.at("group");
        name = j.at("name");
        valuePct = j.at("valuePct");
    } catch (nlohmann::detail::out_of_range e) {
        ofLogWarning() << "JSON Parse error: " << e.what();
    }

    bool validName = name == "Base Hue" || name == "Max Hue" || name == "Base Saturation" || name == "Max Saturation" ||
                     name == "Base Value" || name == "Max Value" || name == "Clockwise" || name == "Cyclical";
    if (group != "Color" || !validName) {
        ofLogNotice() << "Invalid parameter combo. Ignoring.";
        return;
    }

    ofParameterGroup * g = getParameterGroup(group);
    if (!g) {
        ofLogWarning() << group << " not found. Discarding message.";
        return;
    }

    // TODO This doesn't work with subclassed ofParams.

    ofAbstractParameter * p;
    try {
        p = &(g->get(name));
    } catch (...) {
        ofLogWarning() << "Got exception fetching " << name << " from " << group << ". Discarding message.";
        return;
    }

    if (!p) {
        ofLogWarning() << name << " not found in " << group << ". Discarding message.";
        // return;
    }

    if (p->type() == typeid(ofParameter<float>).name()) {
        ofParameter<float> * pa = static_cast<ofParameter<float> *>(p);
        float paramMin = pa->getMin();
        float paramMax = pa->getMax();
        ofLogNotice() << "Param is float " << paramMin << " to " << paramMax;
        p->cast<float>() = ofMap(valuePct, 0, 1, paramMin, paramMax, true);
    }
    if (p->type() == typeid(ofParameter<int>).name()) {
        ofParameter<int> * pa = static_cast<ofParameter<int> *>(p);
        int paramMin = pa->getMin();
        int paramMax = pa->getMax();
        ofLogNotice() << "Param is int " << paramMin << " to " << paramMax;
        p->cast<int>() = ofMap(valuePct, 0, 1, paramMin, paramMax, true);
    }
    if (p->type() == typeid(ofParameter<bool>).name()) {
        ofLogNotice() << "Param is bool";
        p->cast<bool>() = 0.5 <= valuePct;
    }
}

ofParameterGroup * ofApp::getParameterGroup(std::string groupName) {
    ofxBaseGui * controlGroup = gui.getControl(groupName);
    if (!controlGroup) {
        ofLogError() << groupName << " not found in gui.getControl (top level groups): "
                     << ofJoinString(gui.getControlNames(), ", ");
        return nullptr;
    }

    ofLogVerbose() << "p is type: " << controlGroup->getParameter().type();
    if (controlGroup->getParameter().type() == typeid(ofParameterGroup).name()) {
        ofAbstractParameter * p = &(controlGroup->getParameter());
        ofParameterGroup * g = static_cast<ofParameterGroup *>(p);
        return g;
    } else {
        return nullptr;
    }
}
