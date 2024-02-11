#ifndef MyLcd_ShowMeasuredData_h
#define MyLcd_ShowMeasuredData_h

#include "Arduino.h"
#include "FormData.h"

class MyLcd_ShowMeasuredData {
	public:
	MyLcd_ShowMeasuredData();

	void begin();

	FormData showForm(int leftJoystick_X, int rightJoystick_X, int leftJoystick_Y, int rightJoystick_Y, String btnsString, String lcdString3);

};
#endif