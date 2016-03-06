#include <Servo.h>
// web client library
#include <SPI.h>
#include <Ethernet.h>

#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// web client configuration
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 137, 15);
IPAddress myDns(83, 171,22, 2);
EthernetClient client;
char server[] = "table-tennis-api.herokuapp.com"; 

unsigned long lastConnectionTime = 0;             
const unsigned long postingInterval = 10L * 100L; 
boolean request_time_state = LOW;

const int state_none = -1;
const int state_begining_A = 0;
const int state_begining_B = 1;
const int state_game_A = 2;
const int state_game_B = 3;
const int table_hit = 1;
const int table_hit_none = 0;
const int time_out_millis = 2000;
const int cool_down_time_out = 50;
const int serving_player_NA = 0; // N/A
const int player_A = 1;
const int player_B = 2;

// pins
const int side_sensor_A = 6;
const int side_sensor_B = 7;
const int RGB_led_A = 53;
const int RGB_led_B = 51;
const int pin_button_reset = 30;
const int pin_button_A_add = 31;
const int pin_button_A_remove = 32;
const int pin_button_B_add = 33;
const int pin_button_B_remove = 34;
const int pin_servo = 8;

// flag positions
const int flag_position_player_A = 0;
const int flag_position_player_B = 180;
const int flag_position_player_none = 90;

int button_temp_value;

int state = state_none;


unsigned long last_table_hit = 0;    

int score_A = 0;
int score_B = 0;

int rally_length = 0;

int serving_player = serving_player_NA;
int initial_serving_player = serving_player_NA;

Servo flag_servo;

void setup() {
  Serial.begin(115200);
  
  Ethernet.begin(mac, ip, myDns);
  
  lcd.begin(16,2);  
  lcd.backlight(); 

  pinMode(RGB_led_A, OUTPUT);
  pinMode(RGB_led_B, OUTPUT);
  
  pinMode(pin_button_reset, INPUT);
  pinMode(pin_button_A_add, INPUT);
  pinMode(pin_button_A_remove, INPUT);
  pinMode(pin_button_B_add, INPUT);
  pinMode(pin_button_B_remove, INPUT);
  
  flag_servo.attach(pin_servo);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  if(millis() - last_table_hit < cool_down_time_out) return; 
  
  int side_A = sense_table(side_sensor_A, 430);
  int side_B = sense_table(side_sensor_B, 400);
  
  if (side_A == table_hit || side_B == table_hit){
    last_table_hit = millis();
  }
  boolean time_out = (millis() - last_table_hit) > time_out_millis;
  
  scoring_state_machine(side_A, side_B, time_out);

  buttons_controller();
  flag_controller();
  display_scores();
  detect_winner();
}

void scoring_state_machine(int side_A, int side_B, boolean time_out) {
  switch (state){
    case state_none:
      print_state("state_none: ", score_A, score_B, side_A, side_B);
      transition_state_none(side_A, side_B, time_out);
      break;
    case state_begining_A: 
      print_state("state_begining_A: ", score_A, score_B, side_A, side_B);
      transition_state_begining_A(side_A, side_B, time_out);
      break;
    case state_begining_B:
      print_state("state_begining_B: ", score_A, score_B, side_A, side_B);
      transition_state_begining_B(side_A, side_B, time_out);
      break;
    case state_game_A:
      print_state("state_game_A: ", score_A, score_B, side_A, side_B);
      transition_state_game_A(side_A, side_B, time_out);
      break;
    case state_game_B:
      print_state("state_game_B: ", score_A, score_B, side_A, side_B);
      transition_state_game_B(side_A, side_B, time_out);
      break;
    default:
      print_state("default: ", score_A, score_B, side_A, side_B);
      break;
  }
}

void print_state(String state, int score_A, int score_B, int side_A, int side_B) {
  Serial.println(state + (String) score_A + ":" + (String) score_B + " (" + (String) side_A + ":" + (String) side_B);
}

int sense_table(int side, int calibration) {
   int sensor_value = analogRead(side);
   if (sensor_value > calibration) {
     return  table_hit;
   }
   
   return  table_hit_none;
}

void transition_state_none(int side_A, int side_B, boolean time_out) {
  if (side_A == table_hit)
  {
     state = state_begining_A; 
     return;
  }
  if (side_B == table_hit)
  {
     state = state_begining_B; 
     return;
  }
  
  if (time_out)
  {
     // nothing happens 
     return;
  }
}
void transition_state_begining_A(int side_A, int side_B, boolean time_out) {
  if (side_A == table_hit)
  {
     // stay in the same state
     return;
  }
  if (side_B == table_hit)
  {
     state = state_game_B; 
     return;
  }
  
  if (time_out)
  {
     state = state_none;
     return;
  }
}

void transition_state_begining_B(int side_A, int side_B, boolean time_out) {
  if (side_A == table_hit)
  {
     state = state_game_A; 
     return;
  }
  if (side_B == table_hit)
  {
     // stay in the same state
     return;
  }
  
  if (time_out)
  {
     state = state_none; 
     return;
  }
}

void transition_state_game_A(int side_A, int side_B, boolean time_out) {
  if (side_A == table_hit)
  {
     add_score_to_B(); 
     return;
  }
  if (side_B == table_hit)
  {
     state = state_game_B; 
     return;
  }
  
  if (time_out)
  {
     add_score_to_B();
     return;
  }
}

void transition_state_game_B(int side_A, int side_B, boolean time_out) {
  if (side_A == table_hit)
  {
     state = state_game_A;
     return;
  }
  if (side_B == table_hit)
  {
     add_score_to_A();
     return;
  }
  
  if (time_out)
  {
     add_score_to_A(); 
     return;
  }
}

void add_score_to_A () {
  score_A += 1;
  state = state_none;
  
  update_whos_serving(player_A);
}

void add_score_to_B () {
  score_B += 1;
  state = state_none;
  
  update_whos_serving(player_B);
}

void update_whos_serving(int point_winner) {
  int score_sum = score_A + score_B;

  if (score_sum == 1 && serving_player == serving_player_NA) {
    score_A = 0; score_B = 0;
    
    if (point_winner == player_A) {
      initial_serving_player = player_A;
    } else {
      initial_serving_player = player_B;
    }
  }
  update_serving_player();
}

void update_serving_player() {
  int score_sum = score_A + score_B;

  if(score_sum % 4 < 2) {
    serving_player = initial_serving_player;
  } else {
    if (initial_serving_player == player_A) {
      serving_player = player_B;
    } else if (initial_serving_player == player_B) {
      serving_player = player_A;
    }
  }
}

void buttons_controller() {
  //pin_button_reset
  button_temp_value = digitalRead(pin_button_reset);
  if (button_temp_value == HIGH) {
    reset_game();
    update_serving_player();
  }
  
  //pin_button_A_add
  button_temp_value = digitalRead(pin_button_A_add);
  if (button_temp_value == HIGH) {
    score_A += 1;
    update_serving_player();
  }
  
  //pin_button_A_remove
  button_temp_value = digitalRead(pin_button_A_remove);
  if (button_temp_value == HIGH) {
    score_A -= 1;
    update_serving_player();
  }
  
  //pin_button_B_add
  button_temp_value = digitalRead(pin_button_B_add);
  if (button_temp_value == HIGH) {
    score_B += 1;
    update_serving_player();
  }
  
  //pin_button_B_remove
  button_temp_value = digitalRead(pin_button_B_remove);
  if (button_temp_value == HIGH) {
    score_B -= 1;
    update_serving_player();
  }
}

void flag_controller() {
  switch (serving_player) {
    case player_A:
      flag_servo.write(flag_position_player_A);
      break;
    case player_B:
      flag_servo.write(flag_position_player_B);
      break;
    case serving_player_NA:
    default:
      flag_servo.write(flag_position_player_none);
      break;
  }
}

void reset_game() {
  serving_player = serving_player_NA;
  score_A = 0;
  score_B = 0;
}

int httpRequest(int score_A, int score_B) {
  client.stop();
  if( request_time_state == 'LOW')
  {
    lastConnectionTime = millis();
    request_time_state = 'HIGH';
  }
  if (client.connect(server, 80)) {
    String stringOne =  String("{\"game\":{\"challenger_score\":" + String(score_A) + ",\"challenged_score\":" + String(score_B) + "}}"); 
    String content_length = "Content-Length: ";
    client.println("POST /v1/games HTTP/1.1");
    client.println("Host: table-tennis-api.herokuapp.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: keep-alive");
    client.println("Content-Type: application/json");
    client.println(content_length + stringOne.length());
    client.println();
    client.println(stringOne);
    request_time_state = 'LOW';
    return 1;
  } else {
    Serial.println("connection error");
    if (millis() - lastConnectionTime > postingInterval) { 
      return 0;       
    }
  }
}

void send_scores_to_server() {
  int state = httpRequest(score_A, score_B);
  Serial.println("http request state: " + (String) state);
}

void display_scores() {
  display_on_lcd("Player left: " + (String)score_A, "Player right: " + (String)score_B);
}

void display_on_lcd(String line_top, String line_bottom) {
  lcd.setCursor(0,0);
  lcd.print(line_top);
  lcd.setCursor(0,1);
  lcd.print(line_bottom);
}

void detect_winner() {
//  if (score_A > game_length)
}


  


