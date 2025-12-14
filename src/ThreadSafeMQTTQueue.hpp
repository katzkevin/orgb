//
//  ThreadSafeMQTTQueue.hpp
//  orgb
//
//  Thread-safe queue for MQTT messages
//

#ifndef ThreadSafeMQTTQueue_hpp
#define ThreadSafeMQTTQueue_hpp

#ifdef HAS_MQTT

#include <mutex>
#include <queue>
#include <string>

// Simple struct to hold MQTT message data for queue transfer
struct MQTTQueueMessage {
    std::string topic;
    std::string payload;
};

class ThreadSafeMQTTQueue {
   public:
    ThreadSafeMQTTQueue() : maxSize_(1000) {}  // Prevent unbounded growth

    // Add message to queue (called from MQTT thread)
    void push(const MQTTQueueMessage & message) {
        std::lock_guard<std::mutex> lock(mutex_);

        // Drop oldest messages if queue is full
        if (queue_.size() >= maxSize_) {
            queue_.pop();
            droppedCount_++;
        }

        queue_.push(message);
    }

    // Try to pop a message (called from main thread)
    bool tryPop(MQTTQueueMessage & message) {
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
    std::queue<MQTTQueueMessage> queue_;
    size_t maxSize_;
    size_t droppedCount_ = 0;
};

#endif  // HAS_MQTT

#endif /* ThreadSafeMQTTQueue_hpp */
