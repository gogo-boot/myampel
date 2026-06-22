#pragma once

// =============================================================================
// MyAmpel Pin Configuration
// GPIO assignments for ESP32-C3-MINI-1
// =============================================================================

// Main Signal LEDs
#define PIN_LED_RED       0
#define PIN_LED_GREEN     1

// Pre-Signal LEDs
#define PIN_PRE_YELLOW_1  3    // Yellow LED 1
#define PIN_PRE_YELLOW_2  10   // Yellow LED 2
#define PIN_PRE_GREEN_1   4    // Green LED 1
#define PIN_PRE_GREEN_2   5    // Green LED 2

// Buttons (active low, internal pull-up)
#define PIN_BTN_GREEN     6
#define PIN_BTN_RED       7

// IR Sensor (active low — goes LOW when beam broken)
#define PIN_IR_SENSOR     8
