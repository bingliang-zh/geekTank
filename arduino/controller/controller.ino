//右边的按键
int upBtn = 2;
int downBtn = 4;
int leftBtn = 3;
int rightBtn = 5;

//摇杆
int axisX = A0;
int axisY = A1;
int stickBtn = 6;

//特定功能键
int stopBtn = 3;
bool isStopBtnPressed = false;

void setup(){
  Serial.begin(9600);
  //Serial.println("------------setup()-------------");
}

void loop(){
  int sensorValue  = digitalRead(stopBtn);
  if (sensorValue == 0||isStopBtnPressed == true){
    //按下停止按钮后改变判断值，进入死循环，只有重启可以解除该状态
    isStopBtnPressed = true;
    return;
  }
}

