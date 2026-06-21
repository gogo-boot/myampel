#pragma once
#include <cstdint>

// Main signal state
enum class MainSignal : uint8_t {
    RED = 0,
    GREEN = 1
};

// Pre-signal state (mirrors next signal's main)
enum class PreSignal : uint8_t {
    YELLOW = 0,  // Next signal is RED
    GREEN = 1    // Next signal is GREEN
};

// Lifecycle stages
enum class Stage : uint8_t {
    BOOT,
    CONFIG,
    IDENTIFY,
    CONNECT,
    RUNNING,
    SLEEP
};

// ESP-NOW message types
enum class MsgType : uint8_t {
    STATE = 0,
    PAIR_OFFER = 1,
    PAIR_ACCEPT = 2,
    STATE_REQUEST = 3
};

// ESP-NOW message payload (4 bytes)
struct SignalMessage {
    uint8_t signal_id;
    uint8_t main_state;   // MainSignal cast
    uint8_t msg_type;     // MsgType cast
    uint8_t sequence;
};

// Button events
enum class ButtonEvent : uint8_t {
    NONE,
    SHORT_PRESS,
    LONG_PRESS
};

// Timing constants (ms)
constexpr uint32_t HEARTBEAT_INTERVAL_MS = 2000;
constexpr uint32_t AUTO_GREEN_TIMER_MS = 30000;
constexpr uint32_t SLEEP_TIMEOUT_MS = 300000;  // 5 minutes
constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
constexpr uint32_t BUTTON_LONG_PRESS_MS = 3000;
constexpr uint32_t IR_DEBOUNCE_MS = 100;
constexpr uint32_t PAIRING_TIMEOUT_MS = 30000;
constexpr uint32_t CONNECT_TIMEOUT_MS = 2000;
constexpr uint32_t IDENTIFY_BLINK_MS = 500;
constexpr uint32_t SELFTEST_DURATION_MS = 500;
constexpr uint8_t  ESP_NOW_MAX_RETRIES = 3;
constexpr uint32_t ESP_NOW_RETRY_DELAY_MS = 50;
