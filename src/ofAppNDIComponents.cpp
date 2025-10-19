#ifndef __EMSCRIPTEN__
//
//  ofAppNDIComponents.cpp
//  orgb
//
//  Created by Kevin Katz on 8/28/22.
//

#include "ofApp.h"

// Return true iff scan finds video and connection succeeds.
bool ofApp::ndiScanForVideoSources() {
    auto sources = finder_.getSources();

    if (sources.size() == 0) {
        ofLogVerbose("NDI") << "Did not detect sources";
        return false;
    }
    std::vector<std::string> sourceStrs;
    for (auto & x : sources) {
        sourceStrs.push_back(x.p_url_address + " " + x.p_ndi_name);
    }
    ofLogNotice("NDI") << "Got sources: " << ofJoinString(sourceStrs, ", ");

    std::string hostnameSelector = "TouchDesignerTo" + getEnv("SYSTEM_HOSTNAME", "HOSTNAME_NOT_FOUND");
    for (auto & source : sources) {
        if (ofStringTimesInString(source.p_ndi_name, hostnameSelector) > 0) {
            ofLogNotice("NDI") << "Found hostname selector match: " << hostnameSelector << " in " << source.p_ndi_name;
            return ndiSwitchToVideoSource(source);
        }
    }

    std::string resolutionSelector = "TouchDesigner" + getEnv("OFW_WIDTH", "160") + "x" + getEnv("OFW_HEIGHT", "32");
    for (auto & source : sources) {
        if (ofStringTimesInString(source.p_ndi_name, resolutionSelector) > 0) {
            ofLogNotice("NDI") << "Found resolution selector match: " << resolutionSelector << " in "
                               << source.p_ndi_name;
            return ndiSwitchToVideoSource(source);
        }
    }

    std::string fallbackSelector = "TouchDesignerAll";
    for (auto & source : sources) {
        if (ofStringTimesInString(source.p_ndi_name, fallbackSelector) > 0) {
            ofLogNotice("NDI") << "Found fallback selector match: " << fallbackSelector << " in " << source.p_ndi_name;
            return ndiSwitchToVideoSource(source);
        }
    }

    return false;
}

// Return true iff video setup succeeds.
bool ofApp::ndiSwitchToVideoSource(ofxNDI::Source & source) {
    ofLogNotice("NDI") << "Switching to " << source.p_ndi_name << " " << source.p_url_address;
    if (receiver_.isSetup()) {
        //            ofLogNotice("NDI") << "Receiver is already set up. Changing connection...";
        //            receiver_.changeConnection(sources[index]);
        //            ofLogVerbose("NDI") << "Receiver changeConnection complete.";
        //            setupSuccessful = true;
        ofLogNotice("NDI") << "Receiver was already set up. Changing connection...";
        receiver_.changeConnection(source);
        ofLogVerbose("NDI") << "Connection change complete.";
        return true;
    } else {
        ofLogNotice("NDI") << "Receiver is not set up. Setting up receiver...";
        bool setupSuccessful = receiver_.setup(source);
        if (setupSuccessful) {
            // First time setup. Video is not setup so we need to set it up.
            ofLogNotice("NDI") << "Receiver setup was successful. Setting up video...";
            video_.setup(receiver_);
            ofLogNotice("NDI") << "Video setup complete.";
            return true;
        } else {
            ofLogWarning("NDI") << "Setup failed. Not setting up video.";
            return false;
        }
    }
}

// Return true iff connected.
bool ofApp::ndiUpdateHandler() {
    /*
     TODO HUH
     auto sources = finder_.getSources();
     if(sources.size() > index) {
         if(receiver_.isSetup() ? (receiver_.changeConnection(sources[index]), true) : receiver_.setup(sources[index]))
     { video_.setup(receiver_);
         }
     }
     */
    if (receiver_.isConnected()) {
        ofLogVerbose("NDI") << "NDI Receiver connected, updating video...";
        video_.update();
        if (video_.isFrameNew()) {
            ofLogVerbose("NDI") << "NDI Receiver received new frame, decoding to pixels...";
            video_.decodeTo(pixels_);
        }
        return true;
    } else {
        // Receiver is not connected or set up
        if (pixels_.isAllocated()) {
            ofLogVerbose("NDI") << "NDI Receiver disconnected but pixels is allocated. Clearing...";
            pixels_.clear();
        }
        double scheduledAttemptDue =
            (lastNdiReconnectAttempt.has_value() ? lastNdiReconnectAttempt.value() : 0) +
            stoi(getEnv("NDI_SCAN_INTERVAL_SECONDS", "10"));
        if (scheduledAttemptDue < getSystemTimeSecondsPrecise()) {
            ofLogVerbose("NDI")
                << "Receiver is not connected and interval elapsed. Attempting to select video source...";
            double t0 = getSystemTimeSecondsPrecise();
            ndiScanForVideoSources();
            warnOnSlow("NDI Scan for Video", t0, WARN_INTERVAL_DENOMINATOR_NDI_SCAN, ofGetFrameNum(),
                       ofGetElapsedTimef());
            // If this switches, then we'll show video next frame.
        }
        return false;
    }
}

// Return true iff allocated. Will only be allocated with active stream.
bool ofApp::ndiDrawHandler() {
    if (pixels_.isAllocated()) {
        ofLogVerbose("NDI") << "Pixels is allocated, drawing pixels.";
        float xCropWidth = (pixels_.getWidth() - ofGetWidth()) / 2.0;
        float yCropWidth = (pixels_.getHeight() - ofGetHeight()) / 2.0;
        ofImage(pixels_).drawSubsection(0.0f, 0.0f, ofGetWidth(), ofGetHeight(), xCropWidth, yCropWidth);

        // ofImage(pixels_).draw(0,0);
        return true;
    } else {
        return false;
    }
}

#endif  // __EMSCRIPTEN__
