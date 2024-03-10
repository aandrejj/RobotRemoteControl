#define ROTARY_ENCODER_PIN1 2
#define ROTARY_ENCODER_PIN2 3
#define ROTARY_ENCODER_KEY  4

#define LCD_ROWS 4
#define LCD_COLS 20

#define BUTTON1 5
#define BUTTON2 6
#define BUTTON3 7
#define BUTTON4 33
#define BUTTON5 32

#define NAV_KEY_UP 46
#define NAV_KEY_DWN 48
#define NAV_KEY_LFT 50
#define NAV_KEY_RHT 52
#define NAV_KEY_MID 53
#define NAV_KEY_SET 51
#define NAV_KEY_RST 49


#define BLUETOOTH_SWITCH 35
#define DISPLAY_SWITCH 30

#define SWITCH1Up 20
#define SWITCH2Up 49
#define SWITCH3Up 50
#define SWITCH4Up 51
#define SWITCH5Up 52
#define SWITCH6Up 53

#define SWITCH1Down 54
#define SWITCH2Down 55
#define SWITCH3Down 56
#define SWITCH4Down 57
#define SWITCH5Down 58
#define SWITCH6Down 59


// Outcomment line below for HM-10, HM-19 etc
//#define HIGHSPEED   // Most modules are only 9600, although you can reconfigure this
#define EN_PIN_HIGH   // You can use this for HC-05 so you don't have to hold the small button on power-up to get to AT-mode

#ifdef HIGHSPEED
  #define Baud 38400   // Serial monitor
  #define BTBaud 38400 // There is only one speed for configuring HC-05, and that is 38400.
#else
  #define Baud 9600    // Serial monitor
  #define BTBaud 9600  // HM-10, HM-19 etc
#endif


#define STATE 11
#define BLUETOOTH_RX 9  // Bluetooth RX -> Arduino D9
#define BLUETOOTH_TX 10 // Bluetooth TX -> Arduino D10
//#define GND 13
//#define Vcc 12
#define ENABLE 8

// Configure keyboard keys (ASCII)
#define UP 56     // NUMPAD 8
#define DOWN 50   // NUMPAD 2
#define ENTER 53  // NUMPAD 5
#define BACK 55   // NUMPAD 7


bool rotary_key;
bool previous_rotary_key;
bool button1;
bool button2;
bool button3;
bool button4;
bool button5;

bool navKeyUp    ;
bool navKeyDown  ;
bool navKeyLeft  ;
bool navKeyRight ;
bool navKeyMiddle;
bool navKeySet   ;
bool navKeyReset ;

bool bluetooth_On;
bool previous_Bluetooth_State;
bool bluetooth_initialized;
bool bluetooth_connecting;
bool bluetooth_connected;

byte showForm;

//bool bt_State;

bool showDataOnDisplay;
bool menuIsShown;

bool switch1Up;
bool switch2Up;
bool switch3Up;
bool switch4Up;
bool switch5Up;
bool switch6Up;

bool switch1Down;
bool switch2Down;
bool switch3Down;
bool switch4Down;
bool switch5Down;
bool switch6Down;


int rightJoystick_X;
int rightJoystick_Y;
int leftJoystick_X;
int leftJoystick_Y;

String count;

String sendToBT;

static const unsigned int lookup[] {
0, 2, 4, 6, 12, 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45, 47, 49, 51, 53, 58, 60, 62, 64, 69, 71, 73, 75, 80, 82, 84, 86, 
91, 93, 95, 97, 101, 103, 105, 107, 111, 113, 115, 117, 121, 123, 125, 127, 131, 133, 135, 137, 141, 143, 145, 147, 151, 153, 155, 157, 160, 162, 164, 166, 
170, 172, 174, 176, 179, 181, 183, 185, 188, 190, 192, 194, 196, 198, 200, 202, 205, 207, 209, 211, 213, 215, 217, 219, 222, 224, 226, 228, 230, 232, 234, 236, 
238, 240, 242, 244, 246, 248, 250, 252, 253, 254, 255, 256, 261, 263, 265, 267, 268, 269, 270, 271, 276, 278, 280, 282, 283, 284, 285, 286, 290, 291, 292, 293, 
297, 298, 299, 300, 303, 304, 305, 306, 310, 311, 312, 313, 316, 317, 318, 319, 322, 323, 324, 325, 329, 330, 331, 332, 335, 336, 337, 338, 340, 341, 342, 343, 
346, 347, 348, 349, 352, 353, 354, 355, 357, 358, 359, 360, 363, 364, 365, 366, 368, 369, 370, 371, 373, 374, 375, 376, 378, 379, 380, 381, 383, 384, 385, 386, 
388, 389, 390, 391, 392, 393, 394, 395, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 
421, 421, 421, 421, 425, 426, 427, 428, 429, 430, 431, 432, 432, 432, 432, 432, 436, 437, 438, 439, 439, 439, 439, 439, 442, 442, 442, 442, 445, 445, 445, 445, 
449, 450, 451, 452, 451, 451, 451, 451, 454, 454, 454, 454, 457, 457, 457, 457, 460, 460, 460, 460, 462, 462, 462, 462, 465, 465, 465, 465, 467, 467, 467, 467, 
470, 470, 470, 470, 472, 472, 472, 472, 474, 474, 474, 474, 476, 476, 476, 476, 478, 478, 478, 478, 480, 480, 480, 480, 482, 482, 482, 482, 484, 484, 484, 484, 
486, 486, 486, 486, 487, 487, 487, 487, 489, 489, 489, 489, 490, 490, 490, 490, 492, 492, 492, 492, 493, 493, 493, 493, 494, 494, 494, 494, 496, 496, 496, 496, 
497, 497, 497, 497, 498, 498, 498, 498, 499, 499, 499, 499, 500, 500, 500, 500, 501, 501, 501, 501, 502, 502, 502, 502, 503, 503, 503, 503, 504, 504, 504, 504, 
505, 505, 505, 505, 505, 505, 505, 505, 506, 506, 506, 506, 507, 507, 507, 507, 507, 507, 507, 507, 508, 508, 508, 508, 508, 508, 508, 508, 509, 509, 509, 509, 
509, 509, 509, 509, 510, 510, 510, 510, 510, 510, 510, 510, 510, 510, 510, 510, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 511, 
512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 
512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 
512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 
512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 512, 
512, 512, 512, 512, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 513, 514, 514, 514, 514, 514, 514, 514, 514, 514, 514, 514, 514, 
515, 515, 515, 515, 515, 515, 515, 515, 516, 516, 516, 516, 516, 516, 516, 516, 517, 517, 517, 517, 517, 517, 517, 517, 518, 518, 518, 518, 519, 519, 519, 519, 
519, 519, 519, 519, 520, 520, 520, 520, 521, 521, 521, 521, 522, 522, 522, 522, 523, 523, 523, 523, 524, 524, 524, 524, 525, 525, 525, 525, 526, 526, 526, 526, 
527, 527, 527, 527, 528, 528, 528, 528, 530, 530, 530, 530, 531, 531, 531, 531, 532, 532, 532, 532, 534, 534, 534, 534, 535, 535, 535, 535, 537, 537, 537, 537, 
538, 538, 538, 538, 540, 540, 540, 540, 542, 542, 542, 542, 544, 544, 544, 544, 546, 546, 546, 546, 548, 548, 548, 548, 550, 550, 550, 550, 552, 552, 552, 552, 
554, 554, 554, 554, 557, 557, 557, 557, 559, 559, 559, 559, 562, 562, 562, 562, 564, 564, 564, 564, 567, 567, 567, 567, 570, 570, 570, 570, 573, 573, 573, 573, 
575, 575, 575, 575, 579, 580, 581, 582, 582, 582, 582, 582, 585, 585, 585, 585, 588, 588, 588, 588, 592, 593, 594, 595, 595, 595, 595, 595, 599, 600, 601, 602, 
603, 604, 605, 606, 606, 606, 606, 606, 610, 611, 612, 613, 614, 615, 616, 617, 619, 620, 621, 622, 623, 624, 625, 626, 627, 628, 629, 630, 632, 633, 634, 635, 
636, 637, 638, 639, 641, 642, 643, 644, 646, 647, 648, 649, 651, 652, 653, 654, 656, 657, 658, 659, 661, 662, 663, 664, 667, 668, 669, 670, 672, 673, 674, 675, 
678, 679, 680, 681, 684, 685, 686, 687, 689, 690, 691, 692, 695, 696, 697, 698, 702, 703, 704, 705, 708, 709, 710, 711, 714, 715, 716, 717, 721, 722, 723, 724, 
727, 728, 729, 730, 734, 735, 736, 737, 741, 742, 743, 744, 748, 749, 750, 751, 756, 758, 760, 762, 763, 764, 765, 766, 771, 773, 775, 777, 778, 779, 780, 781, 
786, 788, 790, 792, 794, 796, 798, 800, 802, 804, 806, 808, 811, 813, 815, 817, 819, 821, 823, 825, 828, 830, 832, 834, 836, 838, 840, 842, 845, 847, 849, 851, 
854, 856, 858, 860, 864, 866, 868, 870, 873, 875, 877, 879, 883, 885, 887, 889, 893, 895, 897, 899, 903, 905, 907, 909, 913, 915, 917, 919, 923, 925, 927, 929, 
933, 935, 937, 939, 944, 946, 948, 950, 955, 957, 959, 961, 966, 968, 970, 972, 977, 979, 981, 983, 988, 990, 992, 994, 1000, 1003, 1006, 1009, 1012, 1015, 1018, 1021, 
1024, 1027, 1030, 1033, 
};