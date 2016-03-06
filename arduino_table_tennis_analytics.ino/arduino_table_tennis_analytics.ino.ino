const int state_none = -1;
const int state_begining_A = 0;
const int state_begining_B = 1;
const int state_game_A = 2;
const int state_game_B = 3;
const int table_hit = 1;
const int table_hit_none = 0;
const int time_out_millis = 2000;
const int cool_down_time_out = 50;

int state = state_none;

int side_sensor_A = 0;
int side_sensor_B = 1;

int RGB_led_A = 53;
int RGB_led_B = 51;

unsigned long last_table_hit = 0;    

int score_A = 0;
int score_B = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(RGB_led_A, OUTPUT);
  pinMode(RGB_led_B, OUTPUT);
}

void loop() {
  if(millis() - last_table_hit < cool_down_time_out) return; 
  
  int side_A = sense_table(side_sensor_A, 430);
  int side_B = sense_table(side_sensor_B, 400);
  
  if (side_A == table_hit || side_B == table_hit){
    last_table_hit = millis();
  }
  boolean time_out = (millis() - last_table_hit) > time_out_millis;
  
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
   Serial.print( sensor_value);
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
}

void add_score_to_B () {
  score_B += 1;
  state = state_none;
}

