
/*
 * ESP32 Power Consumption Benchmark
 * Board profile: AI-S3 (YD-ESP32-S3)
 *
 * Hardware:
 *   MCU:       ESP32-S3 (Dual-core Xtensa LX7, up to 240 MHz)
 *   Flash:     16 MB Quad / Octal SPI
 *   PSRAM:      8 MB Octal SPI
 *   RGB LED:   WS2812B, GPIO48
 *   USB:       Native USB OTG + CH343P USB-to-UART (COM)
 *
 * Measured Power Consumption:
 *   Idle:              41 mA
 *   CPU active:        40 mA
 *   Wireless active:   40 mA
 *   Wi-Fi scan:       112 mA
 *   Light sleep:        5 mA
 *   Deep sleep:         4 mA
 *
 * Required library:
 *   Adafruit NeoPixel
 *
 * Serial commands:
 *   ?  Show menu
 *   b  Board and memory survey
 *   i  Idle benchmark
 *   c  CPU active benchmark
 *   w  Wi-Fi enabled, disconnected
 *   s  Wi-Fi scan
 *   r  RGB LED benchmark
 *   l  Light sleep
 *   d  Deep sleep
 *   a  Run all tests (deep sleep last)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <esp_sleep.h>
#include <esp_system.h>

// --------------------------------------------------
// AI-S3 / YD-ESP32-S3 board profile
// --------------------------------------------------

#define BOARD_NAME       "AI-S3 (YD-ESP32-S3)"
#define EXPECTED_FLASH   (16UL * 1024UL * 1024UL)
#define EXPECTED_PSRAM   (8UL * 1024UL * 1024UL)

#define LED_PIN          48
#define LED_COUNT        1
#define LED_ENABLED      1

#define SERIAL_BAUD      115200

// Benchmark durations
#define IDLE_SECONDS       30
#define CPU_SECONDS        30
#define WIFI_SECONDS       30
#define LIGHT_SLEEP_SECONDS 30
#define DEEP_SLEEP_SECONDS 30

#define WIFI_SCAN_MS       120

Adafruit_NeoPixel pixels(
    LED_COUNT,
    LED_PIN,
    NEO_GRB + NEO_KHZ800
);

// Survives deep sleep because it is stored in RTC memory.
RTC_DATA_ATTR uint32_t deepSleepCount = 0;

// --------------------------------------------------
// Helpers
// --------------------------------------------------

void printBytes(const char *label, uint32_t bytes) {
    Serial.printf(
        "%-24s %lu bytes (%.2f MB)\n",
        label,
        (unsigned long)bytes,
        bytes / 1048576.0
    );
}

void printResetReason() {
    Serial.print("Reset reason:           ");

    switch (esp_reset_reason()) {
        case ESP_RST_POWERON:   Serial.println("Power-on"); break;
        case ESP_RST_EXT:       Serial.println("External reset"); break;
        case ESP_RST_SW:        Serial.println("Software reset"); break;
        case ESP_RST_PANIC:     Serial.println("Exception / panic"); break;
        case ESP_RST_INT_WDT:   Serial.println("Interrupt watchdog"); break;
        case ESP_RST_TASK_WDT:  Serial.println("Task watchdog"); break;
        case ESP_RST_WDT:       Serial.println("Other watchdog"); break;
        case ESP_RST_DEEPSLEEP: Serial.println("Deep sleep wake-up"); break;
        case ESP_RST_BROWNOUT:  Serial.println("Brownout"); break;
        case ESP_RST_SDIO:      Serial.println("SDIO reset"); break;
        default:                Serial.println("Other / unknown"); break;
    }
}

// --------------------------------------------------
// Board survey
// --------------------------------------------------

void surveyBoard() {
    Serial.println();
    Serial.println("==========================================");
    Serial.println("           ESP32 BOARD SURVEY");
    Serial.println("==========================================");

    Serial.println("[1] BOARD PROFILE");

    Serial.printf("Board:                   %s\n", BOARD_NAME);
    Serial.println("Manufacturer:            AI-S3 / YD-ESP32-S3 family");
    Serial.println("MCU module:               ESP32-S3-WROOM-1");
    Serial.println("USB connectors:           2 x USB-C");
    Serial.println("USB-to-UART bridge:       CH343P");
    Serial.println("Native USB:               ESP32-S3 USB OTG");
    Serial.println("Wireless:                 2.4 GHz Wi-Fi + Bluetooth LE");

    Serial.println();
    Serial.println("[2] CHIP INFORMATION");

    Serial.printf("Chip model:               %s\n", ESP.getChipModel());
    Serial.printf("Chip revision:            %u\n", ESP.getChipRevision());
    Serial.printf("CPU frequency:            %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("CPU cores:                %u\n", ESP.getChipCores());
    Serial.printf("SDK version:              %s\n", ESP.getSdkVersion());

    printResetReason();

    Serial.println();
    Serial.println("[3] FLASH MEMORY");

    uint32_t flashSize = ESP.getFlashChipSize();

    printBytes("Detected flash:", flashSize);
    printBytes("Expected flash:", EXPECTED_FLASH);

    Serial.printf(
        "Flash frequency:          %u Hz\n",
        ESP.getFlashChipSpeed()
    );

    Serial.printf(
        "Flash mode:               %d\n",
        (int)ESP.getFlashChipMode()
    );

    Serial.printf(
        "Flash size matches:       %s\n",
        flashSize == EXPECTED_FLASH ? "YES" : "NO"
    );

    Serial.println();
    Serial.println("[4] PSRAM");

    bool hasPsram = psramFound();
    uint32_t psramSize = ESP.getPsramSize();
    uint32_t psramFree = ESP.getFreePsram();

    Serial.printf("PSRAM detected:           %s\n", hasPsram ? "YES" : "NO");
    printBytes("Detected PSRAM:", psramSize);
    printBytes("Expected PSRAM:", EXPECTED_PSRAM);
    printBytes("Free PSRAM:", psramFree);

    Serial.printf(
        "PSRAM size matches:       %s\n",
        psramSize == EXPECTED_PSRAM ? "YES" : "NO"
    );

    Serial.println();
    Serial.println("[5] INTERNAL RAM");

    printBytes("Total heap:", ESP.getHeapSize());
    printBytes("Free heap:", ESP.getFreeHeap());
    printBytes("Minimum free heap:", ESP.getMinFreeHeap());

    Serial.println();
    Serial.println("[6] SKETCH");

    printBytes("Sketch size:", ESP.getSketchSize());
    printBytes("Free sketch space:", ESP.getFreeSketchSpace());

    Serial.println();
    Serial.println("[7] ONBOARD PERIPHERALS");

    Serial.println("RGB LED:                  WS2812B addressable");
    Serial.println("RGB LED data GPIO:        GPIO48");
    Serial.println("USB UART TX:              GPIO43");
    Serial.println("USB UART RX:              GPIO44");
    Serial.println("Native USB D-:            GPIO19");
    Serial.println("Native USB D+:            GPIO20");
    Serial.println("BOOT button:              GPIO0");
    Serial.println("RESET button:             CHIP_PU / EN");

    Serial.println();
    Serial.println("[8] BOARD GPIO REFERENCE");

    // This is a reference list, not an electrical pin test.
    const int gpioPins[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        10, 11, 12, 13, 14, 15, 16, 17, 18,
        21, 38, 39, 40, 41, 42, 43, 44, 45,
        46, 47, 48
    };

    Serial.println("GPIOs exposed on AI-S3 (YD-ESP32-S3) headers:");

    for (size_t i = 0; i < sizeof(gpioPins) / sizeof(gpioPins[0]); i++) {
        Serial.printf("%d ", gpioPins[i]);
    }

    Serial.println();
    Serial.println();
    Serial.println("Important GPIO notes:");
    Serial.println("  GPIO0:  BOOT / strapping pin");
    Serial.println("  GPIO3:  strapping pin");
    Serial.println("  GPIO45: strapping pin");
    Serial.println("  GPIO46: strapping pin");
    Serial.println("  GPIO48: onboard WS2812B RGB LED");
    Serial.println("  GPIO43/44: UART0 TX/RX (CH343P COM)");
    Serial.println("  GPIO19/20: native USB OTG");

    Serial.println();
    Serial.println("[9] CURRENT CONFIGURATION");

    Serial.printf("Wi-Fi status:             %s\n",
        WiFi.status() == WL_CONNECTED ? "Connected" : "Not connected");

    Serial.printf("LED benchmark:            %s\n",
        LED_ENABLED ? "Enabled" : "Disabled");

    Serial.printf("LED brightness:           %d\n", 32);

    Serial.println();
    Serial.println("==========================================");
    Serial.println("             SURVEY COMPLETE");
    Serial.println("==========================================");
    Serial.println();
}

// --------------------------------------------------
// RGB LED
// --------------------------------------------------

void ledOff() {
#if LED_ENABLED
    pixels.clear();
    pixels.show();
#endif
}

void testRGB() {
#if LED_ENABLED
    Serial.printf("\nRGB LED test: GPIO%d\n", LED_PIN);

    pixels.setBrightness(32);

    const uint32_t colors[] = {
        pixels.Color(255, 0, 0),
        pixels.Color(0, 255, 0),
        pixels.Color(0, 0, 255),
        pixels.Color(255, 255, 255)
    };

    const char *names[] = {
        "Red", "Green", "Blue", "White"
    };

    for (int i = 0; i < 4; i++) {
        Serial.printf("LED: %s\n", names[i]);
        pixels.setPixelColor(0, colors[i]);
        pixels.show();
        delay(3000);
    }

    ledOff();
    Serial.println("LED off.");
#endif
}

// --------------------------------------------------
// Idle benchmark
// --------------------------------------------------

void testIdle() {
    Serial.printf(
        "\nIdle test: %d seconds\n",
        IDLE_SECONDS
    );

    ledOff();
    WiFi.mode(WIFI_OFF);

    uint32_t start = millis();

    while (millis() - start < IDLE_SECONDS * 1000UL) {
        delay(100);
    }

    Serial.println("Idle test complete.");
}

// --------------------------------------------------
// CPU benchmark
// --------------------------------------------------

void testCPU() {
    Serial.printf(
        "\nCPU active test: %d seconds\n",
        CPU_SECONDS
    );

    ledOff();
    WiFi.mode(WIFI_OFF);

    volatile uint32_t value = 1;

    uint32_t start = millis();
    uint32_t iterations = 0;

    while (millis() - start < CPU_SECONDS * 1000UL) {
        value = value * 1664525UL + 1013904223UL;
        iterations++;
    }

    Serial.printf("Iterations: %lu\n", (unsigned long)iterations);
    Serial.printf("Result: %lu\n", (unsigned long)value);
    Serial.println("CPU test complete.");
}

// --------------------------------------------------
// Wi-Fi idle benchmark
// --------------------------------------------------

void testWiFiIdle() {
    Serial.printf(
        "\nWi-Fi enabled, disconnected: %d seconds\n",
        WIFI_SECONDS
    );

    ledOff();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    WiFi.setSleep(false);

    delay(1000);

    uint32_t start = millis();

    while (millis() - start < WIFI_SECONDS * 1000UL) {
        delay(100);
    }

    WiFi.mode(WIFI_OFF);

    Serial.println("Wi-Fi idle test complete.");
}

// --------------------------------------------------
// Wi-Fi scan benchmark
// --------------------------------------------------

void testWiFiScan() {
    Serial.println("\nWi-Fi scan test");

    ledOff();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);

    delay(500);

    uint32_t start = millis();

    int networks = WiFi.scanNetworks(
        false,       // synchronous scan
        true,        // include hidden networks
        false,       // passive scan
        WIFI_SCAN_MS
    );

    uint32_t duration = millis() - start;

    Serial.printf("Networks found: %d\n", networks);
    Serial.printf("Scan duration:  %lu ms\n", (unsigned long)duration);

    WiFi.scanDelete();
    WiFi.mode(WIFI_OFF);

    Serial.println("Wi-Fi scan complete.");
}

// --------------------------------------------------
// Light sleep benchmark
// --------------------------------------------------

void testLightSleep() {
    Serial.printf(
        "\nLight sleep: %d seconds\n",
        LIGHT_SLEEP_SECONDS
    );

    ledOff();
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup(
        (uint64_t)LIGHT_SLEEP_SECONDS * 1000000ULL
    );

    Serial.println("Entering light sleep...");

    Serial.flush();

    esp_light_sleep_start();

    Serial.println("Returned from light sleep.");
    Serial.printf(
        "Wake-up cause: %d\n",
        (int)esp_sleep_get_wakeup_cause()
    );
}

// --------------------------------------------------
// Deep sleep benchmark
// --------------------------------------------------

void testDeepSleep() {
    deepSleepCount++;

    Serial.printf(
        "\nDeep sleep test #%lu\n",
        (unsigned long)deepSleepCount
    );

    Serial.printf(
        "Sleeping for %d seconds...\n",
        DEEP_SLEEP_SECONDS
    );

    ledOff();
    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup(
        (uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL
    );

    Serial.flush();

    esp_deep_sleep_start();
}

// --------------------------------------------------
// Menu and test runner
// --------------------------------------------------

void printMenu() {
    Serial.println();
    Serial.println("========== POWER BENCHMARK ==========");
    Serial.println("b  Board and memory survey");
    Serial.println("i  Idle");
    Serial.println("c  CPU active");
    Serial.println("w  Wi-Fi enabled, disconnected");
    Serial.println("s  Wi-Fi scan");
    Serial.println("r  RGB LED test");
    Serial.println("l  Light sleep");
    Serial.println("d  Deep sleep");
    Serial.println("a  Run all tests");
    Serial.println("?  Show menu");
    Serial.println("=====================================");
}

void runAllTests() {
    Serial.println("\nStarting complete benchmark.");

    testRGB();
    testIdle();
    testCPU();
    testWiFiIdle();
    testWiFiScan();
    testLightSleep();

    // Deep sleep is last because it restarts the board.
    testDeepSleep();
}

// --------------------------------------------------
// Setup and loop
// --------------------------------------------------

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1500);

    pixels.begin();
    pixels.setBrightness(32);
    ledOff();

    WiFi.mode(WIFI_OFF);

    Serial.println();
    Serial.println("=====================================");
    Serial.println(" ESP32 POWER CONSUMPTION BENCHMARK");
    Serial.println("=====================================");

    Serial.printf("Board profile: %s\n", BOARD_NAME);

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
        Serial.println("Wake-up: timer after deep sleep.");
        Serial.printf(
            "Completed deep sleep cycles: %lu\n",
            (unsigned long)deepSleepCount
        );
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
        case 'r': testRGB(); break;
        case 'l': testLightSleep(); break;
        case 'd': testDeepSleep(); break;
        case 'a': runAllTests(); break;
        case '?': printMenu(); break;
        case '\r':
        case '\n':
            break;
        default:
            Serial.println("Unknown command. Press ? for menu.");
            break;
    }
}
