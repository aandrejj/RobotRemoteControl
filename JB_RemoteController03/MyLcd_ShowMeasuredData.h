#ifndef MyLcd_ShowMeasuredData_h
#define MyLcd_ShowMeasuredData_h

#include "Arduino.h"
#include "FormData.h"

class MyLcd_ShowMeasuredData {
	public:
	MyLcd_ShowMeasuredData();

	void begin();

	FormData showForm4(int lUpJoy_X, int rUpJoy_X, int lDownJoy_X, int rDownJoy_X, int lUpJoy_Y, int rUpJoy_Y, int lDownJoy_Y, int rDownJoy_Y, String btnsString, String lcdString3);
	FormData showForm(    int leftJoystick_X,    int rightJoystick_X,    int leftJoystick_Y,    int rightJoystick_Y, String btnsString, String lcdString3);
	FormData showForm2(String leftJoystick_X, String rightJoystick_X, String leftJoystick_Y, String rightJoystick_Y, String btnsString, String lcdString3);

};
#endif