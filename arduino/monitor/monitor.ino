// blStudio修改 使用ocrobot honeybee作遥控接收端和小车控制板
// 无线控制使用zigbee协议

// POWSOS WIFI TANK程序
//--------------POWSOS出品--------------------
//2015-4-18 V2.0
//标注“PWM比例0~255调速”，可以按比例调整转速，因电机产品有差异，转速略有差异，直行会跑偏，可略增减
//此处舵机使用模拟方式控制实现，电机控制与舵机不能同时用库函数
//不同地面轮胎抓地力差异较大，可以适当调整转速

//================下载程序务必断开路由器插头=======

//#include <Servo.h> 
int Left_motor_P=9;     //左电机正极
int Left_motor_N=6;     //左电机负极

int Right_motor_P=10;    // 右电机正极
int Right_motor_N=11;    // 右电机负极

int Light_Pin = 3;           // 点灯PWM正极

#define Max_deg  120
#define Min_deg  75
#define Step_deg 2

int servopin = 5;//定义数字接口5 连接伺服舵机信号线
int myangle;//定义角度变量
int pulsewidth;//定义脉宽变量
int val;
unsigned char i=0;
unsigned char temp=0;
unsigned char deg = 90;
int brightness = 2;
int sensorMode = 1;//0为检测距离，1为纯手动
int sensorInputPin = 13;//超声波信号接收接口
int sensorOutputPin = 12;//超声波信号发射接口
//履带传输值
int lTrans;
int rTrans;

void servopulse(int servopin,int myangle){//定义一个脉冲函数
  pulsewidth=(myangle*11)+500;//将角度转化为500-2480 的脉宽值
  digitalWrite(servopin,HIGH);//将舵机接口电平至高
  delayMicroseconds(pulsewidth);//延时脉宽值的微秒数
  digitalWrite(servopin,LOW);//将舵机接口电平至低
  delay(20-pulsewidth/1000);
}

void Set_deg(unsigned char deg_in){
  for(int i=0;i<=5;i++) {//保证转到有效角度
    servopulse(servopin,deg_in);
  }
}
void Set_deg_fast(unsigned char deg_in){
  servopulse(servopin,deg_in);
}

void setup(){
  Serial.begin(9600);     // 初始化串口
  Serial.println("Setup().");

  //初始化电机驱动IO为输出方式
  pinMode(Left_motor_P,OUTPUT);   // 腳位 8 (PWM)
  pinMode(Left_motor_N,OUTPUT);   // 腳位 9 (PWM)
  pinMode(Right_motor_P,OUTPUT);  // 腳位 10 (PWM) 
  pinMode(Right_motor_N,OUTPUT);  // 腳位 11 (PWM)

  pinMode(servopin,OUTPUT);//设定舵机接口为输出接口
  //digitalWrite(servopin,LOW);   //拉低舵机控制脚
  pinMode(13, OUTPUT);

  pinMode(sensorInputPin, INPUT);
  pinMode(sensorOutputPin, OUTPUT);
  
  Set_deg(deg);
  analogWrite(Light_Pin, brightness);
  Brake();
  for(i=0;i<4 ;i++)
  {
    digitalWrite(13, LOW);
    delay(500);
    digitalWrite(13, HIGH);
    delay(500);        
  }
}

//主函数  
char lastInput=0,currentInput=0;
void loop(){
  Judge();
}

//刹车，停车
void Brake(){
  digitalWrite(Right_motor_P,LOW);
  digitalWrite(Right_motor_N,LOW);
  digitalWrite(Left_motor_P,LOW);
  digitalWrite(Left_motor_N,LOW);
}

void Judge(void){
  String str;
  if(Serial.available() > 0){
    str = Serial.readStringUntil('\n');
    Serial.println(str);
  }
  if(str.length() >= 4 && str.substring(0, 3) == "CMD"){
    char cmd = str[3];
    switch(cmd){
      case 'T':
        if(str.length()==6){
          driveTracks(int(str[4]),int(str[5]));
        }break;
      case 'B': Brake();break;                         //履带刹车 break
      case 'F': deg = 90;Set_deg(deg);break;            //相机复位 focus
      case 'D':                                         //相机低头 down
        if(str.length()==5 && deg>=Min_deg){
          deg -= str[4] - '0';
          Set_deg_fast(deg);
        }
        break;
      case 'U':                                         //相机抬头 up
        if(str.length()==5 && deg<=Max_deg){
          deg += str[4] - '0';
          Set_deg_fast(deg);
        }
      case 'M':                                         //调亮
        if(str.length()==5 && brightness < 255){
          brightness += str[4] - '0';
          if (brightness > 255){
            brightness = 255;
          }
          analogWrite(Light_Pin, brightness);
          Serial.println(brightness);
        }
        break;
      case 'N':                                         //暗
        if(str.length()==5 && brightness > 0){
          brightness -= str[4] - '0';
          if (brightness < 0){
            brightness = 0;
          }
          analogWrite(Light_Pin, brightness);
          Serial.println(brightness);
        }
        break;
      case 'S': sensorMode++;sensorMode%=2;break;       //切换距离感应开关 sensor
      case ' ': break;                                  //只亮灯，什么都不做
      default: break;
    }
  }
}

void driveTracks(int l, int r){
  if(sensorMode == 0){
    double dis = getDistance();
    if(dis < 20.0 && dis > 0 ){
      Brake();
      return;
    }
  }
  if(l > 0){
    digitalWrite(Left_motor_P,HIGH);//左轮前进
    digitalWrite(Left_motor_N,LOW);
    analogWrite(Left_motor_P,2 * l);//PWM比例0~255调速，左右轮差异略增减
    analogWrite(Left_motor_N,0);
  }else if(l < 0){
    digitalWrite(Left_motor_P,LOW);//左轮后退
    digitalWrite(Left_motor_N,HIGH);
    analogWrite(Left_motor_P,0);//PWM比例0~255调速，左右轮差异略增减
    analogWrite(Left_motor_N,-2 * l);
  }else{
    digitalWrite(Left_motor_P,LOW);
    digitalWrite(Left_motor_N,LOW);
  }
  if(r > 0){
    digitalWrite(Right_motor_P,LOW);  //右轮后退
    digitalWrite(Right_motor_N,HIGH);
    analogWrite(Right_motor_P,0);
    analogWrite(Right_motor_N,2 * r);//PWM比例0~255调速
  }else if(r < 0){
    digitalWrite(Right_motor_P,HIGH);  //右轮后退
    digitalWrite(Right_motor_N,LOW);
    analogWrite(Right_motor_P,-2 * r);
    analogWrite(Right_motor_N,0);//PWM比例0~255调速
  }else{
    digitalWrite(Right_motor_P,LOW);
    digitalWrite(Right_motor_N,LOW);
  }
}

double getDistance(){
  digitalWrite(sensorOutputPin, LOW); // 使发出发出超声波信号接口低电平2μs
  delayMicroseconds(2);
  digitalWrite(sensorOutputPin, HIGH); // 使发出发出超声波信号接口高电平10μs，这里是至少10μs
  delayMicroseconds(10);
  digitalWrite(sensorOutputPin, LOW); // 保持发出超声波信号接口低电平
  double distance = pulseIn(sensorInputPin, HIGH); // 读出脉冲时间 60ms约为10m
  distance= distance/58.0; // 将脉冲时间转化为距离（单位：厘米）
  Serial.print("Distance(cm): ");
  Serial.println(distance); //输出距离值
  return distance;
}

