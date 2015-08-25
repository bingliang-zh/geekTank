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
int lstVals[7] = {1,1,1,1,1,1,1};
//Recent Value
int rctVals[7];

//模式开关
bool isStopBtnPressed = false;
int stickMode   = 0;//0为控制履带，1为控制相机
int speedMode   = 0;//0为变速，1为匀速
int sensorMode  = 0;//0为检测距离，1为纯手动

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

void loop(){
  for(int i = 0; i < 7; i++){
    rctVals[i] = digitalRead(i);
  }
  if(rctVals[stopBtn] == 0||isStopBtnPressed == true){
    //按下停止按钮后改变判断值，进入死循环，只有重启可以解除该状态
    isStopBtnPressed = true;
    driveTracks(0, 0);//关履带动力
    return;
  }
  if(isBtnReleased(stickModeBtn)){
    //摇杆模式切换按键
  }
  if(isBtnReleased(holdSpeedBtn)){
    //保持匀速按键
  }
  if(isBtnReleased(disSensorBtn)){
    //防撞开关
  }
  if(isBtnReleased(camResetBtn)){
    //相机复位开关
  }
  
  int xValue = analogRead(axisX);
  int yValue = analogRead(axisY);
  driveTracks(xValue, yValue);

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

