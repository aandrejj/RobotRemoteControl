long y;

#define MAXVAL 128
#define INTERPOLATION 4
#define MAX_PER_LINE 32
long prevY;
float delta_y;

float delta_per_i;

int lineCounter =0;
void setup() {
  Serial.begin(9600);
  //while (!Serial.available());

  Serial.println("Start");
  prevY = -1*MAXVAL*INTERPOLATION;
  prevY = prevY-10;

  for (int x=-MAXVAL; x<=MAXVAL; x++) {
    //y = x2 / ((512*512)/512);
    //y = pow(x,3);

    y = INTERPOLATION * ((pow(x,3)) /(MAXVAL * MAXVAL));


    //y = (x*x) /(MAXVAL);
    //y = (y*x) /(MAXVAL);

    delta_per_i = (y-prevY)/INTERPOLATION;

    for (int i=0; i<=(INTERPOLATION-1); i++) {
      delta_y = delta_per_i *i;
     //Serial.println("x= "+String(x)+", i="+String(i)+", delta_per_i="+String(delta_per_i)+", delta_y="+String(delta_y)+",,,,  x+i ="+String((INTERPOLATION * x)+i)+",  y = "+String(int(y+delta_y)) );
     Serial.print(String(int(y+delta_y)+512)+", ");
     prevY = y;
     
     lineCounter = lineCounter +1;
     if(lineCounter>= MAX_PER_LINE) {
      Serial.println();
      lineCounter =0;
     }
    }
    //Serial.println("");
  }
}

// the loop function runs over and over again forever
void loop( void) 
{
}
