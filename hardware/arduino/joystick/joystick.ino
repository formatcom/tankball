/*

   $ sudo rfcomm connect 0 98:D3:41:F5:D9:D1

   -- MODO DEBUG BLUETOOTH

   $ sudo picocom -b 9600 --omap crcrlf --imap spchex /dev/ttyUSB0
   $ sudo picocom -b 9600 --omap crcrlf --imap spchex /dev/rfcomm0

   -- MODO 2 AT

   $ sudo picocom -b 38400 -c --omap crcrlf /dev/ttyUSB0

 */

#include <math.h>
#include <Wire.h>

#define LED_RED   5
#define LED_GREEN 6
#define LED_BLUE  7

#define BTN_ACTION 2
#define BTN_RIGHT  3
#define BTN_LEFT   4

#define STATE_BTN_ACTION 1<<0
#define STATE_BTN_RIGHT  1<<1
#define STATE_BTN_LEFT   1<<2

#define STATE_LED_RED    1<<0
#define STATE_LED_GREEN  1<<1
#define STATE_LED_BLUE   1<<2

#if defined(__AVR_ATmega32U4__)
#define BTSerial Serial1
#else
#define BTSerial Serial
#endif

#define SENSOR_TRG A4
#define SENSOR_VIN A3

#if defined(SENSOR_ULTRASONIC)
#define SENSOR_VCC A5
#define SENSOR_GND A2
#else
// MPU6050 VCC -> 3.3v  // only enabled support in atmega328p
// MPU6050 GND ->  GND
#define SENSOR_VCC A2 // no usado, solo para evitar colisión
#define SENSOR_GND A3 // no usado, solo para evitar colisión
#endif

// REF: https://www.arduino.cc/en/reference/wire
// REF: https://www.arduino.cc/en/Tutorial/MasterWriter

// REF: https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/
// REF: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
// REF: 7.13 MPU-60X0 Solution for 9-axis Sensor Fusion Using I2C Interface
// REF: 7.16 Sensor Data Registers
// REF: 9.1  I2C and SPI (MPU-6000 only) Serial Interfaces
// REF: 9.2  I2C Interface
// REF: 9.3  I2C Communications Protocol PAG: 36

// REF: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
// REF: 3    Register Map
// REF: 4.3  Register 26        – Configuration
// REF: 4.5  Register 28        – Accelerometer Configuration
// REF: 4.4  Register 27        – Gyroscope Configuration
// REF: 4.28 Register 107       – Power Management 1
// REF: 4.17 Registers 59 to 64 – Accelerometer Measurements
// REF: 4.19 Registers 67 to 72 – Gyroscope Measurements

// MPU-6050

#define RAD2DEG            ( 180.0 / M_PI )
#define MAXINT16           ( (1<<15) - 1  ) // 32767

// AFS_SEL   Full Scale Range     LSB Sensitivity
//       0             ±  2g         16384 LSB/g     <- SELECTED
//       1             ±  4g          8192 LSB/g
//       2             ±  8g          4096 LSB/g
//       3             ± 16g          2048 LSB/g

#define ACCEL_LBS (MAXINT16/2) // 16384 LSB/g


//  FS_SEL   Full Scale Range     LSB Sensitivity
//       0         ±  250 °/s         131 LSB/°/s     <- SELECTED
//       1         ±  500 °/s        65.5 LSB/°/s
//       2         ± 1000 °/s        32.8 LSB/°/s
//       3         ± 2000 °/s        16.4 LSB/°/s

#define GYRO_LBS  (MAXINT16/250) // 131 LSB/°/s

// AD0 LOW ADDR 1101000 or AD0 HIGH ADDR 1101001
#define MPU6050_ADDR 0x68
#define MPU6050_SCL  A5
#define MPU6050_SDA  A4

#define MPU6050_PWR_MGMT_1         0x6B
#define MPU6050_ACCEL_CONFIG       0x1C
#define MPU6050_GYRO_CONFIG        0x1B

#define MPU6050_ACCEL_ADDR     0x3B
#define MPU6050_GYRO_ADDR      0x43

// Por si se quire hacer animacion
uint8_t LED[] = {LED_RED, LED_GREEN, LED_BLUE};

// Estados de los botones
uint8_t buttons;
uint8_t lastButtons;

unsigned long  time0;
float delta;

float _angle; // pre angle degress

void setup()
{
	pinMode(BTN_ACTION, INPUT_PULLUP);
	pinMode(BTN_LEFT,   INPUT_PULLUP);
	pinMode(BTN_RIGHT,  INPUT_PULLUP);

	pinMode(SENSOR_VCC, OUTPUT);
	pinMode(SENSOR_GND, OUTPUT);

	digitalWrite(SENSOR_VCC, HIGH);
	digitalWrite(SENSOR_GND,  LOW);

#if defined(SENSOR_ULTRASONIC)
	pinMode(SENSOR_TRG, OUTPUT);
	pinMode(SENSOR_VIN, INPUT_PULLUP);
#else
	Wire.begin();

	// wake up
	Wire.beginTransmission(MPU6050_ADDR);
	Wire.write(MPU6050_PWR_MGMT_1);
	Wire.write(0);
	Wire.endTransmission();

	Wire.beginTransmission(MPU6050_ADDR);
	Wire.write(MPU6050_ACCEL_CONFIG);
	Wire.write(0);             // ± 2g | 16384 LSB/g
	Wire.endTransmission();

	Wire.beginTransmission(MPU6050_ADDR);
	Wire.write(MPU6050_GYRO_CONFIG);
	Wire.write(0);             // ± 250 °/s | 131.0 LSB Sensitivity
	Wire.endTransmission();
	delay(500);

#endif


	BTSerial.begin(9600);

	delay(1000);

}

// REF: https://www.arduino.cc/en/Tutorial/Ping
long microsecondsToInches(long microseconds) {
	// According to Parallax's datasheet for the PING))), there are 73.746
	// microseconds per inch (i.e. sound travels at 1130 feet per second).
	// This gives the distance travelled by the ping, outbound and return,
	// so we divide by 2 to get the distance of the obstacle.
	// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
	return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// The ping travels out and back, so to find the distance of the object we
	// take half of the distance travelled.
	return microseconds / 29 / 2;
}

// REF: https://www.electronicwings.com/avr-atmega/ultrasonic-module-hc-sr04-interfacing-with-atmega1632
uint8_t getAngle()
{
	uint8_t angle;

#if defined(SENSOR_ULTRASONIC)
	long duration, cm;

	// PING
	digitalWrite(SENSOR_TRG,  LOW);
	delayMicroseconds(2);
	digitalWrite(SENSOR_TRG, HIGH);
	delayMicroseconds(5);
	digitalWrite(SENSOR_TRG,  LOW);

	// DEBUG ULTRASONIC SENSOR
	/*
	for (int i = 0; i < 190;  i++)
	{
		Serial.print(digitalRead(SENSOR_VIN));
	}
	*/

	while (digitalRead(SENSOR_VIN) == LOW);  // wait capture on rising edge

	time0 = micros();

	while (digitalRead(SENSOR_VIN) == HIGH); // wait capture on falling edge

	duration = micros() - time0;

	// convert
	// inches = microsecondsToInches      (duration);
	cm     = microsecondsToCentimeters (duration);

	// fix 0 cm
	if (cm > 1000) cm = 0;

	if (cm <= 40) angle = cm * 4.5;
	else          angle = 180;
#else

	Wire.beginTransmission(MPU6050_ADDR);
	Wire.write(MPU6050_ACCEL_ADDR);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU6050_ADDR, 3*2); // accel x, y, z

	float accelx = Wire.read()<<8 | Wire.read();
	float accely = Wire.read()<<8 | Wire.read();
	float accelz = Wire.read()<<8 | Wire.read();

	// triangulo rectangulo, para calcular el angulo
	float ax = (atan(accely / sqrt(pow(accelx, 2) + pow(accelz, 2))) * 180 / PI) - 0.58;

	Wire.beginTransmission(MPU6050_ADDR);
	Wire.write(MPU6050_GYRO_ADDR);
	Wire.endTransmission(false);

	Wire.requestFrom(MPU6050_ADDR, 1*2); // gyro only x

	float gyrox = Wire.read()<<8 | Wire.read();

	float gx = gyrox/GYRO_LBS;

	delta = (millis() - time0) / 1000.0;
	time0 =  millis();

	// apply complementary filter
	_angle = 0.96 * (_angle+gx*delta) + 0.04*ax;

	// compensar el angulo
	float a = _angle + 17;

	if (a >  90) a =  90;
	if (a < -90) a = -90;

	// ajustar angulo
	if (a > 0) angle = a;
	else       angle = a + 180;

#endif

	angle = (uint8_t)( angle * 0.17 ) & 0x1F; // compress

	return angle;
}

void loop()
{

	delay(100);

	if (!digitalRead(BTN_ACTION))
	{
		buttons |= STATE_BTN_ACTION;
	}

	if (!digitalRead(BTN_RIGHT))
	{
		buttons |= STATE_BTN_RIGHT;
	}

	if (!digitalRead(BTN_LEFT))
	{
		buttons |= STATE_BTN_LEFT;
	}

	uint8_t angle = getAngle();

	buttons |= angle << 3;

	if (lastButtons != buttons)
	{
		if (BTSerial.availableForWrite())
		{
			BTSerial.write(buttons);
		}
	}

	if (BTSerial.available())
	{
		uint8_t b = BTSerial.read();

		digitalWrite(LED_RED,   !!(b & STATE_LED_RED  ));
		digitalWrite(LED_GREEN, !!(b & STATE_LED_GREEN));
		digitalWrite(LED_BLUE,  !!(b & STATE_LED_BLUE ));
	}

	lastButtons = buttons;
	buttons = 0;
}
