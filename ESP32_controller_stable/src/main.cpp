#include <WiFi.h>
#include <WebServer.h>

#define sendPin 27 
#define Pin1 26
#define Pin2 25
#define Pin3 33
#define Pin4 32

const char *ssid = "";        // 任意のWifiのSSID
const char *pass = "";    //  任意のWifiのパスワード
WebServer Server(80);         //  ポート番号（HTTP）

String message = 
  "<html lang=\"ja\">\n\
  <meta charset=\"utf-8\">\n\
  <center>\
  <h2>Universal remote controller</h2>\
  <p><button type='button' onclick='location.href=\"/1\"' \
    style='width:200px;height:40px;'>No.1</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/2\"' \
    style='width:200px;height:40px;'>No.2</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/3\"' \
    style='width:200px;height:40px;'>No.3</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/4\"' \
    style='width:200px;height:40px;'>No.4</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/5\"' \
    style='width:200px;height:40px;'>No.5</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/6\"' \
    style='width:200px;height:40px;'>No.6</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/7\"' \
    style='width:200px;height:40px;'>No.7</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/8\"' \
    style='width:200px;height:40px;'>No.8</button></p>\
  <center>\
  <p><button type='button' onclick='location.href=\"/9\"' \
    style='width:200px;height:40px;'>No.9</button></p>\
  </center>";


//  クライアントにウェブページ（HTML）を返す関数
void SendMessage() {
  //  レスポンス文字列の生成（'\n' は改行; '\' は行継続）
  Serial.println("SendMessage");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
}

void SendMessage_1() {
  Serial.println("1");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_2() {
  Serial.println("2");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_3() {
  Serial.println("3");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_4() {
  Serial.println("4");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_5() {
  Serial.println("5");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_6() {
  Serial.println("6");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_7() {
  Serial.println("7");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, HIGH);
  digitalWrite(Pin3, HIGH);
  digitalWrite(Pin4, LOW);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_8() {
  Serial.println("8");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, LOW);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}
void SendMessage_9() {
  Serial.println("9");
  //  クライアントにレスポンスを返す
  Server.send(200, "text/html", message);
  //信号送信
  digitalWrite(Pin1, HIGH);
  digitalWrite(Pin2, LOW);
  digitalWrite(Pin3, LOW);
  digitalWrite(Pin4, HIGH);
  digitalWrite(sendPin, HIGH);
  delay(200);
  digitalWrite(sendPin, LOW);
}



//  クライアントにエラーメッセージを返す関数
void SendNotFound() {
  Serial.println("SendNotFound");
  Server.send(404, "text/plain", "404 not found...");
}

//  メインプログラム
void setup() {
  pinMode(sendPin, OUTPUT);
  pinMode(Pin1, OUTPUT);
  pinMode(Pin2, OUTPUT);
  pinMode(Pin3, OUTPUT);
  pinMode(Pin4, OUTPUT);
  //  シリアルモニタ（動作ログ）
  Serial.begin(115200);               //  ESP  標準の通信速度 115200
  delay(100);                         //  100ms ほど待ってからログ出力可
  Serial.println("\n*** Starting ***");
  //  無線 LAN に接続
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);             
  Serial.println("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Can't connect");
    }
  }
  Serial.println("Connected");
  Serial.println(WiFi.localIP());     //  ESP 自身の IP アドレスをログ出力
  //  ウェブサーバの設定
  Server.on("/", SendMessage);         //  ルートアクセス時の応答関数を設定
  Server.on("/1", SendMessage_1);
  Server.on("/2", SendMessage_2);
  Server.on("/3", SendMessage_3);
  Server.on("/4", SendMessage_4);
  Server.on("/5", SendMessage_5);
  Server.on("/6", SendMessage_6);
  Server.on("/7", SendMessage_7);
  Server.on("/8", SendMessage_8);
  Server.on("/9", SendMessage_9);
  Server.onNotFound(SendNotFound);  //  不正アクセス時の応答関数を設定
  Server.begin();                     //  ウェブサーバ開始
}
void loop() {
  //  クライアントからの要求を処理する
  Server.handleClient();
}
