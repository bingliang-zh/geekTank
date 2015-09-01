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

#define Max_deg  120
#define Min_deg  75

int servopin = 5;//定义数字接口5 连接伺服舵机信号线
int myangle;//定义角度变量
int pulsewidth;//定义脉宽变量
int val;
unsigned char Continue_flag = 0;//舵机持续转动标记
unsigned char i=0;
unsigned char temp=0;
unsigned char deg = 90;
int sensorMode = 0;//0为检测距离，1为纯手动
int sensorInputPin = 3;//超声波信号接收接口
int sensorOutputPin = 2;//超声波信号发射接口
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
  for(int i=0;i<=25;i++) {//保证转到有效角度
    servopulse(servopin,deg_in);
  }
}
void Set_deg_fast(unsigned char deg_in){
  for(int i=0;i<=2;i++) {//保证转到有效角度
    servopulse(servopin,deg_in);
  }
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
  Brake(1);
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
  //舵机持续转动
  switch(Continue_flag){
    case 0:break;//stop
    case 1:if(deg>=Min_deg) deg -= 5; Set_deg_fast(deg);break;//turn right
    case 2:if(deg<=Max_deg) deg += 5; Set_deg_fast(deg);break;//turn left
    default: break;
  }
  Serial.print("Distance: ");
  Serial.println(getDistance());
}

void Action(char ctrl_val){
  switch(ctrl_val){
    //move                           //hex  dec
    case 'L': Fast_TurnL(1);break;   //0x4C==76
    case 'R': Fast_TurnR(1);break;   //0x52==82
    case 'F': advance(1);break;      //0x46==70
    case 'B': Reverse(1);break;      //0x42==66
    case '1': break;
    case '2': break;
    case '3': break;
    case '4': break;
    case 'S': Brake(1);break;        //0x53==83
    //servo 
    case 'r': if(deg>=Min_deg) {deg -= 5; Set_deg_fast(deg);} Continue_flag = 1;break;  //0x72==114
    case 'm': deg=90;Set_deg(deg);break;                      //0x6D==109
    case 'l': if(deg<=Max_deg) {deg += 5; Set_deg_fast(deg);}Continue_flag = 2;break; //0x6C==108
    case 's': Continue_flag = 0;break;//0x73==115
    default: break;
  }   
}

 //快速右转弯
void Fast_TurnR(int a){
  digitalWrite(Right_motor_P,HIGH);  // 右电机前进
  digitalWrite(Right_motor_N,LOW);     
  analogWrite(Right_motor_P,255);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Right_motor_N,0);
  digitalWrite(Left_motor_P,HIGH);  // 左电机前进
  digitalWrite(Left_motor_N,LOW);
  analogWrite(Left_motor_P,255);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Left_motor_N,0);
  //delay(a * 100);   //执行时间，可以调整  
}

//快速左转弯
void Fast_TurnL(int g){
  digitalWrite(Right_motor_P,LOW);  //右轮后退
  digitalWrite(Right_motor_N,HIGH);
  analogWrite(Right_motor_P,0);
  analogWrite(Right_motor_N,255);//PWM比例0~255调速
  digitalWrite(Left_motor_P,LOW);  //左轮后退
  digitalWrite(Left_motor_N,HIGH);
  analogWrite(Left_motor_P,0);
  analogWrite(Left_motor_N,255);//PWM比例0~255调速
  //delay(g * 100);     //执行时间，可以调整  
}

 // 前進，走偏请调整左右PWM 
void advance(int d){
  digitalWrite(Right_motor_P,LOW);  //右轮后退
  digitalWrite(Right_motor_N,HIGH);
  analogWrite(Right_motor_P,0);
  analogWrite(Right_motor_N,255);//PWM比例0~255调速
  digitalWrite(Left_motor_P,HIGH);//左轮前进
  digitalWrite(Left_motor_N,LOW);  
  analogWrite(Left_motor_P,255);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Left_motor_N,0);
  //delay(d * 10);//执行时间，可以调整  
}
//後退
void Reverse(int e){
  digitalWrite(Right_motor_P,HIGH);//右轮前进
  digitalWrite(Right_motor_N,LOW);   
  analogWrite(Right_motor_P,255);//PWM比例0~255调速，左右轮差异略增减
  analogWrite(Right_motor_N,0);
  digitalWrite(Left_motor_P,LOW);   //左轮后退
  digitalWrite(Left_motor_N,HIGH);
  analogWrite(Left_motor_P,0);
  analogWrite(Left_motor_N,255);//PWM比例0~255调速
  //delay(e * 10);//执行时间，可以调整  
}    
//刹车，停车
void Brake(int f){
  digitalWrite(Right_motor_P,LOW);
  digitalWrite(Right_motor_N,LOW);
  digitalWrite(Left_motor_P,LOW);
  digitalWrite(Left_motor_N,LOW);
  //delay(f * 100);//执行时间，可以调整  
}  

String str;

void Judge(void){
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
      case 'B': Brake(1);break;                         //履带刹车 break
      case 'F': deg = 90;Set_deg(deg);break;            //相机复位 focus
      case 'D':                                         //相机低头 down
        if(deg>=Min_deg){
          deg -= 2;
          Set_deg_fast(deg);
        }Continue_flag = 0;break;
      case 'U':                                         //相机抬头 up
        if(deg<=Max_deg){
          deg += 2;
          Set_deg_fast(deg);
        }Continue_flag = 0;break;
      case 'S': sensorMode++;sensorMode%=2;break;       //切换距离感应开关 sensor
      default: break;
    }
  }
}

void driveTracks(int l, int r){
  Serial.print(l);
  Serial.print("     ");
  Serial.println(r);
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
  double distance = pulseIn(sensorInputPin, HIGH); // 读出脉冲时间
  distance= distance/58.0; // 将脉冲时间转化为距离（单位：厘米）
//  Serial.println(distance); //输出距离值
  return distance;
}

