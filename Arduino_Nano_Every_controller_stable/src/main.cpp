#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <IRremote.hpp>
#include "PinDefinitionsAndMore.h"

//IR_RECEIVE_PIN 2
//IR_SEND_PIN 3

#define BUTTON_PIN_UP 1
#define BUTTON_PIN_DOWN 14
#define BUTTON_PIN_LEFT 0
#define BUTTON_PIN_RIGHT 6
#define BUTTON_PIN_MIDDLE 8


#define RECEIVE_SIGNAL_Pin 7
#define RECEIVE_Pin_1 9
#define RECEIVE_Pin_2 10
#define RECEIVE_Pin_3 11
#define RECEIVE_Pin_4 12

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char key_input;

int mode = 1;   //動作モード、1,送信(通常) 2,名前変更 3,データ消去(警告) 4,信号保存
int data_num = 0;      //0~8まで  保存データに割り当てられる数字

int name_data_selecting = 0; //名前変更時の配列指定用

int IR_read_trig = 0; //信号読み取りのトリガ

int data_name[9][4] = {{0,1,2,3},  //名前保存用(data_name_listを参照)
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3},
                       {0,1,2,3}};
                      
//A~Z,0~9,空白含む37の文字
char data_name_list[37] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
  'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '
};

String data_protocol[9] = {"None","None","None","None","None","None","None","None","None"}; //0~8まで　保存データのプロトコルの名前

uint16_t data_address[9];
uint16_t data_command[9];
uint32_t data_rawdata[9];

uint8_t savedSignalLength[9];
uint8_t savedIRSignal8[9][100];      //生データ配列


int eep_read(int device_add, unsigned int eep_add){
  Wire.beginTransmission(device_add);
  Wire.write((int)(eep_add >> 8));
  Wire.write((int)(eep_add & 0xff));
  Wire.endTransmission();

  Wire.requestFrom(device_add,1);
  if(Wire.available()){}
  return Wire.read();
}

void eep_write(int device_add, unsigned int eep_add, int data){
  Wire.beginTransmission(device_add);//指定したデバイスへ移動
  Wire.write((int)(eep_add >> 8));
  Wire.write((int)(eep_add & 0xff));
  Wire.write(data);
  Wire.endTransmission();
}

void OLED_disp(){
  display.clearDisplay();

  if(mode == 1){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.print("No.");     //1行目、番号の表示
    display.println(data_num + 1);

    display.print("Name:");   //2行目、名前の表示
    display.print(data_name_list[data_name[data_num][0]]);
    display.print(data_name_list[data_name[data_num][1]]);
    display.print(data_name_list[data_name[data_num][2]]);
    display.println(data_name_list[data_name[data_num][3]]);

    display.print("Protocol:");
    display.println(data_protocol[data_num]);
    display.print("Address :");
    display.print("0x");
    display.println(data_address[data_num],HEX); 
    display.print("Command :");
    display.print("0x");
    display.println(data_command[data_num],HEX); 
    display.print("Raw-Data:");
    display.print("0x");
    display.println(data_rawdata[data_num],HEX); 
  }

  if(mode == 2){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.print("No.");     //1行目、番号の表示
    display.println(data_num + 1);

    display.print("Name:");   //2行目、名前の表示、選択中の文字を反転
    if(name_data_selecting == 0){
      display.setTextColor(BLACK, WHITE);
      display.print(data_name_list[data_name[data_num][0]]);
      display.setTextColor(WHITE);
      display.print(data_name_list[data_name[data_num][1]]);
      display.print(data_name_list[data_name[data_num][2]]);
      display.println(data_name_list[data_name[data_num][3]]);
    }
    if(name_data_selecting == 1){
      display.print(data_name_list[data_name[data_num][0]]);
      display.setTextColor(BLACK, WHITE);
      display.print(data_name_list[data_name[data_num][1]]);
      display.setTextColor(WHITE);
      display.print(data_name_list[data_name[data_num][2]]);
      display.println(data_name_list[data_name[data_num][3]]);
    }
    if(name_data_selecting == 2){
      display.print(data_name_list[data_name[data_num][0]]);
      display.print(data_name_list[data_name[data_num][1]]);
      display.setTextColor(BLACK, WHITE);
      display.print(data_name_list[data_name[data_num][2]]);
      display.setTextColor(WHITE);
      display.println(data_name_list[data_name[data_num][3]]);
    }
    if(name_data_selecting == 3){
      display.print(data_name_list[data_name[data_num][0]]);
      display.print(data_name_list[data_name[data_num][1]]);
      display.print(data_name_list[data_name[data_num][2]]);
      display.setTextColor(BLACK, WHITE);
      display.println(data_name_list[data_name[data_num][3]]);
      display.setTextColor(WHITE);
    }

    display.print("Protocol:");
    display.println(data_protocol[data_num]);
    display.print("Address :");
    display.print("0x");
    display.println(data_address[data_num],HEX); 
    display.print("Command :");
    display.print("0x");
    display.println(data_command[data_num],HEX); 
    display.print("Raw-Data:");
    display.print("0x");
    display.println(data_rawdata[data_num],HEX); 
  }

  if(mode == 3){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.println("!WARNING!");
    display.println("If you rewrite");
    display.print(" No.");
    display.print(data_num);
    display.println(" data");
    display.println("the original data");
    display.println(" will be lost");
    display.println("");
    display.println("If you continue");
    display.println("press button again");
  }

  if(mode == 4){
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.print("No.");
    display.println(data_num + 1);
    display.println("Searching IRsignal...");

    if(IR_read_trig == 1){
      display.println("Protocol:");
      display.println("Address :");
      display.println("Command :");
      display.println("Raw-Data:");
    }

    if(IR_read_trig == 0){
      display.print("Protocol:");
      display.println(data_protocol[data_num]);
      display.print("Address :");
      display.print("0x");
      display.println(data_address[data_num],HEX); 
      display.print("Command :");
      display.print("0x");
      display.println(data_command[data_num],HEX); 
      display.print("Raw-Data:");
      display.print("0x");
      display.println(data_rawdata[data_num],HEX); 
      display.println();
      display.println("data found");
      display.display();
      delay(2000);
      mode = 1;
    }
  }

  display.display();
}

String getProtocolName(decode_type_t protocol) {
  switch (protocol) {
    case 0: return "UNKNOWN";
    case 1: return "PULSE_WIDTH";
    case 2: return "PULSE_DISTANCE";
    case 3: return "APPLE";
    case 4: return "DENON";
    case 5: return "JVC";
    case 6: return "LG";
    case 7: return "LG2";
    case 8: return "NEC";
    case 9: return "NEC2";
    case 10: return "ONKYO";
    case 11: return "PANASONIC";
    case 12: return "KASEIKYO";
    case 13: return "KASEIKYO_DENON";
    case 14: return "KASEIKYO_SHARP";
    case 15: return "KASEIKYO_JVC";
    case 16: return "KASEIKYO_MITSUBISHI";
    case 17: return "RC5";
    case 18: return "RC6";
    case 19: return "RC6A";
    case 20: return "SAMSUNG";
    case 21: return "SAMSUNGLG";
    case 22: return "SAMSUNG48";
    case 23: return "SHARP";
    case 24: return "SONY";
    case 25: return "BANG_OLUFSEN";
    case 26: return "BOSEWAVE";
    case 27: return "LEGO_PF";
    case 28: return "MAGIQUEST";
    case 29: return "WHYNTER";
    case 30: return "FAST";
    default: return "OTHER";
  }
}

void IR_read(){
  if(IR_read_trig == 1){
      if (IrReceiver.decode()) {  // Grab an IR code
          if (IrReceiver.decodedIRData.protocol != UNKNOWN && IrReceiver.decodedIRData.rawDataPtr->rawlen > 4){ //受信をUNKNOWN以外かつ配列数4以上に限定
              if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
                  Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));
              } else {
                  if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
                      data_protocol[data_num] = getProtocolName(IrReceiver.decodedIRData.protocol);
                      data_address[data_num] = IrReceiver.decodedIRData.address;
                      data_command[data_num] = IrReceiver.decodedIRData.command;
                      data_rawdata[data_num] = IrReceiver.decodedIRData.decodedRawData;

                      Serial.println(data_protocol[data_num]);
                      Serial.println(data_address[data_num],HEX);
                      Serial.println(data_command[data_num],HEX);
                      Serial.println(data_rawdata[data_num],HEX);

                      savedSignalLength[data_num] = IrReceiver.decodedIRData.rawDataPtr->rawlen;

                      Serial.println(savedSignalLength[data_num]);

                      for (uint8_t i = 1; i < savedSignalLength[data_num]; i++) {
                        savedIRSignal8[data_num][i] = IrReceiver.decodedIRData.rawDataPtr->rawbuf[i];
                        Serial.print(savedIRSignal8[data_num][i]);
                        Serial.print(",");
                      }
                      for (uint8_t i = 1; i < savedSignalLength[data_num]; i++) {
                        uint16_t savedIRSignal16[savedSignalLength[data_num]];
                        savedIRSignal16[i]  = savedIRSignal8[data_num][i] * MICROS_PER_TICK;
                        if (i & 1) {
                          savedIRSignal16[i] -= MARK_EXCESS_MICROS;
                        } else {
                          savedIRSignal16[i] += MARK_EXCESS_MICROS;
                        }
                        Serial.print(savedIRSignal16[i]);
                        Serial.print(",");
                      }

                      int count = 90;   //90~98 savedsignallength
                      for(int i=0; i<9; i++){
                        eep_write(0x50,count,savedSignalLength[i]);
                        delay(5);//書込み待ち
                        //Serial.print(count);
                        //Serial.print(":");
                        //Serial.println(savedSignalLength[i]);
                        count++;
                      }
                      count = 100 * (data_num + 1);   //data num = 0~8 0→100~199 1→200~299 ...8→900→999
                      for(int i=1; i<100; i++){
                        eep_write(0x50,count,savedIRSignal8[data_num][i]);
                        delay(5);//書込み待ち
                        //Serial.print(count);
                        //Serial.print(":");
                        //Serial.print(savedIRSignal8[data_num][i]);
                        //Serial.print(",");
                        count++;
                      }                      
                      IR_read_trig = 0;
                  }
              Serial.println();
              }
          }
      }
  IrReceiver.resume();
  }    
}

void IR_send(){
  //9080,4470,580,570,580,570,580,520,630,520,630,520,580,570,580,570,580,1670,530,620,530,1720,580,1720,530,1720,530,1770,530,570,580,1720,530,1720,530,620,530,620,580,570,580,570,580,570,580,520,630,520,630,520,580,1670,630,1670,580,1670,630,1670,580,1670,580,1670,630,1670,580,1670,580
  uint16_t savedIRSignal32[savedSignalLength[data_num]];
  for (uint8_t i = 1; i < savedSignalLength[data_num]; i++) {
    savedIRSignal32[i]  = savedIRSignal8[data_num][i] * MICROS_PER_TICK;
    if (i & 1) {
      savedIRSignal32[i] -= MARK_EXCESS_MICROS;
    } else {
      savedIRSignal32[i] += MARK_EXCESS_MICROS;
    }
  }
  IrSender.sendRaw(savedIRSignal32, savedSignalLength[data_num], 38);
  Serial.println("IR_send");
  delay(10);
}

void ESP32_receieve(){
  if(mode == 1){
    if(digitalRead(RECEIVE_SIGNAL_Pin) == HIGH){
      if(digitalRead(RECEIVE_Pin_1) == HIGH && digitalRead(RECEIVE_Pin_2) == LOW && digitalRead(RECEIVE_Pin_3) == LOW && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 0; //1
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_0");
      }
      if(digitalRead(RECEIVE_Pin_1) == LOW && digitalRead(RECEIVE_Pin_2) == HIGH && digitalRead(RECEIVE_Pin_3) == LOW && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 1; //2
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_1");
      }
      if(digitalRead(RECEIVE_Pin_1) == HIGH && digitalRead(RECEIVE_Pin_2) == HIGH && digitalRead(RECEIVE_Pin_3) == LOW && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 2; //3
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_2");
      }
      if(digitalRead(RECEIVE_Pin_1) == LOW && digitalRead(RECEIVE_Pin_2) == LOW && digitalRead(RECEIVE_Pin_3) == HIGH && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 3; //4
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_3");
      }
      if(digitalRead(RECEIVE_Pin_1) == HIGH && digitalRead(RECEIVE_Pin_2) == LOW && digitalRead(RECEIVE_Pin_3) == HIGH && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 4; //5
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_4");
      }
      if(digitalRead(RECEIVE_Pin_1) == LOW && digitalRead(RECEIVE_Pin_2) == HIGH && digitalRead(RECEIVE_Pin_3) == HIGH && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 5; //6
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_5");
      }
      if(digitalRead(RECEIVE_Pin_1) == HIGH && digitalRead(RECEIVE_Pin_2) == HIGH && digitalRead(RECEIVE_Pin_3) == HIGH && digitalRead(RECEIVE_Pin_4) == LOW){
        data_num = 6; //7
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_6");
      }
      if(digitalRead(RECEIVE_Pin_1) == LOW && digitalRead(RECEIVE_Pin_2) == LOW && digitalRead(RECEIVE_Pin_3) == LOW && digitalRead(RECEIVE_Pin_4) == HIGH){
        data_num = 7; //8
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_7");
      }
      if(digitalRead(RECEIVE_Pin_1) == HIGH && digitalRead(RECEIVE_Pin_2) == LOW && digitalRead(RECEIVE_Pin_3) == LOW && digitalRead(RECEIVE_Pin_4) == HIGH){
        data_num = 8; //9
        key_input = 'g';
        //IR_send();
        Serial.println("ESP32_8");
      }
    }
  }
}

void keyboard_read(){
  static int button_up = 0;
  static int button_down = 0;
  static int button_left = 0;
  static int button_right = 0;
  static int button_middle = 0;

  key_input = Serial.read();

  ESP32_receieve();

  if (digitalRead(BUTTON_PIN_UP) == LOW && button_up == 0) {
    button_up = 1;
    key_input = 'w';
  }else{
    button_up = 0;
  }
  if (digitalRead(BUTTON_PIN_DOWN) == LOW && button_down == 0) {
    button_down = 1;
    key_input = 's';
  }else{
    button_down = 0;
  }
  if (digitalRead(BUTTON_PIN_LEFT) == LOW && button_left == 0) {
    button_left = 1;
    key_input = 'a';
  }else{
    button_left = 0;
  }
  if (digitalRead(BUTTON_PIN_RIGHT) == LOW && button_right == 0) {
    button_right = 1;
    key_input = 'd';
  }else{
    button_right = 0;
  }
  if (digitalRead(BUTTON_PIN_MIDDLE) == LOW && button_middle == 0) {
    button_middle = 1;
    key_input = 'g';
  }else{
    button_middle = 0;
  }

  if(key_input == 'w'){
    Serial.println("W");
    if(mode == 2 && data_name[data_num][name_data_selecting] < 36){ //名前変更モード 文字の選択
      data_name[data_num][name_data_selecting]++;
    }
    if(mode == 3){
      mode = 4;         //読み取りモード
      IR_read_trig = 1;
      IrReceiver.resume();
    }
    if(mode == 1){
      mode = 3;     //警告画面表示
    }

  }
  if(key_input == 'a'){
    Serial.println("A");
    if(mode == 1 && data_num > 0){  //通常モード データの選択
      data_num--;
    }
    if(mode == 2 && name_data_selecting > 0){      //名前変更モード 名前の選択
      name_data_selecting--; 
    }
  }
  if(key_input == 's'){
    Serial.println("S");
    if(mode == 2 && data_name[data_num][name_data_selecting] > 0){  //名前変更モード 文字の選択
      data_name[data_num][name_data_selecting]--;
    }
    if(mode == 1){    //通常モード 名前変更の切り替え
      mode = 2;
    }
    if(mode == 3){
      mode = 1;
    }
    if(mode == 4){
      mode = 1;
    }
  }
  if(key_input == 'd'){
    Serial.println("D");
    if(mode == 1 && data_num < 8){  //通常モード データの選択
      data_num++;
    }
    if(mode == 2 && name_data_selecting < 3){    //名前変更モード 名前の選択
      name_data_selecting++;
    }
  }
  if(key_input == 'g'){
    Serial.println("G");
    if(mode == 1){
      IR_send();
    }
    if(mode == 2){                            //名前変更モード 名前の決定、通常モードに戻る
      if(name_data_selecting < 3){
        name_data_selecting++;
      }else{
      int count = 0;    //0~35名前保存
      for(int i=0; i<9; i++){
        for(int j=0; j<4; j++){
          eep_write(0x50,count,data_name[i][j]);
          delay(5);//書込み待ち
          count = count + 1;
        }
      }
        mode = 1;
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  //ボタンセットアップ
  pinMode(BUTTON_PIN_UP,INPUT_PULLUP);
  pinMode(BUTTON_PIN_DOWN,INPUT_PULLUP);
  pinMode(BUTTON_PIN_LEFT,INPUT_PULLUP);
  pinMode(BUTTON_PIN_RIGHT,INPUT_PULLUP);
  pinMode(BUTTON_PIN_MIDDLE,INPUT_PULLUP);

  pinMode(RECEIVE_SIGNAL_Pin,INPUT);
  pinMode(RECEIVE_Pin_1,INPUT);
  pinMode(RECEIVE_Pin_2,INPUT);
  pinMode(RECEIVE_Pin_3,INPUT);
  pinMode(RECEIVE_Pin_4,INPUT);

  //OLEDセットアップ
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 disconnection"));
    for(;;); 
  }
  display.clearDisplay(); //初期化

  //IRremoteセットアップ
  pinMode(LED_BUILTIN, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin(IR_SEND_PIN);
  Serial.print(F("Ready to receive IR signals of protocols: "));
  printActiveIRProtocols(&Serial);

/*初期化用
  for(int i=0; i<1000; i++){
    eep_write(0x50,i,0);
    delay(5);//書込み待ち
  }
*/
  //EEPROM読み取り
  //0~35 名前
  int count = 0;
  for(int i=0; i<9; i++){
    for(int j=0; j<4; j++){
      data_name[i][j] = eep_read(0x50,count);
      delay(5);//書込み待ち
      //Serial.print(count);
      //Serial.print(":");
      //Serial.println(data_name[i][j]);
      count++;
    }
  }
  //90~98 savedSignalLength
  count = 90;
  for(int i=0; i<9; i++){
    savedSignalLength[i] = eep_read(0x50,count);
    delay(5);
    //Serial.print(count);
    //Serial.print(":");
    //Serial.println(savedSignalLength[i]);
    count++;
  }
  //100~999 savedIRsignal8
  count = 100;
  for(int i=0; i<9; i++){
    for(int j=0; j<100; j++){
      count = 100*(i+1)+j;
      savedIRSignal8[i][j] = eep_read(0x50,count);
      delay(5);
      //Serial.print(count);
      //Serial.print(":");
      //Serial.print(savedIRSignal8[i][j]);
      //Serial.println("");
      count++;
    }   
  }
}

void loop() {
  keyboard_read();
  IR_read();
  OLED_disp();
  delay(10);
}