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

//基准量
int benchmarkX = 512;
int benchmarkY = 512;

void benchmarkSetup(){
  int i;
  int xSum = 0;
  int ySum = 0;
  for(i = 0; i < 3; i++){
    xSum += analogRead(axisX);
    ySum += analogRead(axisY);
    delay(100);
  }
  benchmarkX = xSum / i;
  benchmarkY = ySum / i;
}

void setup(){
  Serial.begin(9600);
  //Serial.println("------------setup()-------------");
  benchmarkSetup();
}

//公用临时变量
int sensorValue;

void loop(){
  sensorValue = digitalRead(stopBtn);
  if (sensorValue == 0||isStopBtnPressed == true){
    //按下停止按钮后改变判断值，进入死循环，只有重启可以解除该状态
    isStopBtnPressed = true;
    return;
  }
  
  sensorValue = digitalRead(rightBtn);
  if (sensorValue == 0){
    //按下了摄像头居中按钮
    //...
  }
  else{
    int upValue = digitalRead(upBtn);
    int downValue = digitalRead(downBtn);
    if(upValue != downValue){
      //都只有一个按下一个不按下时才进入函数体
      if(upValue == 0){
        //按下了上按键
        //...
      }
      else{
        //按下了下按键
        //...
      }
    }
  }
  
  sensorValue = digitalRead(stickBtn);
  if (sensorValue == 0){
    //按下了摇杆按钮
    //...
  }

  int xValue = analogRead(axisX);
  int yValue = analogRead(axisY);
  driveTracks(xValue, yValue);  
}

void driveTracks(int xValue, int yValue){
  
}

