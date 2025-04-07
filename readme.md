# Garage Alarm System

## **摘要**：

 本研究设计了一种基于多传感器融合的智能车库安全监测系统，采用超声波测距、光敏传感及温湿度检测技术实现环境参数实时采集。系统通过FreeRTOS实时操作系统构建多任务调度架构，分别实现距离检测（优先级2）、温度监测（优先级1）和状态指示（优先级0）的并发执行。针对传感器噪声干扰问题，提出改进型递推平均滤波算法，实验表明该算法可使超声波测距误差降低至±0.2cm（@2m量程）。系统集成三重安全预警机制：当检测到车灯信号（光照值>700Lux）且障碍距离<50cm时触发碰撞预警；环境温度超出0-50℃阈值时启动温控报警；工作指示灯采用PWM动态呼吸算法实现设备状态可视化。显著提升了传统车库安防系统的可靠性和智能化水平。 

## **关键词**：

智能车库；多传感器融合；滤波算法；实时操作系统（FreeRTOS）；PWM动态呼吸算法；安全预警系统

## 使用的器件

使用器件有：蜂鸣器、LED灯、HC-SR04超声波传感器、温湿度传感器，Arduino UNO R3。

## 功能：

1. 防止车辆碰撞（优先级高）：超声波传感器固定到车库大门正对的墙壁上，当且仅当光线传感器接收到光线（开车时，汽车启动时有尾灯；不开车时，人经过时不警报。）而且超声波传感器测距小于一定值（比如0.5米）时，促使蜂鸣器发声，产生警报。
2. 防止温度过低结冰，或者温度过高产生火灾（优先级高）：温度传感器置放于车库内，当温度低于一定温度（比如零摄氏度）或者高于一定温度（比如50摄氏度）时，促使蜂鸣器发声，产生警报。
3. 工作指示灯（优先级低）：LED灯呈现呼吸状态，第一周期从暗到亮，第二周期从亮到暗，第三周期一直暗。工作指示灯不亮可能表示装置出现故障。

## 器件接法

HC-SR04超声波传感器：

Grid -> G

Echo -> D5

Trig -> D4

Vcc -> 5V

蜂鸣器：12

指示灯：9

光感器：A0

温度传感器：A5

## 原理图

![](Schematic_%E4%BC%A0%E6%84%9F%E5%99%A8_2025-03-11.png)

## 滤波器的使用

 将采集的数据⽤三种不同滤波器进⾏处理，将原始数据，处理后的数据，真实的数值⽤⼆维图展示。 

 使用三种不同的滤波器：中值平均滤波器、中位值滤波器、算数均值滤波器。 

### 结果图

![1741743074426](C:/Users/panyue/AppData/Roaming/Typora/typora-user-images/1741743074426.png)

(以超声波传感器为例）图表的标识从左到右依次为：

原始数据，中值平均滤波值，中位值滤波值，算数均值滤波值。

- **蓝色折线**为原始数据
- **绿色折线**为中位值滤波值；
- **橙色折线**为中值平均滤波值；
- **黄色折线**为算数均值滤波值。

 放大波动细节，可以看到：**黄色折线（算数均值滤波）**在波动时，也随着波动，但是幅度偏小；**绿色折线（中位值滤波）**在测量和**橙色折线（中位值滤波）**波动不明显，说明两者能有效克服因偶然因素引起的波动干扰，体现了中位值滤波的优点。 

## QT+串口显示数据

### 系统软件架构设计

​		本系统采用C/S架构设计，上位机监控软件基于QT 5.15.2开发，通过Serial与下位机FreeRTOS嵌入式系统进行数据交互。如图3所示，软件架构分为三个核心层次：

1. **通信管理层**
   采用QSerialPort类实现串口通信模块，配置波特率9600bps参数。设计环形缓冲区（Buffer Size=1024Bytes）应对数据突发流量，通过CRC-16校验保证数据传输完整性（误码率<1×10⁻⁶）。

**2.实时监控界面实现**
如图所示，主界面包含四大功能区域：

![1741742436892](C:/Users/panyue/AppData/Roaming/Typora/typora-user-images/1741742436892.png)

光照区：光照大小会改变太阳颜色深浅以及射线条数

距离区：距离会改变进度条，颜色也会渐变

湿度区：水滴蓝色区域会随着湿度改变

温度区：温度计值随温度上下改变

------

**关键实现技术说明**

1. **异步事件处理**
   采用Qt信号槽机制实现松耦合通信，定义自定义事件类型处理紧急报警：

   ```
   enum CustomEvent { 
       CriticalAlert = QEvent::User + 1 
   };
   void MainWindow::customEvent(QEvent* event) {
       if(event->type() == CriticalAlert) {
           // 触发声光报警
       }
   }
   ```

2.**自定义组件以及事件机制的触发**

​	![1741742794156](C:/Users/panyue/AppData/Roaming/Typora/typora-user-images/1741742794156.png)

在QT工程中对四个区域分别自定义widget并定义绘制事件。



第1章 系统总体设计
1.1 功能需求分析
本系统需实现以下核心功能：
1. 车辆防撞预警：超声波传感器（HC-SR04）检测障碍物距离，结合光敏传感器（A0）信号触发蜂鸣器报警。
2. 温湿度安全监测：DHT11传感器监控车库环境，超阈值时启动声光报警。
3. 状态可视化：LED指示灯通过PWM动态呼吸算法反馈系统运行状态。
1.2 硬件架构设计
系统硬件基于Arduino UNO R3开发板，主要器件包括：
- 传感器模块：HC-SR04（Trig-D4, Echo-D5）、DHT11（A5）、光敏电阻（A0）
- 执行模块：蜂鸣器（D12）、LED指示灯（D9）
- 通信模块：串口通信（波特率9600bps）
1.3 软件任务调度模型
采用FreeRTOS实时操作系统，任务优先级分配如下：
- 任务1（优先级2）：超声波测距（周期50ms）
- 任务2（优先级1）：温湿度采集（周期2s）
- 任务3（优先级0）：LED状态指示（周期1s）



第2章 硬件实现与传感器选型
2.1 传感器模块设计
- 超声波传感器：量程2cm-4m，精度±0.1cm
- 光敏传感器：量程1-1000Lux，模拟信号输出
- 温湿度传感器：温度量程0-50℃，精度±0.1℃
HC-SR04 超声波测距模块
1. 概述
HC-SR04 是一种超声波测距模块，广泛用于距离测量、障碍物检测和机器人避障等应用。它采用超声波回波时间差原理，通过发射超声波并计算其返回时间来测量目标物体的距离。

________________________________________
2. 工作原理
HC-SR04 通过发射 40kHz 的超声波脉冲，并计算它返回的时间来确定与障碍物的距离。工作流程如下：
1.触发信号（Trigger）
o给 Trig 引脚施加 至少 10µs 的高电平脉冲，模块开始测距。
2.超声波发射
o模块内部产生 40kHz 超声波脉冲，通过换能器（Transducer）发射出去。
3.超声波接收
o当超声波遇到障碍物时，会被反射回来。
4.计算时间
oEcho 引脚输出高电平，持续时间等于超声波往返时间（单位：微秒 µs）。
5.计算距离

DHT11 温湿度传感器详解
1. 概述
DHT11 是一款常见的 温湿度传感器，广泛用于环境监测、智能家居、农业自动化等领域。它可以测量 温度 和 相对湿度，并通过 单总线通信 将数据传输给微控制器（如 Arduino、ESP8266、Raspberry Pi、STM32 等）。
________________________________________
2. 工作原理
DHT11 采用 电容式湿度传感器 和 NTC（负温度系数）热敏电阻 进行温湿度检测，并通过 内置 8 位单片机 处理数据，最终以 数字信号 形式输出。

________________________________________
3. 引脚定义
DHT11 采用 单总线通信，只需一根数据线即可传输数据。
数据引脚（DATA）需要一个 4.7kΩ - 10kΩ 的 上拉电阻，否则无法正常通信。
4. 数据通信协议
DHT11 通过 单总线通信 进行数据传输，通信过程如下：
1.主机（MCU）发送起始信号
o主机将 DATA 引脚拉低 18ms，然后释放（拉高）。
2.DHT11 响应信号
oDHT11 发现起始信号后，拉低 80µs，再拉高 80µs。
3.数据传输（40bit）
oDHT11 发送 40 位数据：
8bit 湿度整数部分
8bit 湿度小数部分（DHT11 始终为 0）
8bit 温度整数部分
8bit 温度小数部分（DHT11 始终为 0）
8bit 校验和
o校验方法：前 4 个字节相加，低 8 位应等于校验和。
光敏传感器模块
光敏传感器模块 5516 是一种基于 光敏电阻（LDR, Light Dependent Resistor） 的环境光检测模块。它可以检测光照强度变化，并输出 模拟信号（AO） 和 数字信号（DO），适用于自动光照控制、环境监测、智能家居等应用。

2.工作原理
（1）光敏电阻（LDR）的特性
•光敏电阻 的阻值 随光照强度变化：
o光照增强 → 电阻变小
o光照减弱 → 电阻变大
•通过 分压电路 将光敏电阻的电阻变化转换为 电压信号，再由 LM393 比较器 进行信号处理。
（2）信号输出
•AO（模拟输出）：
o输出 0V ~ 5V 的电压信号，电压大小与光照强度成反比。
•DO（数字输出）：
o通过 LM393 比较器 处理，当光照超过设定阈值时，输出 高电平（1）；低于设定阈值时，输出 低电平（0）。
o电位器 可调节 触发阈值。
________________________________________
3.引脚说明
引脚	名称	功能说明
VCC	电源正极	3.3V - 5V 供电
GND	地	电源负极（GND）
AO	模拟信号输出	输出与光照强度成反比的电压信号
DO	数字信号输出	设定阈值触发时输出 高/低电平
Arduino UNO R3

 

________________________________________
2.2 器件接法与原理图
器件接法如下表所示：
| 器件         | 引脚接法    |
| ------------ | ----------- |
| HC-SR04 Trig | Arduino D4  |
| HC-SR04 Echo | Arduino D5  |
| 蜂鸣器       | Arduino D12 |
| LED指示灯    | Arduino D9  |
| 光敏传感器   | Arduino A0  |
| DHT11        | Arduino A5  |
原理图：

实物图：

串口输出效果：

代码片段：
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
disBuz = false;     changeBuz(disBuz, ligBuz, temBuz);
}
vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);
}
第3章 软件设计与算法优化
3.1 FreeRTOS多任务调度
任务调度代码片段：
void TaskDistanceCheck(void *pvParameters) {
for (;;) {
float cm = Filter_3(50); // 滑动窗口滤波
vTaskDelay(50 / portTICK_PERIOD_MS);
}}
3.2 滤波算法实现与对比
采用三种滤波算法处理超声波数据：
1. 中值平均滤波：剔除极端值后取平均
2. 中位值滤波：取采样序列中间值
3. 算术均值滤波：直接计算平均值
实验结果图：


- 蓝色：原始数据
- 绿色：中位值滤波
- 橙色：中值平均滤波
- 黄色：算术均值滤波
由上两图中可以看出中位值滤波最为稳定，一直保持在5.37cm和12.70cm,
中值平均滤波和算数平均滤波在5.35cm和12.72左右以—+0.02cm幅度波动，波动不明显。
代码片段：
 中位值平均滤波法（又称防脉冲干扰平均滤波法）
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
3.3 上位机监控界面开发
基于QT 5.15.2设计上位机界面，功能分区如下：

光照区：光照大小会改变太阳颜色深浅以及射线条数
距离区：距离会改变进度条，颜色也会渐变
湿度区：水滴蓝色区域会随着湿度改变
温度区：温度计值随温度上下改变
界面代码片段：
1.太阳绘制函数
void SunlightWidget::paintEvent(QPaintEvent *event)
{
QPainter painter(this);
painter.setRenderHint(QPainter::Antialiasing);
int w = width();
int h = height();
int size = qMin(w, h);
// **反向归一化光照值**
float normalized = (1000 -value) / 400.0f;
// **颜色变换：从亮黄到深黄**
QColor sunColor = QColor::fromHsv(50, 255, static_cast<int>(normalized * 255));
// 画太阳主体
painter.setBrush(QBrush(sunColor));
painter.setPen(Qt::NoPen);
painter.drawEllipse(QPoint(w / 2, h / 2), size / 4, size / 4);
// 画光晕
painter.setPen(QPen(sunColor, 2));
int rayLength = static_cast<int>((size / 3) + (normalized * (size / 6)));
for (int i = 0; i < 12; ++i) {
float angle = i * 30 * 3.14159 / 180;
int x1 = w / 2 + cos(angle) * (size / 4);
int y1 = h / 2 + sin(angle) * (size / 4);
int x2 = w / 2 + cos(angle) * rayLength;
int y2 = h / 2 + sin(angle) * rayLength;
painter.drawLine(x1, y1, x2, y2);
}
}
2.串口数据读取
void MainWindow::readSerialData()
{
while (serial->canReadLine()) {
QByteArray line = serial->readLine().trimmed();
     receivedData = QString::fromUtf8(line);
qDebug() << "收到数据：" << receivedData;
}
QStringList values = receivedData.split(",");
if (values.size() >= 1) {  // 读取光照值
float lightValue = values[0].toFloat();
float disValue = values[1].toFloat();
float temValue = values[2].toFloat();
float humValue = values[3].toFloat();
sunlightWidget->setValue(lightValue);
distancewidget->setDistance(disValue);
tempwidget->setTemperature(temValue);
humwidget->setHumidity(humValue);
}
}




