//
//  ofAppOSCComms.cpp
//  orgb
//
//  Created by Kevin Katz on 9/5/22.
//

#include "Utilities.hpp"
#include "ofApp.h"

using json = nlohmann::json;

void OfApp::pollForOSCMessages() {
    double t0 = getSystemTimeSecondsPrecise();
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if (monitorFrameRateMode) {
            warnOnSlow("OSC IO getNextMessage", t0, TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_OSC_IO,
                       ofGetFrameNum(), ofGetElapsedTimef());  // Check every update frame
        }

        if (m.getAddress() == MIDI_GUITAR_OSC_ADDRESS) {
            if (ofGetFrameNum() % 400 == 0) {
                ofLogNotice("OSC") << "(midi-guitar, 1/400th) " << m;
            }
        } else if (m.getAddress() == MIC0_PITCH_OSC_ADDRESS) {
            if (ofGetFrameNum() % 400 == 0) {
                ofLogVerbose("OSC") << "(mic-0, 1/400th) " << m;
            }
        } else {
            ofLogNotice("OSC") << m;
        }

        try {
            if (m.getAddress() == MIDI_KEYBOARD_OSC_ADDRESS) {
                std::string messageKey;

                json j = nlohmann::json::object();
                std::string messageType = m.getArgAsString(0);
                j["type"] = messageType;

                switch (stringToMidiType(messageType)) {
                    case MIDITYPE::NOTE_ON:
                    case MIDITYPE::NOTE_OFF:
                        j["channel"] = m.getArgAsInt(1);
                        j["note"] = m.getArgAsInt(2);
                        j["velocity"] = m.getArgAsInt(3);
                        break;
                    case MIDITYPE::CONTROL_CHANGE:
                        j["channel"] = m.getArgAsInt(1);
                        j["control"] = m.getArgAsInt(2);
                        j["value"] = m.getArgAsInt(3);
                        j["time"] = m.getArgAsInt(4);
                        break;
                    case MIDITYPE::PITCHWHEEL:
                        j["channel"] = m.getArgAsInt(1);
                        j["pitch"] = m.getArgAsInt(2);
                        j["time"] = m.getArgAsInt(3);
                        break;
                    case MIDITYPE::KEYBOARD_ON:
                        j["key"] = m.getArgAsString(1);
                        break;
                    case MIDITYPE::KEYBOARD_OFF:
                        j["key"] = m.getArgAsString(1);
                        break;
                    default:
                        ofLogWarning() << "Unhandled OSC Message: " << messageType;
                }
                jsonHandlerMidiMessage(j);

            } else if (m.getAddress() == MIDI_GUITAR_OSC_ADDRESS) {
                json j = nlohmann::json::object();
                if (m.getNumArgs() <= 1) {
                    ofLogNotice("OSC") << "Got " << m.getNumArgs() << "-length " << MIDI_GUITAR_OSC_ADDRESS
                                       << " message. Skipping.";
                    continue;
                }
                int messageId = m.getArgAsInt(m.getNumArgs() - 1);  // Last element is an ID
                for (int i = 0; i < static_cast<int>(m.getNumArgs()) - 1; i += 2) {
                    if (m.getArgAsInt(i + 1) > 127) {
                        ofLogVerbose("OSC") << "Guitar pitch velocity greater than 127: " << m.getArgAsInt(i + 1);
                    }
                    // Right now only distinction with this an mic is scaling down
                    j[std::to_string(m.getArgAsInt(i))] = ofMap(m.getArgAsInt(i + 1), 0, MIDI_NOTE_MAX, 0, 1, true);
                }
                jsonHandlerEphemeralNote(j, messageId);
            } else if (m.getAddress() == MIC0_PITCH_OSC_ADDRESS) {
                if (m.getNumArgs() == 0) {
                    ofLogWarning("OSC") << "Received 0 arg message: " << m;
                    continue;
                }

                json j = nlohmann::json::object();

                //                if (ofGetFrameNum() % 20 == 0) {
                //                    // Unlike guitar, these come through with a timetag
                //                    uint64_t tagNineteenHundredTimeSecs =
                //                        (m.getArgAsTimetag(0) & 0xFFFFFFFF00000000) >> 32;                 // Stored
                //                        in higher 32 bits
                //                    uint64_t tagEpochTimeSecs = tagNineteenHundredTimeSecs - 2208988800L;  // Time
                //                    between 1900 and 1970 uint64_t tagTimeFrac = (m.getArgAsTimetag(0) &
                //                    0x00000000FFFFFFFF);    // Stored in higher 32 bits uint64_t tagTimeMicroseconds =
                //                        (uint32_t)((double)tagTimeFrac * 1.0e6 /
                //                                   (double)(1LL << 32));  //
                //                                   https://tickelton.gitlab.io/articles/ntp-timestamps/
                //                    uint64_t tagEpochTimeTotalMicrosecond = tagEpochTimeSecs * 1000000 +
                //                    tagTimeMicroseconds; uint64_t nowTotalMicrosecond =
                //                    std::chrono::duration_cast<std::chrono::microseconds>(
                //                                                       std::chrono::system_clock::now().time_since_epoch())
                //                                                       .count();
                //                    float totalLag = ((nowTotalMicrosecond - tagEpochTimeTotalMicrosecond) / 1.0e6);
                //                    //                    ofLogVerbose("OSC") << tagNineteenHundredTimeSecs << " " <<
                //                    tagEpochTimeSecs
                //                    //                    << " " << tagTimeFrac << " " << tagTimeMicroseconds << " "
                //                    <<
                //                    //                    tagEpochTimeTotalMicrosecond << " " << nowTotalMicrosecond;
                //                    ofLogVerbose("OSC") << "(mic delay, 1/20th) " << totalLag;
                //                }

                unsigned int messageId = m.getArgAsInt(m.getNumArgs() - 1);  // Last element is an ID

                // Argument 0 and 1 are the time tag, fetch through 0 although it's listed as type "tsff"
                for (int i = 1; i < static_cast<int>(m.getNumArgs()) - 2; i += 2) {
                    j[std::to_string(std::lround(m.getArgAsFloat(i)))] = m.getArgAsFloat(i + 1);
                }

                jsonHandlerEphemeralNote(j, messageId);
            } else if (m.getAddress() == SETTINGS_ADDRESS) {
                auto x = m.getArgAsString(0);
                ofLogVerbose("OSC") << "Received SETTINGS message: " << x;
                loadSettingsFromJsonString(x);
            }

        } catch (nlohmann::detail::parse_error e) {
            ofLogWarning("OSC") << "Skipping parse error: " << e.what();
        }
    }

    warnOnSlow("OSC IO", t0, TARGET_FRAME_TIME_S / WARN_INTERVAL_DENOMINATOR_OSC_IO, ofGetFrameNum(),
               ofGetElapsedTimef());  // Check every update frame
}
