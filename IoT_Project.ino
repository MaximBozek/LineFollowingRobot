// - 𝕸𝖆𝖝𝖎𝖒 𝕭𝖔𝖟𝖊𝖐 -

// ---------------- PINS ----------------
#define ena 12
#define in1 26
#define in2 27

#define enaB 2
#define in3 18
#define in4 19

#define ir3Pin 25 // orange wire (IR)
#define ir1Pin 33 // blue wire (IR)
#define ir5Pin 32 // white wire (IR)

#define echoPin 5 // purple wire (HC)
#define trigPin 4 // blue wire (HC)

#define redPin 23
#define orangePin 13
#define greenPin 14

#define button 35

// ---------------- GLOBAL VARIABLES ----------------
int sped = 200;

float duration;
float distance;

int ir3;
int ir1;
int ir5;
int m = 15;

char distanceArray[20];

// ---------------- WIFI & MQTT ----------------
#include <WiFi.h>
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "embed";
const char* password = "weareincontrol";
const char* mqtt_server = "192.168.1.231";
const int mqtt_port = 1883;
const char* mqtt_user = "maximbozek";
const char* mqtt_passw = "odroid";

void setup() {
  Serial.begin(115200);
  
  // ---------------- PINMODES ----------------
  pinMode(ena, OUTPUT);
  digitalWrite(ena, HIGH);
  analogWrite(ena, sped);
  
  pinMode(enaB, OUTPUT);
  digitalWrite(enaB, HIGH);
  analogWrite(enaB, sped);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(ir3Pin, INPUT);
  pinMode(ir1Pin, INPUT);
  pinMode(ir5Pin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(orangePin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  pinMode(button, INPUT);
  
  // ---------------- WIFI & MQTT ----------------
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to wifi.");
  Serial.println("-----");
  
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  // ---------------- MQTT CONNECTION ----------------
  client.loop();
  
  while (!client.connected()) 
  {
    if (client.connect("ESP32Client", mqtt_user, mqtt_passw)) 
    {
     Serial.println("connected to MQTT");
     Serial.println("-----");
    }
    else 
    {
      Serial.print("failed with state ");
      Serial.println(client.state());
      Serial.println("-----");
      delay(2000);
    }
  }
  
  // ---------------- SENSOR READS & PUBLISHES ----------------
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  
  dtostrf(distance, 2, 2, distanceArray);
  client.publish("home/linerobot/distance", distanceArray);
  
  // ---------------- CAR MOVEMENT ----------------
  ir3 = map(analogRead(ir3Pin), 0, 4096, 0, 20);
  ir1 = map(analogRead(ir1Pin), 0, 4096, 0, 20);
  ir5 = map(analogRead(ir5Pin), 0, 4096, 0, 20);
  
  if(distance > 10)
  {
    if ((ir1 <= m) && (ir3 > m) && (ir5 <= m))
    {
      carDrive();
      green();
    }
    else if ((ir1 > m) && (ir3 > m) && (ir5 > m))
    {
      carStop();
      orange();
      for(int i = 0;i <= 5000;)
      {
        i++;
        delay(1);
        if(digitalRead(button) == 1)
        {
          break;
        }
      }
      // delay(2000);
      carDrive();
      green();
      delay(200);
    }
    else if ((ir1 <= m) && (ir3 <= m) && (ir5 <= m))
    {
      carStop();
      red();
    }
    else if ((ir1 = 0) && (ir3 = 0) && (ir5 = 0))
    {
      carStop();
      red();
    }
    else if ((ir1 = 0) || (ir3 = 0) || (ir5 = 0))
    {
      carStop();
      red();
    }
    else if ((ir1 > m) && (ir3 > m) && (ir5 <= m))
    {
      carLeft();
      green();
    }
    else if ((ir1 > m) && (ir3 <= m) && (ir5 <= m))
    {
      carLeft();
      green();
    }
    else if ((ir1 <= m) && (ir3 > m) && (ir5 > m))
    {
      carRight();
      green();
    }
    else if ((ir1 <= m) && (ir3 <= m) && (ir5 > m))
    {
      carRight();
      green();
    }
  }
  else if(distance <= 10)
  {
    carStop();
    orange();
  }

  // ---------------- SENSOR CONTROLE FOR TROUBLESHOOTING ----------------
  /*
  Serial.print("IR: ");
  Serial.print(ir1);
  Serial.print("(1)");
  Serial.print(" | ");
  Serial.print(ir3);
  Serial.print("(3)");
  Serial.print(" | ");
  Serial.print(ir5);
  Serial.println("(5)");
  Serial.print("HC: ");
  Serial.println(distance);
  Serial.print("Button: ");
  Serial.println(digitalRead(button));
  Serial.println("-----");
  delay(500);
  */
}

void carStop(){
  digitalWrite(in1,LOW);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,LOW);
}

void carDrive(){
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
}

void carLeft(){
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,HIGH);
  digitalWrite(in4,LOW);
}

void carRight(){
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
}

void red(){
  digitalWrite(redPin, HIGH);
  digitalWrite(orangePin, LOW);
  digitalWrite(greenPin, LOW);
}

void orange(){
  digitalWrite(redPin, LOW);
  digitalWrite(orangePin, HIGH);
  digitalWrite(greenPin, LOW);
}

void green(){
  digitalWrite(redPin, LOW);
  digitalWrite(orangePin, LOW);
  digitalWrite(greenPin, HIGH);
}

// - 𝕸𝖆𝖝𝖎𝖒 𝕭𝖔𝖟𝖊𝖐 -
