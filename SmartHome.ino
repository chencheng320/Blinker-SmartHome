#define BLINKER_WIFI
#define BLINKER_PRINT  Serial

#include <Blinker.h>
#include <DHT.h>                 //DHT11库
#include <Adafruit_NeoPixel.h>   //全彩LED控制库

#ifdef __AVR__
   #include <avr/power.h>
#endif

char auth[] = "3f978653f94c";   //Secret Key
char ssid[] = "chengzhongshuo"; //WiFi热点名称
char pswd[] = "czs18715384816"; //WiFi密码

#define LED1           5       //LED引脚D1
#define LED2           4       //LED引脚D2
#define PIN            13       //RGB引脚D7
#define NUMPIXELS      60       //RGB灯珠数量
#define DHTPIN 2                //接的是GPIO2端口   D4引脚
#define DHTTYPE DHT11           // DHT 11
#define RGB_1 "RGBKey"          //设置RGB组件数据键名为RGBKey

BlinkerNumber HUMI("humi");        //设置湿度组件名为humi
BlinkerNumber TEMP("temp");        //设置温度组件名为temp
BlinkerButton Button1("Button1");//设置LED1灯开关组件名为Button1
BlinkerButton Button2("Button2");//设置LED2灯开关组件名为Button2
BlinkerNumber Number1("num1");
BlinkerNumber Number2("num2");
BlinkerRGB WS2812(RGB_1);          //设置RGB控制组件



Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  //定义一个Adafruit_NeoPixel对象

DHT dht(DHTPIN, DHTTYPE);  //初始设置，生成DHT对象，参数是引脚和DHT类型

float humi_read = 0, temp_read = 0;  //记录每次心跳包反馈的温湿度数据

//心跳包
void heartbeat()
{
  HUMI.print(humi_read);  //监视器打印温度数据
  TEMP.print(temp_read);  //监视器打印湿度数据
}

//云端存储温湿度数据
void dataStorage()
{
    Blinker.dataStorage("temp", temp_read);
    Blinker.dataStorage("humi", humi_read);
}

//语音控制LED1灯开关
void button1_callback_1(const String & state)
{
  if (state == BLINKER_CMD_ON) {    //给开灯指令
    digitalWrite(LED1, HIGH);         //引脚给高电平
    BLINKER_LOG("Toggle on!");
    Button1.print("on");            //打印状态信息
  }
  else if (state == BLINKER_CMD_OFF) {   //给关灯指令
    digitalWrite(LED1, LOW);               //引脚给低电平
    BLINKER_LOG("Toggle off!");       
    Button1.print("off");                //打印状态信息
  }
}

//手动控制LED1开关，按下按键触发函数执行
void button1_callback_2(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  digitalWrite(LED1, !digitalRead(LED1));
}

//语音控制LED2灯开关
void button2_callback_1(const String & state)
{
  if (state == BLINKER_CMD_ON) {    //给开灯指令
    digitalWrite(LED2, HIGH);         //引脚给高电平
    BLINKER_LOG("Toggle on!");
    Button2.print("on");            //打印状态信息
  }
  else if (state == BLINKER_CMD_OFF) {   //给关灯指令
    digitalWrite(LED2, LOW);               //引脚给低电平
    BLINKER_LOG("Toggle off!");       
    Button2.print("off");                //打印状态信息
  }
}

//手动控制LED2开关，按下按键触发函数执行
void button2_callback_2(const String & state)
{
  BLINKER_LOG("get button state: ", state);
  digitalWrite(LED2, !digitalRead(LED2));
}

//氛围灯控制
void ws2812_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG("R value: ", r_value);
  BLINKER_LOG("G value: ", g_value);
  BLINKER_LOG("B value: ", b_value);
  BLINKER_LOG("Rrightness value: ", bright_value);

  pixels.setBrightness(bright_value);

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, r_value, g_value, b_value);
  }
  pixels.show();
}

void setup()
{
  Serial.begin(115200);
  dht.begin();                      //温湿度传感器初始化
  pixels.begin();                   //RGB全彩灯初始化
  BLINKER_DEBUG.stream(Serial);     //指定Debug信息输出的串口号
  BLINKER_DEBUG.debugAll();         //显示更多的调试信息

  pinMode(LED1, OUTPUT);              //配置D1引脚为输出模式
  digitalWrite(LED1, LOW);            //初始化D1引脚为低电平，即灯熄灭
  pinMode(LED2, OUTPUT);              //配置D2引脚为输出模式
  digitalWrite(LED2, LOW);            //初始化D2引脚为低电平，即灯熄灭

  Blinker.begin(auth, ssid, pswd);  //设备联网
  Blinker.attachHeartbeat(heartbeat);
  Blinker.attachDataStorage(dataStorage);
  Button1.attach(button1_callback_1);   //Button1绑定语音功能函数
  Button1.attach(button1_callback_2);  //Button1绑定手动功能函数
  Button2.attach(button2_callback_1);   //Button2绑定语音功能函数
  Button2.attach(button2_callback_2);  //Button2绑定手动功能函数
  WS2812.attach(ws2812_callback);       //氛围灯组件绑定函数
}

void loop()
{
  Blinker.run();
  float h = dht.readHumidity();       //获取湿度信息
  float t = dht.readTemperature();    //获取温度信息
  if (isnan(h) || isnan(t))           //检查获取的温湿度参数是否是非数字型
  {
    BLINKER_LOG("Failed to read from DHT sensor!");
  }
  else
  {
    BLINKER_LOG("Humidity: ", h, " %");     //BLINKER_LOG打印输出调试信息
    BLINKER_LOG("Temperature: ", t, "*C");
    humi_read = h;                          //反馈温湿度信息
    temp_read = t;
  }
  Blinker.delay(1500);
}
