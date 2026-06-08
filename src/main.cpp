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

            pinMode(in1_pin, OUTPUT);
            pinMode(in2_pin, OUTPUT);
            pinMode(pwm_pin, OUTPUT);

            ledcAttachPin(pwm_pin, channel);
            ledcSetup(channel, MOTOR_PWM_FREQUENCY, MOTOR_RESOLUTION);

            stopMotor();
        }

        /**
         * Brakes the motor and sets the duty cycle to 0.
         */
        void stopMotor() {
            digitalWrite(in1_pin, LOW);
            digitalWrite(in2_pin, LOW);
            ledcWrite(pwm_channel, 0);
        }
        
        /**
         * @param speed The speed and direction to drive the motor. Value: [-255, 255].
         */
        void driveMotor(int16_t speed) {
            speed *= is_reversed ? -1 : 1;
            /* CCW rotation is positive */
            if (speed > 0) {
                drive_ccw(speed);
            } else if (speed < 0) {
                drive_cw(-speed);
            } else {
                stopMotor();
            }
        }
};

/* -------- Status LED -------- */
constexpr uint8_t STATUS_LED_PIN = 2;
constexpr uint16_t SHORT_DELAY = 2000;

/* -------- Motor Driver 1 (Left Side) -------- */
constexpr uint8_t DRIVER1_BIN2_PIN = 4;
constexpr uint8_t DRIVER1_BIN1_PIN = 13;
constexpr uint8_t DRIVER1_AIN2_PIN = 14;
constexpr uint8_t DRIVER1_AIN1_PIN = 18;
constexpr uint8_t DRIVER1_PWMB_PIN = 19;
constexpr uint8_t DRIVER1_PWMA_PIN = 21;

/* -------- Motor Driver 2 (Right Side) -------- */
constexpr uint8_t DRIVER2_BIN2_PIN = 32;
constexpr uint8_t DRIVER2_BIN1_PIN = 27;
constexpr uint8_t DRIVER2_AIN2_PIN = 26;
constexpr uint8_t DRIVER2_AIN1_PIN = 25;
constexpr uint8_t DRIVER2_PWMB_PIN = 23;
constexpr uint8_t DRIVER2_PWMA_PIN = 22;

/* -------- Motor PWM Channels -------- */
constexpr uint8_t FRONT_LEFT_CHANNEL = 1;
constexpr uint8_t BACK_LEFT_CHANNEL = 2;
constexpr uint8_t FRONT_RIGHT_CHANNEL = 3;
constexpr uint8_t BACK_RIGHT_CHANNEL = 4;

/* -------- Drivetrain -------- */
/* The last argument to each constructor is whether or not the motor is reversed. */
MotorChannel front_left = MotorChannel(DRIVER1_AIN1_PIN, DRIVER1_AIN2_PIN, DRIVER1_PWMA_PIN,
                                        FRONT_LEFT_CHANNEL, true);
MotorChannel back_left = MotorChannel(DRIVER1_BIN1_PIN, DRIVER1_BIN2_PIN, DRIVER1_PWMB_PIN,
                                        BACK_LEFT_CHANNEL, true);
MotorChannel front_right = MotorChannel(DRIVER2_AIN1_PIN, DRIVER2_AIN2_PIN, DRIVER2_PWMA_PIN,
                                        FRONT_RIGHT_CHANNEL, false);
MotorChannel back_right = MotorChannel(DRIVER2_BIN1_PIN, DRIVER2_BIN2_PIN, DRIVER2_PWMB_PIN,
                                        BACK_RIGHT_CHANNEL, true);
                                        
void setup() {
    pinMode(STATUS_LED_PIN, OUTPUT);
    // digitalWrite(STATUS_LED_PIN, HIGH);
}

void test_drive() {
    front_left.driveMotor(100);
    delay(SHORT_DELAY);
    front_left.stopMotor();
    front_right.driveMotor(100);
    delay(SHORT_DELAY);
    front_right.stopMotor();
    back_right.driveMotor(100);
    delay(SHORT_DELAY);
    back_right.stopMotor();
    back_left.driveMotor(100);
    delay(SHORT_DELAY);
    back_left.stopMotor();
}

void loop() {
    test_drive();
}
