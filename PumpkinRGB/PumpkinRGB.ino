int blue = 3;
int green = 6;
int red = 9;

void setup() {
  // put your setup code here, to run once:
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  analogWrite(red, 0);
  analogWrite(green,0);
  analogWrite(blue,0);
}

void loop() {
  // put your main code here, to run repeatedly:

  for (int n = 0; n < 200; n++) {
     setRGB(0, n/2, n);
     delay(10);
  }

   for (int n = 200; n > 0; n--) {
     setRGB(0, n/2, n);
     delay(10);
  }

}

void setRGB(int r, int g, int b) {
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}
