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
int stopBtn        = leftBtn;
int stickModeBtn   = upBtn;
int holdSpeedBtn   = rightBtn;
int disSensorBtn   = downBtn;
int camResetBtn    = stickBtn;

//Last Value
int lstVals[7];
//Recent Value
int rctVals[7];

//模式开关
bool isStopBtnPressed = false;
int stickMode   = 0;//0为控制履带，1为控制相机
int speedMode   = 0;//0为变速，1为匀速
//int sensorMode  = 0;//0为检测距离，1为纯手动

//基准量
int benchmarkX = 512;
int benchmarkY = 512;
int allowError = 10;//电涌误差
bool sendBreaks  = true;

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

//根据模拟量输出履带对应速度
void driveTracks(int xValue, int yValue){
  int xDValue = xValue - benchmarkX;
  int yDValue = yValue - benchmarkY;
  if(abs(xDValue) < allowError && abs(yDValue) < allowError){
    //在基准上下误差内
    if(sendBreaks){
      sendOrder('B');
      sendOrder('B');
      sendBreaks = false;
    }
    return;
  }else{
    sendBreaks = true;
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
  Serial.print("CMD");
  Serial.print(param);
  switch(param){
    case 'T':                   //履带运动 track
      Serial.write(lTrans);
      Serial.write(rTrans);
      break;
    case 'B': break;            //履带刹车 break
    case 'F': break;            //相机复位 focus
    case 'S': break;            //切换距离感应开关 sensor
    case ' ': break;            //空白，只亮灯，什么用都没
    default:break;
  }
  Serial.print("\n");
  delay(20);
}

void sendOrder(char param, int n){
  Serial.print("CMD");
  Serial.print(param);
  switch(param){
    case 'U':                   //相机抬头 up
      Serial.print(n);
      break;
    case 'D':                   //相机低头 down
      Serial.print(n);
      break;
    default:break;
  }
  Serial.print("\n");
  delay(30);
}

void setup(){
  Serial.begin(9600);
  for(int i = 0; i < 7; i++){
    lstVals[i] = 1;
  }
  benchmarkSetup();
}

void loop(){
  for(int i = 0; i < 7; i++){
    rctVals[i] = digitalRead(i);
  }
  if(rctVals[stopBtn] == 0||isStopBtnPressed == true){
    //按下停止按钮后改变判断值，进入死循环，只有重启可以解除该状态
    if(rctVals[stopBtn] == 0){
      isStopBtnPressed = true;
      sendOrder('B');//刹车
      sendOrder('B');
    }
    return;
  }
  if(isBtnReleased(stickModeBtn)){
    //摇杆模式切换按键
    sendOrder(' ');
    stickMode++;
    stickMode %= 2;
    delay(200);
  }
  if(isBtnReleased(holdSpeedBtn)){
    //保持匀速按键
    sendOrder(' ');
    speedMode++;
    speedMode %= 2;
    delay(200);
  }
  if(isBtnReleased(disSensorBtn)){
    //防撞开关
//    sensorMode++;
//    sensorMode %= 2;
    sendOrder('S');
    delay(200);
  }
  if(isBtnReleased(camResetBtn)){
    //相机复位开关
    sendOrder('F');
    delay(200);
  }

  if(stickMode == 0){
    int xValue = analogRead(axisX);
    int yValue = analogRead(axisY);
    if(speedMode==0){
      driveTracks(xValue, yValue);
    }
  }else if(stickMode == 1){
    int yValue = analogRead(axisY);
    int diff = yValue - benchmarkY;
    if(diff > allowError){
      diff = 3 * diff / (1023 - benchmarkY);
      sendOrder('U', diff + 1);
    }else if(diff < -allowError){
      diff = -3 * diff / benchmarkY;
      sendOrder('D', diff + 1);
    }
  }
  updateBtnValue();
}

void updateBtnValue(){
  for(int i = 0; i < 7; i++){
    lstVals[i] = rctVals[i];
  }
}

bool isBtnReleased(int btn){
  if(lstVals[btn] == 0 && rctVals[btn] == 1){
    return true;
  }else{
    return false;
  }
}

