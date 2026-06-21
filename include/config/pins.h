#pragma once

// =============================================================================
// MyAmpel Pin Configuration
// GPIO assignments for ESP32-C3-DevKitC-02
// =============================================================================

// Main Signal LEDs
#define PIN_LED_RED       2
#define PIN_LED_GREEN     3

// Pre-Signal LEDs (single-color for now, abstracted for future RGB)
#define PIN_PRE_LED_1     4   // Yellow or Green
#define PIN_PRE_LED_2     5   // Yellow or Green

// Buttons (active low, internal pull-up)
#define PIN_BTN_GREEN     6
#define PIN_BTN_RED       7

// IR Sensor (active low — goes LOW when beam broken)
#define PIN_IR_SENSOR     8
