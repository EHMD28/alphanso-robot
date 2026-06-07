#include <Arduino.h>

/* -------- Motor Utilities -------- */

constexpr int MOTOR_PWM_FREQUENCY = 20000; // 20 kHz
constexpr int MOTOR_RESOLUTION = 8; // 8-bits [0, 255]
constexpr int MAX_MOTOR_PWM = 255;

/**
 * An interface for controlling a motor channel using the TB6612FNG motor
 * driver. Assumes that the STBY is externally pulled HIGH.
 */
class MotorChannel {
    private:
        uint8_t in1_pin;
        uint8_t in2_pin;
        uint8_t pwm_channel;
        bool is_reversed;

        void drive_cw(uint8_t duty_cycle) {
            digitalWrite(in1_pin, HIGH);
            digitalWrite(in2_pin, LOW);
            ledcWrite(pwm_channel, duty_cycle);
        }
        
        void drive_ccw(uint8_t duty_cycle) {
            digitalWrite(in1_pin, LOW);
            digitalWrite(in2_pin, HIGH);
            ledcWrite(pwm_channel, duty_cycle);
        }
        
    public:
        MotorChannel(uint8_t in1_pin, uint8_t in2_pin, uint8_t pwm_pin, uint8_t channel, bool is_reversed) {
            this->in1_pin = in1_pin;
            this->in2_pin = in2_pin;
            this->pwm_channel = channel;
            this->is_reversed = is_reversed;

            ledcAttachPin(pwm_pin, channel);
            ledcSetup(channel, MOTOR_PWM_FREQUENCY, MOTOR_RESOLUTION);

            stopMotor();
        }

        /**
         * @param speed The speed and direction to drive the motor. Values: [-255, 255].
         */
        void driveMotor(int16_t speed) {
            speed *= is_reversed ? -1 : 1;
            // CCW Rotation is positive
            if (speed > 0) {
                drive_ccw(speed);
            } else if (speed < 0) {
                drive_cw(-speed);
            } else {
                stopMotor();
            }
        }

        /**
         * Brakes the motor and sets the duty cycle to 0.
         */
        void stopMotor() {
            digitalWrite(in1_pin, LOW);
            digitalWrite(in2_pin, LOW);
            ledcWrite(pwm_channel, 0);
        }
};

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

/* Drivetrain */
/* The last argument to each constructor is whether or not the motor is reversed. */
MotorChannel front_left = MotorChannel(DRIVER1_AIN1_PIN, DRIVER1_AIN2_PIN, DRIVER1_PWMA_PIN,
                                        FRONT_LEFT_CHANNEL, false);

void setup() {
}

void loop() {
    front_left.driveMotor(100);
    delay(2000);
    front_left.stopMotor();
    delay(2000);
    front_left.driveMotor(-100);
    delay(2000);
}
