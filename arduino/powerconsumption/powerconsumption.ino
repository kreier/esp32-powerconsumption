/*
 * ESP32 Power Consumption Benchmark
 *
 * A multi-board benchmark suite to measure and compare the power consumption
 * of ESP32 family development boards across various operational stages:
 *   - Idle (CPU on, Wi-Fi off)
 *   - CPU active (continuous arithmetic workload)
 *   - Wi-Fi enabled (STA mode, disconnected, modem-sleep off)
 *   - Wi-Fi scan (active 2.4 GHz channel probing)
 *   - LED / RGB LED active test
 *   - Light sleep (timer wake-up)
 *   - Deep sleep (timer wake-up with RTC memory retention)
 *
 * HOW TO SWITCH BOARDS:
 *   Simply change the single define 'SELECTED_BOARD' below.
 *   All pin assignments, RGB NeoPixel vs standard LED settings,
 *   expected flash/PSRAM sizes, and GPIO surveys are updated automatically.
 *
 * Required library:
 *   Adafruit NeoPixel (by Adafruit) - install via Arduino Library Manager
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_sleep.h>
#include <esp_system.h>

// =============================================================================
// 1. BOARD SELECTION
// =============================================================================

#define BOARD_AI_S3_YD_ESP32_S3       1  // AI-S3 / YD-ESP32-S3 (Dual USB-C, CH343P, RGB LED on GPIO 48)
#define BOARD_ESP32_C3_DUAL_USB       2  // Nameless ESP32-C3 (Dual USB-C, 4MB Flash, RGB LED on GPIO 8)
#define BOARD_ESP32_C3_SUPER_MINI     3  // ESP32-C3 Super Mini (16 pins, ceramic ant., Blue LED on GPIO 8, active low)
#define BOARD_WEMOS_S2_MINI           4  // WEMOS LOLIN S2 Mini v1.0.0 (ESP32-S2, Blue LED on GPIO 15, active high)
#define BOARD_WEMOS_LOLIN_S3          5  // WEMOS LOLIN S3 (ESP32-S3, RGB LED on GPIO 38)
#define BOARD_GENERIC_ESP32           6  // Generic ESP32 DevKit (ESP-WROOM-32, Blue LED on GPIO 2, active high)

// >>> CHANGE JUST THIS SINGLE LINE TO SELECT YOUR BOARD <<<
#ifndef SELECTED_BOARD
#define SELECTED_BOARD BOARD_AI_S3_YD_ESP32_S3
#endif

// =============================================================================
// 2. BOARD PROFILES & CONFIGURATIONS
// =============================================================================

#if (SELECTED_BOARD == BOARD_AI_S3_YD_ESP32_S3)

  #define BOARD_NAME              "AI-S3 (YD-ESP32-S3)"
  #define BOARD_MANUFACTURER      "VCC-GND Studio / YD family"
  #define BOARD_MCU_MODULE        "ESP32-S3 (Dual-core Xtensa LX7, up to 240 MHz)"
  #define EXPECTED_FLASH          (16UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          (8UL * 1024UL * 1024UL)
  #define BOARD_HAS_PSRAM_SUPPORT 1
  #define USB_PORTS_DESC          "2 x USB-C: COM (CH343P USB-to-UART) & USB (Native S3 USB OTG)"

  // RGB LED settings (WS2812B NeoPixel on GPIO 48)
  #define HAS_RGB_LED             1
  #define RGB_LED_PIN             48
  #define RGB_LED_COUNT           1
  #define RGB_LED_BRIGHTNESS      32

  // Standard LED settings (none)
  #define HAS_STANDARD_LED        0
  #define LED_PIN                 -1
  #define LED_ACTIVE_LEVEL        HIGH

  // Measured reference benchmarks (mA)
  #define HAS_RECORDED_BENCHMARK  1
  #define REF_BENCH_IDLE          "41 mA"
  #define REF_BENCH_CPU           "40 mA"
  #define REF_BENCH_WIFI_ACTIVE   "40 mA"
  #define REF_BENCH_WIFI_SCAN     "112 mA"
  #define REF_BENCH_LIGHT_SLEEP   "5 mA"
  #define REF_BENCH_DEEP_SLEEP    "4 mA"

#elif (SELECTED_BOARD == BOARD_ESP32_C3_DUAL_USB)

  #define BOARD_NAME              "Nameless ESP32-C3 Dual USB-C"
  #define BOARD_MANUFACTURER      "Generic / Unbranded (30 pins, 15 per side)"
  #define BOARD_MCU_MODULE        "ESP32-C3 (Single-core RISC-V, up to 160 MHz)"
  #define EXPECTED_FLASH          (4UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          0UL
  #define BOARD_HAS_PSRAM_SUPPORT 0
  #define USB_PORTS_DESC          "2 x USB-C: COM (USB-to-UART bridge) & USB (Native C3 USB CDC/JTAG)"

  // RGB LED settings (WS2812 NeoPixel on GPIO 8)
  #define HAS_RGB_LED             1
  #define RGB_LED_PIN             8
  #define RGB_LED_COUNT           1
  #define RGB_LED_BRIGHTNESS      32

  // Standard LED settings (none)
  #define HAS_STANDARD_LED        0
  #define LED_PIN                 -1
  #define LED_ACTIVE_LEVEL        HIGH

  // Measured reference benchmarks (mA)
  #define HAS_RECORDED_BENCHMARK  1
  #define REF_BENCH_IDLE          "26 mA"
  #define REF_BENCH_CPU           "33 mA"
  #define REF_BENCH_WIFI_ACTIVE   "28 mA"
  #define REF_BENCH_WIFI_SCAN     "94 mA"
  #define REF_BENCH_LIGHT_SLEEP   "9 mA"
  #define REF_BENCH_DEEP_SLEEP    "9 mA"

#elif (SELECTED_BOARD == BOARD_ESP32_C3_SUPER_MINI)

  #define BOARD_NAME              "ESP32-C3 Super Mini"
  #define BOARD_MANUFACTURER      "Maker / Generic (16 pins, ceramic antenna)"
  #define BOARD_MCU_MODULE        "ESP32-C3 (Single-core RISC-V, up to 160 MHz)"
  #define EXPECTED_FLASH          (4UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          0UL
  #define BOARD_HAS_PSRAM_SUPPORT 0
  #define USB_PORTS_DESC          "1 x USB-C: Native C3 USB CDC / JTAG"

  // RGB LED settings (none)
  #define HAS_RGB_LED             0
  #define RGB_LED_PIN             -1
  #define RGB_LED_COUNT           0
  #define RGB_LED_BRIGHTNESS      0

  // Standard onboard Blue LED on GPIO 8 (Active LOW)
  #define HAS_STANDARD_LED        1
  #define LED_PIN                 8
  #define LED_ACTIVE_LEVEL        LOW

  // Measured reference benchmarks (mA)
  #define HAS_RECORDED_BENCHMARK  0
  #define REF_BENCH_IDLE          "TBD"
  #define REF_BENCH_CPU           "TBD"
  #define REF_BENCH_WIFI_ACTIVE   "TBD"
  #define REF_BENCH_WIFI_SCAN     "TBD"
  #define REF_BENCH_LIGHT_SLEEP   "TBD"
  #define REF_BENCH_DEEP_SLEEP    "TBD"

#elif (SELECTED_BOARD == BOARD_WEMOS_S2_MINI)

  #define BOARD_NAME              "WEMOS LOLIN S2 Mini v1.0.0"
  #define BOARD_MANUFACTURER      "WEMOS.CC"
  #define BOARD_MCU_MODULE        "ESP32-S2 (Single-core Xtensa LX7, up to 240 MHz)"
  #define EXPECTED_FLASH          (4UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          (2UL * 1024UL * 1024UL)
  #define BOARD_HAS_PSRAM_SUPPORT 1
  #define USB_PORTS_DESC          "1 x USB-C: Native S2 USB CDC / OTG"

  // RGB LED settings (none)
  #define HAS_RGB_LED             0
  #define RGB_LED_PIN             -1
  #define RGB_LED_COUNT           0
  #define RGB_LED_BRIGHTNESS      0

  // Standard onboard Blue LED on GPIO 15 (Active HIGH)
  #define HAS_STANDARD_LED        1
  #define LED_PIN                 15
  #define LED_ACTIVE_LEVEL        HIGH

  // Measured reference benchmarks (mA)
  #define HAS_RECORDED_BENCHMARK  0
  #define REF_BENCH_IDLE          "TBD"
  #define REF_BENCH_CPU           "TBD"
  #define REF_BENCH_WIFI_ACTIVE   "TBD"
  #define REF_BENCH_WIFI_SCAN     "TBD"
  #define REF_BENCH_LIGHT_SLEEP   "TBD"
  #define REF_BENCH_DEEP_SLEEP    "TBD"

#elif (SELECTED_BOARD == BOARD_WEMOS_LOLIN_S3)

  #define BOARD_NAME              "WEMOS LOLIN S3"
  #define BOARD_MANUFACTURER      "WEMOS.CC"
  #define BOARD_MCU_MODULE        "ESP32-S3 (Dual-core Xtensa LX7, up to 240 MHz)"
  #define EXPECTED_FLASH          (16UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          (8UL * 1024UL * 1024UL)
  #define BOARD_HAS_PSRAM_SUPPORT 1
  #define USB_PORTS_DESC          "1 x USB-C: Native S3 USB OTG (or CH340 on pro version)"

  // RGB LED settings (WS2812B NeoPixel on GPIO 38)
  #define HAS_RGB_LED             1
  #define RGB_LED_PIN             38
  #define RGB_LED_COUNT           1
  #define RGB_LED_BRIGHTNESS      32

  // Standard LED settings (none)
  #define HAS_STANDARD_LED        0
  #define LED_PIN                 -1
  #define LED_ACTIVE_LEVEL        HIGH

  #define HAS_RECORDED_BENCHMARK  0
  #define REF_BENCH_IDLE          "TBD"
  #define REF_BENCH_CPU           "TBD"
  #define REF_BENCH_WIFI_ACTIVE   "TBD"
  #define REF_BENCH_WIFI_SCAN     "TBD"
  #define REF_BENCH_LIGHT_SLEEP   "TBD"
  #define REF_BENCH_DEEP_SLEEP    "TBD"

#elif (SELECTED_BOARD == BOARD_GENERIC_ESP32)

  #define BOARD_NAME              "Generic ESP32 DevKit"
  #define BOARD_MANUFACTURER      "Espressif / Generic (ESP-WROOM-32)"
  #define BOARD_MCU_MODULE        "ESP32-D0WDQ6 (Dual-core Xtensa LX6, up to 240 MHz)"
  #define EXPECTED_FLASH          (4UL * 1024UL * 1024UL)
  #define EXPECTED_PSRAM          0UL
  #define BOARD_HAS_PSRAM_SUPPORT 0
  #define USB_PORTS_DESC          "1 x Micro-USB / USB-C: CP2102 or CH340 USB-to-UART"

  // RGB LED settings (none)
  #define HAS_RGB_LED             0
  #define RGB_LED_PIN             -1
  #define RGB_LED_COUNT           0
  #define RGB_LED_BRIGHTNESS      0

  // Standard onboard Blue LED on GPIO 2 (Active HIGH)
  #define HAS_STANDARD_LED        1
  #define LED_PIN                 2
  #define LED_ACTIVE_LEVEL        HIGH

  #define HAS_RECORDED_BENCHMARK  0
  #define REF_BENCH_IDLE          "TBD"
  #define REF_BENCH_CPU           "TBD"
  #define REF_BENCH_WIFI_ACTIVE   "TBD"
  #define REF_BENCH_WIFI_SCAN     "TBD"
  #define REF_BENCH_LIGHT_SLEEP   "TBD"
  #define REF_BENCH_DEEP_SLEEP    "TBD"

#else
  #error "SELECTED_BOARD is not defined or invalid. Please select a valid board ID."
#endif

// =============================================================================
// 3. BENCHMARK DURATIONS & SETTINGS
// =============================================================================

#define SERIAL_BAUD          115200
#define IDLE_SECONDS         30
#define CPU_SECONDS          30
#define WIFI_SECONDS         30
#define LIGHT_SLEEP_SECONDS  30
#define DEEP_SLEEP_SECONDS   30
#define WIFI_SCAN_MS        120

// Conditional Adafruit NeoPixel initialization
#if HAS_RGB_LED
  #include <Adafruit_NeoPixel.h>
  Adafruit_NeoPixel pixels(RGB_LED_COUNT, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

// Retained in RTC memory across deep-sleep cycles
RTC_DATA_ATTR uint32_t deepSleepCount = 0;

// =============================================================================
// 4. LED MANAGEMENT
// =============================================================================

void ledInit() {
#if HAS_RGB_LED
    pixels.begin();
    pixels.setBrightness(RGB_LED_BRIGHTNESS);
    pixels.clear();
    pixels.show();
#endif

#if HAS_STANDARD_LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, !LED_ACTIVE_LEVEL);  // Turn off
#endif
}

void ledOff() {
#if HAS_RGB_LED
    pixels.clear();
    pixels.show();
#endif

#if HAS_STANDARD_LED
    digitalWrite(LED_PIN, !LED_ACTIVE_LEVEL);  // Turn off
#endif
}

void testLED() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("              LED BENCHMARK");
    Serial.println("==========================================");

#if HAS_RGB_LED
    Serial.printf("Testing WS2812 RGB LED on GPIO %d...\n", RGB_LED_PIN);
    pixels.setBrightness(RGB_LED_BRIGHTNESS);

    const uint32_t colors[] = {
        pixels.Color(255, 0, 0),
        pixels.Color(0, 255, 0),
        pixels.Color(0, 0, 255),
        pixels.Color(255, 255, 255)
    };
    const char *names[] = { "Red", "Green", "Blue", "White" };

    for (int i = 0; i < 4; i++) {
        Serial.printf("  RGB Color: %-6s (3 seconds)\n", names[i]);
        pixels.setPixelColor(0, colors[i]);
        pixels.show();
        delay(3000);
    }
    ledOff();
    Serial.println("  RGB LED turned off.");
#endif

#if HAS_STANDARD_LED
    Serial.printf("Testing onboard LED on GPIO %d (Active %s)...\n",
                  LED_PIN, (LED_ACTIVE_LEVEL == HIGH) ? "HIGH" : "LOW");
    for (int cycle = 1; cycle <= 3; cycle++) {
        Serial.printf("  Standard LED: ON  (cycle %d/3, 1 second)\n", cycle);
        digitalWrite(LED_PIN, LED_ACTIVE_LEVEL);
        delay(1000);

        Serial.printf("  Standard LED: OFF (cycle %d/3, 1 second)\n", cycle);
        digitalWrite(LED_PIN, !LED_ACTIVE_LEVEL);
        delay(1000);
    }
    ledOff();
    Serial.println("  Standard LED turned off.");
#endif

#if (!HAS_RGB_LED && !HAS_STANDARD_LED)
    Serial.println("No LED configured for this board.");
#endif

    Serial.println("==========================================");
    Serial.println("          LED BENCHMARK COMPLETE");
    Serial.println("==========================================");
}

// =============================================================================
// 5. HELPER FUNCTIONS
// =============================================================================

void printBytes(const char *label, uint32_t bytes) {
    Serial.printf("%-26s %lu bytes (%.2f MB)\n",
                  label, (unsigned long)bytes, bytes / 1048576.0);
}

void printResetReason() {
    Serial.print("Reset reason:              ");
    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:   Serial.println("Power-on"); break;
        case ESP_RST_EXT:       Serial.println("External pin reset"); break;
        case ESP_RST_SW:        Serial.println("Software reset"); break;
        case ESP_RST_PANIC:     Serial.println("Exception / panic reset"); break;
        case ESP_RST_INT_WDT:   Serial.println("Interrupt watchdog reset"); break;
        case ESP_RST_TASK_WDT:  Serial.println("Task watchdog reset"); break;
        case ESP_RST_WDT:       Serial.println("Other watchdog reset"); break;
        case ESP_RST_DEEPSLEEP: Serial.println("Deep-sleep wake-up"); break;
        case ESP_RST_BROWNOUT:  Serial.println("Brownout reset"); break;
        case ESP_RST_SDIO:      Serial.println("SDIO reset"); break;
        default:                Serial.println("Other / unknown"); break;
    }
}

// =============================================================================
// 6. BOARD SURVEY
// =============================================================================

void surveyBoard() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("           ESP32 BOARD SURVEY");
    Serial.println("==========================================");

    Serial.println("[1] BOARD PROFILE");
    Serial.printf("Board Name:                %s\n", BOARD_NAME);
    Serial.printf("Manufacturer / Style:      %s\n", BOARD_MANUFACTURER);
    Serial.printf("MCU Description:           %s\n", BOARD_MCU_MODULE);
    Serial.printf("USB Ports:                 %s\n", USB_PORTS_DESC);

#if HAS_RGB_LED
    Serial.printf("RGB LED:                   WS2812B NeoPixel on GPIO %d\n", RGB_LED_PIN);
#elif HAS_STANDARD_LED
    Serial.printf("Standard Status LED:       GPIO %d (Active %s)\n",
                  LED_PIN, (LED_ACTIVE_LEVEL == HIGH) ? "HIGH" : "LOW");
#else
    Serial.println("LED:                       None");
#endif

#if HAS_RECORDED_BENCHMARK
    Serial.println();
    Serial.println("[2] RECORDED REFERENCE BENCHMARKS");
    Serial.printf("Idle (CPU on, Wi-Fi off):  %s\n", REF_BENCH_IDLE);
    Serial.printf("CPU Active:                %s\n", REF_BENCH_CPU);
    Serial.printf("Wi-Fi Active (STA idle):   %s\n", REF_BENCH_WIFI_ACTIVE);
    Serial.printf("Wi-Fi Scan:                %s\n", REF_BENCH_WIFI_SCAN);
    Serial.printf("Light Sleep:               %s\n", REF_BENCH_LIGHT_SLEEP);
    Serial.printf("Deep Sleep:                %s\n", REF_BENCH_DEEP_SLEEP);
#endif

    Serial.println();
    Serial.println("[3] CHIP HARDWARE INFORMATION");
    Serial.printf("Chip Model:                %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision:             %u\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency:             %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("CPU Cores:                 %u\n", ESP.getChipCores());
    Serial.printf("SDK Version:               %s\n", ESP.getSdkVersion());
    printResetReason();

    Serial.println();
    Serial.println("[4] FLASH MEMORY");
    uint32_t flashSize = ESP.getFlashChipSize();
    printBytes("Detected Flash:", flashSize);
    printBytes("Expected Flash:", EXPECTED_FLASH);
    Serial.printf("Flash Frequency:           %u Hz\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash Mode:                %d\n", (int)ESP.getFlashChipMode());
    Serial.printf("Flash Size Matches:        %s\n", (flashSize == EXPECTED_FLASH) ? "YES" : "NO");

    Serial.println();
    Serial.println("[5] PSRAM (EXTERNAL RAM)");
#if BOARD_HAS_PSRAM_SUPPORT
    bool hasPsram = psramFound();
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t psramFree = ESP.getFreePsram();
    Serial.printf("PSRAM Detected:            %s\n", hasPsram ? "YES" : "NO");
    printBytes("Detected PSRAM:", psramSize);
    printBytes("Expected PSRAM:", EXPECTED_PSRAM);
    printBytes("Free PSRAM:", psramFree);
    Serial.printf("PSRAM Size Matches:        %s\n", (psramSize == EXPECTED_PSRAM) ? "YES" : "NO");
#else
    Serial.println("PSRAM:                     Not supported / Not fitted on this SoC");
#endif

    Serial.println();
    Serial.println("[6] INTERNAL HEAP & RAM");
    printBytes("Total Heap:", ESP.getHeapSize());
    printBytes("Free Heap:", ESP.getFreeHeap());
    printBytes("Minimum Free Heap:", ESP.getMinFreeHeap());

    Serial.println();
    Serial.println("[7] SKETCH FLASH USAGE");
    printBytes("Sketch Size:", ESP.getSketchSize());
    printBytes("Free Sketch Space:", ESP.getFreeSketchSpace());

    Serial.println();
    Serial.println("[8] BOARD PINOUT & STRAPPING REFERENCE");
#if (SELECTED_BOARD == BOARD_AI_S3_YD_ESP32_S3)
    Serial.println("Header GPIOs: 0-18, 21, 38-48");
    Serial.println("  GPIO 48:      Onboard WS2812B RGB LED");
    Serial.println("  GPIO 43/44:   UART0 TX/RX (CH343P COM port)");
    Serial.println("  GPIO 19/20:   Native USB OTG D- / D+");
    Serial.println("  GPIO 0:       BOOT button / Strapping");
    Serial.println("  Strapping:    GPIO0, GPIO3, GPIO45, GPIO46");
#elif (SELECTED_BOARD == BOARD_ESP32_C3_DUAL_USB)
    Serial.println("30-pin Dual USB-C C3 Board:");
    Serial.println("  GPIO 8:       Onboard WS2812 RGB LED (Note: strapping pin)");
    Serial.println("  GPIO 20/21:   UART0 RX/TX (CH340/CH343 COM port)");
    Serial.println("  GPIO 18/19:   Native USB D- / D+");
    Serial.println("  GPIO 9:       BOOT button / Strapping");
    Serial.println("  Strapping:    GPIO2, GPIO8, GPIO9");
#elif (SELECTED_BOARD == BOARD_ESP32_C3_SUPER_MINI)
    Serial.println("16-pin Super Mini Board:");
    Serial.println("  Left row:     GPIO5, GPIO6, GPIO7, GPIO8, GPIO9, GPIO10, GPIO20, GPIO21");
    Serial.println("  Right row:    5V, GND, 3V3, GPIO4, GPIO3, GPIO2, GPIO1, GPIO0");
    Serial.println("  GPIO 8:       Onboard Blue Status LED (Active LOW)");
    Serial.println("  GPIO 9:       BOOT button / Strapping");
    Serial.println("  GPIO 18/19:   Internal USB D- / D+");
    Serial.println("  Strapping:    GPIO2, GPIO8, GPIO9");
#elif (SELECTED_BOARD == BOARD_WEMOS_S2_MINI)
    Serial.println("WEMOS LOLIN S2 Mini v1.0.0:");
    Serial.println("  GPIO 15:      Onboard Blue Status LED (Active HIGH)");
    Serial.println("  GPIO 19/20:   Native USB D- / D+");
    Serial.println("  GPIO 0:       BOOT button / Strapping");
    Serial.println("  Strapping:    GPIO0, GPIO45, GPIO46");
#elif (SELECTED_BOARD == BOARD_WEMOS_LOLIN_S3)
    Serial.println("WEMOS LOLIN S3:");
    Serial.println("  GPIO 38:      Onboard WS2812B RGB LED");
    Serial.println("  GPIO 41/42:   I2C SCL / SDA");
    Serial.println("  GPIO 19/20:   Native USB D- / D+");
    Serial.println("  GPIO 0:       BOOT button");
#elif (SELECTED_BOARD == BOARD_GENERIC_ESP32)
    Serial.println("Generic ESP32 DevKit:");
    Serial.println("  GPIO 2:       Onboard Blue LED (Active HIGH, strapping pin)");
    Serial.println("  GPIO 1/3:     UART0 TX/RX (USB-UART bridge)");
    Serial.println("  GPIO 0:       BOOT button / Strapping");
    Serial.println("  Strapping:    GPIO0, GPIO2, GPIO5, GPIO12, GPIO15");
#endif

    Serial.println();
    Serial.println("==========================================");
    Serial.println("             SURVEY COMPLETE");
    Serial.println("==========================================");
    Serial.println();
}

// =============================================================================
// 7. BENCHMARK TESTS
// =============================================================================

void testIdle() {
    Serial.printf("\n[BENCHMARK] Idle test: %d seconds\n", IDLE_SECONDS);
    Serial.println("Conditions: CPU running at default frequency, Wi-Fi off, LEDs off.");
    ledOff();
    WiFi.mode(WIFI_OFF);

    uint32_t start = millis();
    while (millis() - start < IDLE_SECONDS * 1000UL) {
        delay(100);
    }
    Serial.println("[BENCHMARK] Idle test complete.");
}

void testCPU() {
    Serial.printf("\n[BENCHMARK] CPU active test: %d seconds\n", CPU_SECONDS);
    Serial.println("Conditions: 100% CPU arithmetic workload, Wi-Fi off, LEDs off.");
    ledOff();
    WiFi.mode(WIFI_OFF);

    volatile uint32_t value = 1;
    uint32_t iterations = 0;
    uint32_t start = millis();

    while (millis() - start < CPU_SECONDS * 1000UL) {
        value = value * 1664525UL + 1013904223UL;
        iterations++;
    }

    Serial.printf("Iterations completed:      %lu\n", (unsigned long)iterations);
    Serial.printf("Checksum output:           %lu\n", (unsigned long)value);
    Serial.println("[BENCHMARK] CPU active test complete.");
}

void testWiFiIdle() {
    Serial.printf("\n[BENCHMARK] Wi-Fi enabled, disconnected: %d seconds\n", WIFI_SECONDS);
    Serial.println("Conditions: Wi-Fi STA active, no connection, modem-sleep disabled.");
    ledOff();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    WiFi.setSleep(false);  // Keep RF receiver continuously powered

    delay(1000);

    uint32_t start = millis();
    while (millis() - start < WIFI_SECONDS * 1000UL) {
        delay(100);
    }

    WiFi.mode(WIFI_OFF);
    Serial.println("[BENCHMARK] Wi-Fi enabled test complete.");
}

void testWiFiScan() {
    Serial.println("\n[BENCHMARK] Wi-Fi scan test");
    Serial.println("Conditions: Active 2.4 GHz channel scan with probe requests.");
    ledOff();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(500);

    uint32_t start = millis();
    int networks = WiFi.scanNetworks(
        false,        // Synchronous scan
        true,         // Include hidden networks
        false,        // Passive scan = false (active probing)
        WIFI_SCAN_MS  // Channel dwell time in ms
    );
    uint32_t duration = millis() - start;

    Serial.printf("Networks detected:         %d\n", networks);
    Serial.printf("Scan duration:             %lu ms\n", (unsigned long)duration);

    WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);
    Serial.println("[BENCHMARK] Wi-Fi scan test complete.");
}

void testLightSleep() {
    Serial.printf("\n[BENCHMARK] Light sleep test: %d seconds\n", LIGHT_SLEEP_SECONDS);
    Serial.println("Entering light sleep. Current should drop significantly.");
    ledOff();
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup((uint64_t)LIGHT_SLEEP_SECONDS * 1000000ULL);
    Serial.flush();

    esp_light_sleep_start();

    Serial.println("Returned from light sleep.");
    Serial.printf("Wake-up cause code:        %d\n", (int)esp_sleep_get_wakeup_cause());
    Serial.println("[BENCHMARK] Light sleep test complete.");
}

void testDeepSleep() {
    deepSleepCount++;
    Serial.println("\n[BENCHMARK] Deep sleep test");
    Serial.printf("Sleep cycle count:         #%lu\n", (unsigned long)deepSleepCount);
    Serial.printf("Sleeping for:              %d seconds...\n", DEEP_SLEEP_SECONDS);
    Serial.println("Board will restart upon waking.");

    ledOff();
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup((uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL);
    Serial.flush();

    esp_deep_sleep_start();
}

void printMenu() {
    Serial.println();
    Serial.println("========== POWER BENCHMARK MENU ==========");
    Serial.println("  b  Board and memory survey");
    Serial.println("  i  Idle benchmark (30s)");
    Serial.println("  c  CPU active benchmark (30s)");
    Serial.println("  w  Wi-Fi enabled, disconnected (30s)");
    Serial.println("  s  Wi-Fi scan benchmark");
    Serial.println("  r  LED / RGB LED benchmark");
    Serial.println("  l  Light sleep (30s)");
    Serial.println("  d  Deep sleep (30s)");
    Serial.println("  a  Run complete test suite (deep sleep last)");
    Serial.println("  ?  Show this menu");
    Serial.println("==========================================");
    Serial.println("Enter command: ");
}

void runAllTests() {
    Serial.println("\n>>> Starting Complete Benchmark Suite <<<");
    testLED();
    testIdle();
    testCPU();
    testWiFiIdle();
    testWiFiScan();
    testLightSleep();

    // Deep sleep restarts the MCU, so run it last
    testDeepSleep();
}

// =============================================================================
// 8. SETUP & LOOP
// =============================================================================

void setup() {
    Serial.begin(SERIAL_BAUD);

    // Wait briefly for native USB CDC enumeration (useful on C3 Super Mini & S2 Mini)
    uint32_t startWait = millis();
    while (!Serial && (millis() - startWait < 2000)) {
        delay(10);
    }
    delay(200);

    ledInit();
    ledOff();
    WiFi.mode(WIFI_OFF);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("    ESP32 POWER CONSUMPTION BENCHMARK");
    Serial.println("==========================================");
    Serial.printf("Active Board Profile: %s\n", BOARD_NAME);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Wake-up reason: Timer after deep sleep.");
        Serial.printf("Completed deep-sleep cycles: %lu\n", (unsigned long)deepSleepCount);
    }

    surveyBoard();
    printMenu();
}

void loop() {
    if (!Serial.available()) {
        delay(10);
        return;
    }

    char command = Serial.read();

    switch (command) {
        case 'b': surveyBoard(); break;
        case 'i': testIdle(); break;
        case 'c': testCPU(); break;
        case 'w': testWiFiIdle(); break;
        case 's': testWiFiScan(); break;
        case 'r': testLED(); break;
        case 'l': testLightSleep(); break;
        case 'd': testDeepSleep(); break;
        case 'a': runAllTests(); break;
        case '?': printMenu(); break;
        case '\r':
        case '\n':
            break;
        default:
            Serial.printf("Unknown command '%c'. Type '?' for menu.\n", command);
            break;
    }
}
