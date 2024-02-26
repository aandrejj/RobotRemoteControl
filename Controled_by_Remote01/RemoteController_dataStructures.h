struct RECEIVE_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  
    bool menuDown;      
    bool Select; 
    bool menuUp;  
    bool toggleBottom;  
    bool toggleTop; 
    int mode;  

    int16_t thumb_fingertip;
    int16_t thumb_knuckle_left;
    int16_t thumb_knuckle_right;

    int16_t index_finger_fingertip;
    int16_t index_finger_knuckle_left;
    int16_t index_finger_knuckle_right;

    int16_t middle_finger_fingertip;
    int16_t middle_finger_knuckle_left;
    int16_t middle_finger_knuckle_right;
    
    int16_t ring_finger_fingertip;
    int16_t ring_finger_knuckle_left;
    int16_t ring_finger_knuckle_right;

    int16_t pinky_fingertip;
    int16_t pinky_knuckle_left;
    int16_t pinky_knuckle_right;

};

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to receive
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int16_t mode;
  int16_t count;
};

