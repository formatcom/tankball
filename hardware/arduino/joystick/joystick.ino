/*

   $ sudo rfcomm connect 0 98:D3:41:F5:D9:D1

   -- MODO DEBUG BLUETOOTH

   $ sudo picocom -b 9600 --omap crcrlf --imap spchex /dev/ttyUSB0
   $ sudo picocom -b 9600 --omap crcrlf --imap spchex /dev/rfcomm0

   -- MODO 2 AT

   $ sudo picocom -b 38400 -c --omap crcrlf /dev/ttyUSB0

 */


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

#if defined(HAVE_HWSERIAL1)
#define BTSerial Serial1
#else
#define BTSerial Serial
#endif

// Por si se quire hacer animacion
uint8_t LED[] = {LED_RED, LED_GREEN, LED_BLUE};

// Estados de los botones
uint8_t buttons;
uint8_t lastButtons;


void setup()
{
	pinMode(BTN_ACTION, INPUT_PULLUP);
	pinMode(BTN_LEFT,   INPUT_PULLUP);
	pinMode(BTN_RIGHT,  INPUT_PULLUP);

	BTSerial.begin(9600);
}

void loop()
{
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
