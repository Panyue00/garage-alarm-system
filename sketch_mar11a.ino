#include <Arduino_FreeRTOS.h>
#include <dht11.h>

// 声明实时任务函数
void TaskDistanceCheck( void *pvParameters );     // 距离检测任务
void TaskTemperatureCheck( void *pvParameters );  // 温度检测任务
void TaskBlink( void *pvParameters );             // 工作指示灯任务

#define BUZZER_PIN 6   // 蜂鸣器引脚
#define TRIG_PIN 4       // 超声波Trig引脚
#define ECHO_PIN 5       // 超声波Echo引脚
#define LIGHT_PIN A0     // 光传感器引脚
#define TEM_PIN A5       // 温度传感器引脚
#define WORK_LED_PIN 3   // 指示灯引脚

#define DELAY_TIME 100   // 检测时间间隔(ms)

#define MIN_DISTANCE 10   // 最小警报距离(cm)
#define MIN_LIGHT 500     // 警报亮度
#define MAX_TEM 25        // 警报温度(℃)
#define MAX_LIGHT 255     // 指示灯最大亮度
#define CHANGE_LIGHT 25   // 指示灯亮度变化幅度


dht11 DHT11;

bool disBuz = false; // 距离蜂鸣器状态变量
bool ligBuz = false; // 光感蜂鸣器状态变量
bool temBuz = false; // 温度蜂鸣器状态变量
float cm=0;  
int light=0;                 //距离变量               //存储回波等待时间
// 递推平均滤波法（又称滑动平均滤波法）

// 初始化函数
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);    // 初始化蜂鸣器引脚为输出
  digitalWrite(BUZZER_PIN, LOW); // 初始化蜂鸣器为高电平，不响
  pinMode(TRIG_PIN, OUTPUT);      // 设置Trig引脚为输出
  pinMode(ECHO_PIN, INPUT);       // 设置Echo引脚为输入
  pinMode(WORK_LED_PIN, OUTPUT);  // 设置LED引脚为输出

  Serial.begin(9600); // 初始化串口为9600
  while (!Serial) {
    ; // 等待串口连接
  }

  // 设置独立任务
  // 距离检测任务
  xTaskCreate(
    TaskDistanceCheck
    ,  (const portCHAR *) "DistanceCheck"
    ,  128  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  NULL );
  // 温度检测任务
  xTaskCreate(
    TaskTemperatureCheck
    ,  (const portCHAR *) "TemperatureCheck"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority
    ,  NULL );
  // 工作指示灯任务
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *)"Blink"
    ,  128  // Stack size
    ,  NULL
    ,  0   // Priority
    ,  NULL );

  // 任务计划器自动启动
}

void loop()
{
  // Empty. Things are done in Tasks.
}
/*
  根据状态值改变蜂鸣器的输出状态
*/
void changeBuz(bool disBuz, bool ligBuz, bool temBuz) {
  analogWrite(BUZZER_PIN, (disBuz || ligBuz || temBuz) ? 40 : 0);
}
/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/
/*
  防止车辆碰撞（优先级高）：
  超声波传感器固定到车库大门正对的墙壁上，
  当且仅当光线传感器接收到光线（开车时，汽车启动时有尾灯；不开车时，人经过时不警报。）
  而且超声波传感器测距小于一定值（比如0.5米）时，促使蜂鸣器发声，产生警报。
*/
void TaskDistanceCheck(void *pvParameters)
{
  (void) pvParameters;
  float temp;
  for (;;)
  {
    //给Trig发送一个低高低的短时间脉冲,触发测距
    digitalWrite(TRIG_PIN, LOW); //给Trig发送一个低电平
    delayMicroseconds(2);        //等待 2微妙
    digitalWrite(TRIG_PIN, HIGH); //给Trig发送一个高电平
    delayMicroseconds(10);        //等待 10微妙
    digitalWrite(TRIG_PIN, LOW); //给Trig发送一个低电平
    temp = float(pulseIn(ECHO_PIN, HIGH)); //存储回波等待时间
    cm = (temp * 17 ) / 1000;              //把回波时间换算成cm
    
    if (cm <= MIN_DISTANCE) {
      disBuz = true;
      changeBuz(disBuz, ligBuz, temBuz);
    } else {
      disBuz = false;
      changeBuz(disBuz, ligBuz, temBuz);
    }
   
    
    
    vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
  }
}
/*
  防止温度过低结冰，或者温度过高产生火灾（优先级高）：
  温度传感器置放于车库内，当温度低于一定温度（比如零摄氏度）
  或者高于一定温度（比如50摄氏度）时，促使蜂鸣器发声，产生警报。
*/
void TaskTemperatureCheck(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    int chk = DHT11.read(TEM_PIN);
    float temp = (float)DHT11.temperature;
    float hum = (float)DHT11.humidity;
     int light = analogRead(LIGHT_PIN); // read the input on analog pin 0:
    cm = Filter_3(5);
    Serial.print(light);
    Serial.print(",");
    Serial.print(cm);//串口输出距离换算成cm的结果
    Serial.print(",");
    Serial.print(temp, 2);
    Serial.print(",");
    Serial.println(hum, 2);
    if (temp >= MAX_TEM) {
      temBuz = true;
      changeBuz(disBuz, ligBuz, temBuz);
    } else {
      temBuz = false;
      changeBuz(disBuz, ligBuz, temBuz);
    }
    if (light <= MIN_LIGHT) {
      ligBuz = true;
      changeBuz(disBuz, ligBuz, temBuz);
    } else {
      ligBuz = false;
      changeBuz(disBuz, ligBuz, temBuz);
    }
    vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
  }
}
/*
  工作指示灯（优先级低）：
  LED灯呈现呼吸状态，第一周期从暗到亮，第二周期从亮到暗，第三周期一直暗。
  工作指示灯不亮可能表示装置出现故障。
*/
void TaskBlink(void *pvParameters)
{
  (void) pvParameters;
  for (;;)
  {
    // 第一个周期，LED由暗(0)到亮(MAX_LIGHT)
    for (int i = 0; i <= MAX_LIGHT; i += CHANGE_LIGHT) {
      analogWrite(WORK_LED_PIN, i); // 虚拟写入亮度
      vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);       // 保持亮度
    }
    // 第二个周期，LED由亮(MAX_LIGHT)到暗(0)
    for (int i = MAX_LIGHT; i >= 0; i -= CHANGE_LIGHT) {
      analogWrite(WORK_LED_PIN, i);
      vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
    }
    // 第三个周期，LED始终保持暗。
    analogWrite(WORK_LED_PIN, 0);
    vTaskDelay(DELAY_TIME * (MAX_LIGHT / CHANGE_LIGHT) / portTICK_PERIOD_MS);
  }
}


// 获取当前的感应值
float Get_Value() {
  float cm;                   //距离变量
  float temp;                 //存储回波等待时间
  //给Trig发送一个低高低的短时间脉冲,触发测距
  digitalWrite(TRIG_PIN, LOW); //给Trig发送一个低电平
  delayMicroseconds(2);        //等待 2微妙
  digitalWrite(TRIG_PIN, HIGH); //给Trig发送一个高电平
  delayMicroseconds(10);        //等待 10微妙
  digitalWrite(TRIG_PIN, LOW); //给Trig发送一个低电平
  temp = float(pulseIn(ECHO_PIN, HIGH)); //存储回波等待时间
  cm = (temp * 17 ) / 1000;              //把回波时间换算成cm
  return cm;
  //  return analogRead(LIGHT_PIN);
}
/**
   中位值平均滤波法（又称防脉冲干扰平均滤波法）
*/
float Filter_1(int FILTER_N) {
  int i, j;
  float filter_temp, filter_sum = 0;
  float filter_buf[FILTER_N];
  for (i = 0; i < FILTER_N; i++) {
    filter_buf[i] = Get_Value();
    delay(10);
  }
  // 采样值从小到大排列（冒泡法）
  for (j = 0; j < FILTER_N - 1; j++) {
    for (i = 0; i < FILTER_N - 1 - j; i++) {
      if (filter_buf[i] > filter_buf[i + 1]) {
        filter_temp = filter_buf[i];
        filter_buf[i] = filter_buf[i + 1];
        filter_buf[i + 1] = filter_temp;
      }
    }
  }
  // 去除最大最小极值后求平均
  for (i = 1; i < FILTER_N - 1; i++) filter_sum += filter_buf[i];
  return filter_sum / (FILTER_N - 2);
}
/**
   中位值滤波法
*/
float Filter_2(int FILTER_N) {
  float filter_buf[FILTER_N];
  int i, j;
  float filter_temp;
  for (i = 0; i < FILTER_N; i++) {
    filter_buf[i] = Get_Value();
    delay(10);
  }
  // 采样值从小到大排列（冒泡法）
  for (j = 0; j < FILTER_N - 1; j++) {
    for (i = 0; i < FILTER_N - 1 - j; i++) {
      if (filter_buf[i] > filter_buf[i + 1]) {
        filter_temp = filter_buf[i];
        filter_buf[i] = filter_buf[i + 1];
        filter_buf[i + 1] = filter_temp;
      }
    }
  }
  return filter_buf[(FILTER_N - 1) / 2];
}
/**
   算术平均滤波法
*/
float Filter_3(int FILTER_N) {
  int i;
  float filter_sum = 0;
  for (i = 0; i < FILTER_N; i++) {
    filter_sum += Get_Value();
    delay(10); 
  }
  return (filter_sum / FILTER_N);
}
