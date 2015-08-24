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
int allowError = 5;//电涌误差

void benchmarkSetup(){
  int i;
  int xSum = 0;
  int ySum = 0;
  for(i = 0; i < 3; i++){
    xSum += analogRead(axisX);
    ySum += analogRead(axisY);
    delay(10);
  }
  benchmarkX = xSum / i;
  benchmarkY = ySum / i;
}

//手柄值归一量
double xNol;
double yNol;
//履带归一值
double lTransNol;
double rTransNol;
//履带传输值
int lTrans;
int rTrans;

//根据模拟量输出值
void driveTracks(int xValue, int yValue){
  int xDValue = xValue - benchmarkX;
  int yDValue = yValue - benchmarkY;
  if(abs(xDValue) < allowError && abs(yDValue) < allowError){
    //在基准上下误差内
    return;
  }
  if(xDValue >= 0){
    xNol = 1.0 * xDValue / (1023-benchmarkX);
  }
  else{
    xNol = 1.0 * xDValue / benchmarkX;
  }
  xNol = -xNol;//因为手柄电路上X轴是反的
  if(yDValue >= 0){
    yNol = 1.0 * yDValue / (1023-benchmarkY);
  }
  else{
    yNol = 1.0 * yDValue / benchmarkY;
  }
  if(xNol >= 0 && yNol >= 0){
    lTransNol = max(xNol, yNol);
    rTransNol = yNol - xNol;
  }
  else if(xNol < 0 && yNol >= 0){
    lTransNol = xNol + yNol;
    rTransNol = max(-xNol, yNol);
  }
  //下部代码由ZY同学提供数学支持（我太笨了哦天）
  else if(xNol < 0 && yNol < 0){
//    lTransNol = -max(-xNol, -yNol);
//    rTransNol = yNol - xNol;
    lTransNol = min(xNol, yNol) - max(xNol, yNol);
    if(xNol > yNol){
      rTransNol = yNol;
    }
    else{
      rTransNol = 2*yNol - xNol;
    }
  }
  else if(xNol >= 0 && yNol < 0){
//    lTransNol = xNol + yNol;
//    rTransNol = -max(xNol, -yNol);
    rTransNol = min(-xNol, yNol) - max(-xNol, yNol);
    if(-xNol > yNol){
      lTransNol = yNol;
    }
    else{
      lTransNol = 2*yNol + xNol;
    }
  }
  lTrans = lTransNol * 127;
  rTrans = rTransNol * 127;
  sendOrder('T');
}

void sendOrder(char param){
  switch(param){
    case 'T'://履带运动
      Serial.print("TT");
      Serial.write(lTrans);
      Serial.write(rTrans);
      Serial.print("\n");
      break;
    default:break;
  }
}

void setup(){
  Serial.begin(9600);
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


