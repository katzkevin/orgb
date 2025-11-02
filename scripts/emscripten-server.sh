#!/bin/bash
# Emscripten HTTP server with proper cleanup and signal handling

# Configuration
PORT=8005
SERVER_DIR="bin/em"
URL="http://localhost:${PORT}/orgb/"

# Store the server PID
SERVER_PID=""

# Function to cleanup any existing servers on our port
cleanup_existing() {
    local existing_pids
    existing_pids=$(lsof -ti :${PORT} 2>/dev/null)

    if [ -n "$existing_pids" ]; then
        echo "Found existing process(es) on port ${PORT}, cleaning up..."
        echo "$existing_pids" | xargs -r kill -9 2>/dev/null || true
        sleep 0.5  # Give it a moment to release the port
    fi
}

# Function to cleanup on exit
cleanup() {
    echo ""
    echo "Shutting down server..."

    # Kill our server process if it's running
    if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        kill -TERM "$SERVER_PID" 2>/dev/null || true
        sleep 0.5
        # Force kill if still running
        kill -9 "$SERVER_PID" 2>/dev/null || true
    fi

    # Clean up any remaining processes on the port (belt and suspenders)
    local remaining_pids
    remaining_pids=$(lsof -ti :${PORT} 2>/dev/null)
    if [ -n "$remaining_pids" ]; then
        echo "$remaining_pids" | xargs -r kill -9 2>/dev/null || true
    fi

    echo "Server stopped."
    exit 0
}

# Set up signal handlers for graceful cleanup
trap cleanup EXIT INT TERM

# Main execution
main() {
    # First cleanup any existing servers
    cleanup_existing

    # Check if the server directory exists
    if [ ! -d "$SERVER_DIR" ]; then
        echo "Error: Server directory '$SERVER_DIR' does not exist"
        echo "Please run 'just build-emscripten' first"
        exit 1
    fi

    # Change to server directory
    cd "$SERVER_DIR" || exit 1

    # Start the server
    echo "Starting web server for Emscripten build..."
    echo "Open ${URL} in your browser"
    echo "Press Ctrl+C to stop the server"
    echo ""

    # Start Python HTTP server in background and capture its PID
    python3 -m http.server ${PORT} &
    SERVER_PID=$!

    # Wait for the server process to exit or for a signal
    wait $SERVER_PID
}

# Run the main function
main "$@"