int F = 2;
int B = 4;
int L = 3;
int R = 5;

void setup(){
  Serial.begin(9600);
  Serial.println("------------setup()-------------");
}

void loop(){
  if (digitalRead(F) == 0){
    Serial.println("F");
  }
  else if(digitalRead(B) == 0){
    Serial.println("B");
  }
  else if(digitalRead(L) == 0){
    Serial.println("L");
  }
  else if(digitalRead(R) == 0){
    Serial.println("R");
  }
  else{
    Serial.println("S");
  }
}

