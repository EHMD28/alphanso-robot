#include <Arduino.h>
#include <PS4Controller.h>

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
MotorChannel front_left_motor = MotorChannel(DRIVER1_AIN1_PIN, DRIVER1_AIN2_PIN, DRIVER1_PWMA_PIN,
                                        FRONT_LEFT_CHANNEL, true);
MotorChannel back_left_motor = MotorChannel(DRIVER1_BIN1_PIN, DRIVER1_BIN2_PIN, DRIVER1_PWMB_PIN,
                                        BACK_LEFT_CHANNEL, true);
MotorChannel front_right_motor = MotorChannel(DRIVER2_AIN1_PIN, DRIVER2_AIN2_PIN, DRIVER2_PWMA_PIN,
                                        FRONT_RIGHT_CHANNEL, false);
MotorChannel back_right_motor = MotorChannel(DRIVER2_BIN1_PIN, DRIVER2_BIN2_PIN, DRIVER2_PWMB_PIN,
                                        BACK_RIGHT_CHANNEL, true);

/**
 * Drive the robot using inputs from a joystick.
 * 
 * @param fwd The speed of the robot in the forwards-backwards direction.
 * Forwards is positive, backwards is negative. Range: [-1.0, 1.0].
 * @param strafe The speed of the robot in the side-to-side direction.
 * Right is positive, left is negative. Range: [-1.0, 1.0].
 * @param rot The speed to rotate the robot. Clockwise is positive (TODO: Check
 * this). Range: [-1.0, 1.0].
 */
void drive(double fwd, double strafe, double rot) {
    // Front left motor power
    double fl = fwd + rot + strafe;
    // Front right motor power
    double fr = fwd - rot - strafe;
    // Back left motor power
    double bl = fwd + rot - strafe;
    // Back right motor power
    double br = fwd - rot + strafe;
    /* Normalize motor powers if any of them exceed 1.0 in magnitude. */
    double max_val = max({abs(fl), abs(fr), abs(bl), abs(br)});
    if (max_val > 1.0) {
        fl /= max_val;
        fr /= max_val;
        bl /= max_val;
        br /= max_val;
    }
    /* Convert from percentages to duty cycles */
    int16_t fl_speed = fl * MAX_MOTOR_PWM;
    int16_t fr_speed = fr * MAX_MOTOR_PWM;
    int16_t bl_speed = bl * MAX_MOTOR_PWM;
    int16_t br_speed = br * MAX_MOTOR_PWM;
    /* Drive each motor */
    front_left_motor.driveMotor(fl_speed);
    front_right_motor.driveMotor(fr_speed);
    back_left_motor.driveMotor(bl_speed);
    back_right_motor.driveMotor(br_speed);
}

void test_drive() {
    front_left_motor.driveMotor(100);
    delay(SHORT_DELAY);
    front_left_motor.stopMotor();
    front_right_motor.driveMotor(100);
    delay(SHORT_DELAY);
    front_right_motor.stopMotor();
    back_right_motor.driveMotor(100);
    delay(SHORT_DELAY);
    back_right_motor.stopMotor();
    back_left_motor.driveMotor(100);
    delay(SHORT_DELAY);
    back_left_motor.stopMotor();
}

/* -------- Controller -------- */
// This is the of the device the controller is paired with (not the controller itself).
const char* PS4_MAC_ADDRESS = "44:85:00:A0:09:0F";
// Through my testing, all values in the range of [-10, 10] (out of [-128, 127]) can be ignored as stick drift.
constexpr int8_t JOYSTICK_DEADBAND = 10;
constexpr int8_t MAX_JOYSTICK_INPUT = 128;

int8_t apply_deadband(int8_t input) {
    return (abs(input) <= JOYSTICK_DEADBAND) ? 0 : input;
}

void setup() {
    pinMode(STATUS_LED_PIN, OUTPUT);
    PS4.begin(PS4_MAC_ADDRESS);    
    while (!PS4.isConnected()) {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(500);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(500);
    }
    digitalWrite(STATUS_LED_PIN, HIGH);
}

void loop() {
    /* Raw joystick input values. Right is +x, left is +y, and range is [-128, 127]. */
    int8_t raw_lx = PS4.LStickX();
    int8_t raw_ly = PS4.LStickY();
    int8_t raw_rx = PS4.RStickX();
    /* Convert raw inputs into percentages on the interval [-1.0, 1.0]. */
    double lx = (double) apply_deadband(raw_lx) / MAX_JOYSTICK_INPUT;
    double ly = (double) apply_deadband(raw_ly) / MAX_JOYSTICK_INPUT;
    double rx = (double) apply_deadband(raw_rx) / MAX_JOYSTICK_INPUT;
    /* Drive robot using the controller inputs. Left joystick controls translation, right joystick controls rotation. */
    drive(ly, lx, rx);
    delay(100);
}
