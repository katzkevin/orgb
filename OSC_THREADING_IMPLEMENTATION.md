# OSC Threading Implementation

## Summary

OSC message processing has been moved to a background thread to eliminate the performance warnings:
```
[warning] warnOnSlow: OSC IO running slowly: 26.86ms (Target: 4.16667ms)
```

## Changes Made

### 1. New Files Created
- **`src/ThreadSafeOSCQueue.hpp`** - Thread-safe queue for passing OSC messages between threads
- **`scripts/test_osc_sender.py`** - Python script for testing OSC performance
- **`scripts/requirements-osc-test.txt`** - Python dependencies
- **`scripts/README_OSC_TESTING.md`** - Testing instructions

### 2. Modified Files

**`src/ofApp.h`**
- Added threading includes (`<atomic>`, `<thread>`)
- Added `ThreadSafeOSCQueue oscMessageQueue`
- Added `std::thread oscThread` and `std::atomic<bool> oscThreadRunning`
- Added new methods: `oscThreadFunction()`, `processQueuedOSCMessages()`, `startOSCThread()`, `stopOSCThread()`

**`src/ofAppOSCComms.cpp`**
- Implemented `oscThreadFunction()` - runs in background, polls receiver and enqueues messages
- Implemented `processQueuedOSCMessages()` - processes messages from queue in main thread
- Implemented `startOSCThread()` and `stopOSCThread()` for lifecycle management
- Kept original `pollForOSCMessages()` as fallback reference

**`src/ofApp.cpp`**
- Modified `setup()` to call `startOSCThread()` after receiver setup
- Modified `exit()` to call `stopOSCThread()` for clean shutdown
- Modified `update()` to call `processQueuedOSCMessages()` instead of `pollForOSCMessages()`

## How It Works

### Before (Synchronous)
```
update() → pollForOSCMessages() → blocks while processing all messages → continues
          └─ JSON parsing, logging, handler calls all in main thread
```

### After (Threaded)
```
Background Thread:               Main Thread (update()):
┌─────────────────┐             ┌──────────────────────────┐
│ oscThreadFunction│             │ processQueuedOSCMessages │
│                 │             │                          │
│ while running:  │             │ Pop up to 100 messages   │
│   poll receiver │──enqueue──→ │ Process each message     │
│   push to queue │             │ Call handlers            │
│   sleep 1ms     │             │ Check queue health       │
└─────────────────┘             └──────────────────────────┘
```

### Benefits
1. **Non-blocking receive** - OSC receiving happens continuously in background
2. **Bounded processing** - Main thread processes max 100 messages per frame
3. **Queue overflow protection** - Drops oldest messages if queue exceeds 1000
4. **Performance monitoring** - Warns if queue is backing up or dropping messages

## Testing

### 1. Build and run your application
```bash
make -j4
make RunRelease
```

### 2. In another terminal, run the test sender
```bash
# Light load (50 msg/s)
uv run scripts/test_osc_sender.py --load light

# Medium load (100 msg/s)
uv run scripts/test_osc_sender.py --load medium

# Heavy load (300 msg/s) - stress test
uv run scripts/test_osc_sender.py --load heavy
```

### Expected Results

**Before:** You would see warnings like:
```
[warning] warnOnSlow: OSC IO running slowly: 26.86ms (Target: 4.16667ms)
```

**After:**
- Warnings should disappear or be much less frequent
- Processing time should be well under 4.16ms
- Frame rate should remain stable at 60fps
- Under heavy load, you may see queue warnings instead:
  ```
  [warning] OSC: Queue backing up: 52 messages pending
  ```

## Configuration

### Queue Size
Default: 1000 messages. Modify in `ThreadSafeOSCQueue.hpp`:
```cpp
ThreadSafeOSCQueue() : maxSize_(1000) {}
```

### Messages Per Frame
Default: 100 messages/frame. Modify in `ofAppOSCComms.cpp`:
```cpp
const int maxMessagesPerFrame = 100;
```

### Thread Sleep Time
Default: 1ms. Modify in `ofAppOSCComms.cpp`:
```cpp
std::this_thread::sleep_for(std::chrono::milliseconds(1));
```

## Reverting to Non-Threaded (If Needed)

If you encounter issues, you can revert to the old synchronous behavior:

**In `src/ofApp.cpp`:**
```cpp
// setup() - comment out threading
#ifndef __EMSCRIPTEN__
    receiver.setup(OSC_PORT);
    // startOSCThread();  // DISABLED
#endif

// update() - use old function
#ifndef __EMSCRIPTEN__
    pollForOSCMessages();  // Instead of processQueuedOSCMessages()
#endif

// exit() - comment out thread cleanup
#ifndef __EMSCRIPTEN__
    // stopOSCThread();  // DISABLED
#endif
```

## Thread Safety Notes

- `ofxOscReceiver` appears to be thread-safe for concurrent reads
- All message processing (JSON parsing, handler calls) still happens in main thread
- OpenFrameworks drawing/GL calls remain single-threaded
- Queue uses `std::mutex` for cross-thread synchronization

## Performance Tuning

If you still see warnings:

1. **Increase messages per frame** (if you have CPU headroom)
2. **Reduce logging** (remove conditional logs for guitar/mic)
3. **Optimize JSON parsing** (pre-allocate, cache structures)
4. **Batch handler calls** (collect notes, call once per frame)

## Files Reference

| File | Purpose |
|------|---------|
| `src/ThreadSafeOSCQueue.hpp` | Thread-safe message queue |
| `src/ofApp.h:135-145` | Threading member variables |
| `src/ofAppOSCComms.cpp:21-181` | Threading implementation |
| `src/ofApp.cpp:199` | Thread start |
| `src/ofApp.cpp:309` | Queue processing |
| `src/ofApp.cpp:439` | Thread stop |
