//
//  ThreadSafeOSCQueue.hpp
//  orgb
//
//  Thread-safe queue for OSC messages
//

#ifndef ThreadSafeOSCQueue_hpp
#define ThreadSafeOSCQueue_hpp

#include <mutex>
#include <queue>

#include "ofxOsc.h"

class ThreadSafeOSCQueue {
   public:
    ThreadSafeOSCQueue() : maxSize_(1000) {}  // Prevent unbounded growth

    // Add message to queue (called from OSC thread)
    void push(const ofxOscMessage & message) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Drop oldest messages if queue is full
        if (queue_.size() >= maxSize_) {
            queue_.pop();
            droppedCount_++;
        }

        queue_.push(message);
    }

    // Try to pop a message (called from main thread)
    bool tryPop(ofxOscMessage & message) {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.empty()) {
            return false;
        }

        message = queue_.front();
        queue_.pop();
        return true;
    }

    // Get current queue size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    // Check if queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Get and reset dropped message count
    size_t getAndResetDroppedCount() {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = droppedCount_;
        droppedCount_ = 0;
        return count;
    }

    // Clear all messages
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

   private:
    mutable std::mutex mutex_;
    std::queue<ofxOscMessage> queue_;
    size_t maxSize_;
    size_t droppedCount_ = 0;
};

#endif /* ThreadSafeOSCQueue_hpp */
