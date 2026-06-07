#include <Arduino.h>

/* -------- Status LED -------- */
constexpr int STATUS_LED_PIN = 2;
constexpr int SHORT_DELAY = 2000;

/* -------- Motor Driver 1 (Back Left and Back Right) -------- */
constexpr int DRIVER1_BIN2_PIN = 4;
constexpr int DRIVER1_BIN1_PIN = 13;
constexpr int DRIVER1_AIN2_PIN = 14;
constexpr int DRIVER1_AIN1_PIN = 18;
constexpr int DRIVER1_PWMB_PIN = 19;
constexpr int DRIVER1_PWMA_PIN = 21;

/* -------- Motor Driver 2 -------- */
constexpr int DRIVER2_BIN2_PIN = 32;
constexpr int DRIVER2_BIN1_PIN = 27;
constexpr int DRIVER2_AIN2_PIN = 26;
constexpr int DRIVER2_AIN1_PIN = 25;
constexpr int DRIVER2_PWMB_PIN = 23;
constexpr int DRIVER2_PWMA_PIN = 22;

/* -------- Motor PWM Channels -------- */
constexpr int FRONT_LEFT_CHANNEL = 1;

/* -------- Configuration Constants -------- */
constexpr int STBY_PIN = 33;  // This pin is unconnected becuase STBY is always pulled high.
constexpr int IS_REVERSED_FALSE = 1;
constexpr int IS_REVERSED_TRUE = -1;
constexpr int PWM_FREQUENCY = 5000;
constexpr int PWM_RESOLUTION = 8;
constexpr int MAX_PWM = 255;

void setup() {
}

void loop() {

}
