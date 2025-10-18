//
//  ShaderHotReloader.hpp
//  orgb
//
//  Hot reload shaders when files change (industry standard practice)
//

#ifndef ShaderHotReloader_hpp
#define ShaderHotReloader_hpp

#include <chrono>
#include <filesystem>
#include <functional>
#include <map>
#include <string>

#include "ofMain.h"

namespace fs = std::filesystem;

class ShaderHotReloader {
   public:
    ShaderHotReloader() : enabled(true), checkInterval(0.5f), lastCheck(0) {}

    // Register a shader for hot reloading
    void watch(ofShader * shader, const std::string & vertPath, const std::string & fragPath,
               std::function<void()> onReload = nullptr) {
        WatchEntry entry;
        entry.shader = shader;
        entry.vertPath = vertPath;
        entry.fragPath = fragPath;
        entry.onReload = onReload;

        updateModificationTimes(entry);
        watchList[vertPath + "|" + fragPath] = entry;

        ofLogNotice("ShaderHotReloader") << "Watching: " << vertPath << " + " << fragPath;
    }

    // Call this in update() loop
    void update() {
        if (!enabled) return;

        float now = ofGetElapsedTimef();
        if (now - lastCheck < checkInterval) return;
        lastCheck = now;

        for (auto & [key, entry] : watchList) {
            if (hasFileChanged(entry)) {
                reloadShader(entry);
            }
        }
    }

    void setEnabled(bool state) { enabled = state; }
    void setCheckInterval(float seconds) { checkInterval = seconds; }

   private:
    struct WatchEntry {
        ofShader * shader;
        std::string vertPath;
        std::string fragPath;
        std::time_t vertModTime;
        std::time_t fragModTime;
        std::function<void()> onReload;
    };

    bool enabled;
    float checkInterval;
    float lastCheck;
    std::map<std::string, WatchEntry> watchList;

    void updateModificationTimes(WatchEntry & entry) {
        entry.vertModTime = getModificationTime(entry.vertPath);
        entry.fragModTime = getModificationTime(entry.fragPath);
    }

    std::time_t getModificationTime(const std::string & path) {
        try {
            if (!fs::exists(path)) return 0;
            auto ftime = fs::last_write_time(path);
            return std::chrono::system_clock::to_time_t(
                std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()));
        } catch (...) {
            return 0;
        }
    }

    bool hasFileChanged(const WatchEntry & entry) {
        std::time_t vertMod = getModificationTime(entry.vertPath);
        std::time_t fragMod = getModificationTime(entry.fragPath);

        return (vertMod != entry.vertModTime) || (fragMod != entry.fragModTime);
    }

    void reloadShader(WatchEntry & entry) {
        ofLogNotice("ShaderHotReloader") << "Reloading: " << entry.vertPath;

        try {
            // Attempt reload
            bool success = entry.shader->load(entry.vertPath, entry.fragPath);

            if (success) {
                updateModificationTimes(entry);
                ofLogNotice("ShaderHotReloader") << "✓ Reload successful";

                // Trigger callback
                if (entry.onReload) {
                    entry.onReload();
                }
            } else {
                ofLogError("ShaderHotReloader") << "✗ Reload failed - keeping old shader";
            }
        } catch (const std::exception & e) {
            ofLogError("ShaderHotReloader") << "✗ Exception during reload: " << e.what();
        }
    }
};

#endif /* ShaderHotReloader_hpp */
