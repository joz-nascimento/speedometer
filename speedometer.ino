#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define BUTTON_A 2
#define BUTTON_B 3
#define BUTTON_C 4
#define BUTTON_D 5
#define BUTTON_E 6
#define BUTTON_F 7
#define BUTTON_K 8
#define SENSOR_1 12
#define SENSOR_2 13
#define PIN_ANALOG_X A0
#define PIN_ANALOG_Y A1
#define DELAY 500

int x = 0;
int y = 0;
int count = 0;
int index = 0;
int index_list = 0;
bool startCount = false;
bool holding = false;
unsigned long startMillis;
unsigned long currentMillis;
int distance = 30;
int distance_temp;
int cursor_pos = 0;
String menuOptions[] = {
    "Use    e tecle A",
    " Inicia leitura ",
    " Edita distancia",
    " 10 melhores    ",
    " Ultimos valores",
    " Unidade: "};
float top10[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float last10[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const float ms_to_kmh = 3.6;
enum Menu {START_MENU, EDIT_DISTANCE, READING, SCORE, TOP_TEN, LAST_TEN, EDIT_UNIT};
enum Unit {M_S, K_H};
Menu currentMenu = START_MENU;
Unit currentUnit = M_S;
Unit tempUnit = M_S;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
// <- Arrow left symbol
char left=0x00+127; 
// -> Arrow right symbol
char right=0x00+126; 
// Arrow up symbol
byte up[] = {
  0x00,
  0x04,
  0x0E,
  0x15,
  0x04,
  0x04,
  0x04,
  0x00
};
// Arrow down Symbol
byte down[] = {
  0x00,
  0x04,
  0x04,
  0x04,
  0x15,
  0x0E,
  0x04,
  0x00
};


void setup() {
  Serial.begin(9600);
  // to enable pull up resistors first write pin mode and then make that pin HIGH
  pinMode(BUTTON_A, INPUT);
  digitalWrite(BUTTON_A, HIGH);

  pinMode(BUTTON_B, INPUT);
  digitalWrite(BUTTON_B, HIGH);

  pinMode(BUTTON_C, INPUT);
  digitalWrite(BUTTON_C, HIGH);

  pinMode(BUTTON_D, INPUT);
  digitalWrite(BUTTON_D, HIGH);

  pinMode(BUTTON_E, INPUT);
  digitalWrite(BUTTON_E, HIGH);

  pinMode(BUTTON_F, INPUT);
  digitalWrite(BUTTON_F, HIGH);

  pinMode(BUTTON_K, INPUT);
  digitalWrite(BUTTON_K, HIGH);


  lcd.init();
  lcd.createChar(0, up);
  lcd.createChar(1, down);
  lcd.backlight();
  //lcd.noBacklight();
  lcd.setCursor(0,0);
  lcd.print("Luis Antonio");
  lcd.setCursor(0,1);
  lcd.print("e Papai");

  pinMode(SENSOR_1, INPUT);
  pinMode(SENSOR_2, INPUT);
}

 void LcdShow(String line1, String line2 = ""){
    lcd.clear();
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);
  }

void GoToMainMenu(){
  currentMenu = START_MENU;
  SelectOption(index);
}

void GoToDistanceMenu(){
  currentMenu = EDIT_DISTANCE;
  LcdShow("", "Dist:   000 cm  ");
  distance_temp = distance;
  UpdateValue();
  cursor_pos = 0;
  UpdateCursor();
}

void GoToReadingScreen(){
  currentMenu = READING;
  if (startCount){
    LcdShow("Sensor 2:", "Aguardando...");
  } else{
    LcdShow("Sensor 1:", "Aguardando...");
  }
}

void GoToUnitMenu(){	
  currentMenu = EDIT_UNIT;
  LcdShow("Selecione unid:", "   m/s    k/h   ");
  if (tempUnit == M_S){
    lcd.setCursor(2,1);
  } else{
    lcd.setCursor(9,1);
  }
  lcd.print(right);
}

void SaveScore(float x){
  Serial.println(x);
  // Save on Last 10
  for (int i = 10; i > 1; i--) {
    last10[i-1] = last10[i-2];
  }
  last10[0] = x;
  // Save on Top 10
  if (x > top10[9]){
    top10[9] = x;
  }
  float temp;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 9; j++) {
      if (top10[j+1] > top10[j]){
        temp = top10[j];
        top10[j] = top10[j+1];
        top10[j+1] = temp;
      }
    }
  }
  Serial.println("Top 10:");
  for (int i = 0; i < 10; i++) {
    Serial.println(top10[i]);
  }
  Serial.println("Last 10:");
  for (int i = 0; i < 10; i++) {
    Serial.println(last10[i]);
  }
}

void PressA(){
  switch (currentMenu) {
    case START_MENU:
      switch (index){
        case 0:
          startCount = false;
          GoToReadingScreen();
        break;
        case 1:
          GoToDistanceMenu();
        break;  
        case 2:
            index_list = 0;
			currentMenu = TOP_TEN;
			ShowScore(index_list);
        break;
        case 3:
            index_list = 0;
			currentMenu = LAST_TEN;
			ShowScore(index_list);
        break;
        case 4:
          tempUnit = currentUnit;
          GoToUnitMenu();
        break;
      }
    break;
    case EDIT_DISTANCE:
      distance = distance_temp;
      GoToMainMenu();
    break;  
    case SCORE:
      GoToReadingScreen();
    break;
    case TOP_TEN:
    case LAST_TEN:
      GoToMainMenu();
    break; 
    case EDIT_UNIT:
      currentUnit = tempUnit;
      GoToMainMenu();
    break;     
  }
  delay(DELAY);
}

void PressB(){
  if (currentMenu != START_MENU){
    GoToMainMenu();
  }
  delay(DELAY); 
}

void ShowScore(int x){
  String s1 = "";
  String s2 = "";
  float tempList[10];
  
  String unit_s = " m/s";
  float mult = 1.0;
  if(currentUnit == K_H){
    unit_s = " k/h";
	mult = ms_to_kmh;
  }
  
  if(currentMenu == TOP_TEN){
	for (int i = 0; i < 10; i++){
	tempList[i] = top10[i] * mult}
  } else{
	for (int i = 0; i < 10; i++){
	tempList[i] = last10[i] * mult}
  }

  switch (x) {
    case 0:
      if(currentMenu == TOP_TEN){
        s1 = " Melhores tempos:";
      } else{
        s1 = " Tempos recentes:";
      }
      s2 = " 1 - " + String(tempList[0]) + unit_s;
    break;
    case 1:
      s1 = " 2 - " + String(tempList[1]) + unit_s;
      s2 = " 3 - " + String(tempList[2]) + unit_s;
    break;
    case 2:
      s1 = " 4 - " + String(tempList[3]) + unit_s;
      s2 = " 5 - " + String(tempList[4]) + unit_s;
    break;
    case 3:
      s1 = " 6 - " + String(tempList[5]) + unit_s; 
      s2 = " 7 - " + String(tempList[6]) + unit_s;
    break;
    case 4:
      s1 = " 8 - " + String(tempList[7]) + unit_s;
      s2 = " 9 - " + String(tempList[8]) + unit_s;
    break;
    case 5:
      s1 = "10 - " + String(tempList[9]) + unit_s;
      s2 = "                ";
    break;  
  }
  LcdShow(s1, s2);
}

void SelectOption(int x){
    //String menuOptions[] = {"Use ▲▼ e tecle A", " Inicia leitura ", " Edita distancia",  " 10 melhores    ",  " Ultimos valores", " Unidade: "}
  
  switch (x) {
    case 0:
    LcdShow(menuOptions[0], menuOptions[1]);
    lcd.setCursor(4,0);
    lcd.write(0);
    lcd.write(1);
    break;
    case 1:
    case 2:
    LcdShow(menuOptions[2], menuOptions[3]);
    break;
    case 3:
    case 4:
      String unit_s = menuOptions[5] + "m/s";
      if(currentUnit == K_H){
        unit_s = menuOptions[5] + "k/h";
      }
      LcdShow(menuOptions[4], unit_s);
    break;
  }
  
    switch (x) {
    case 0:
      lcd.setCursor(0,1);
      lcd.print(right);
    break;
    case 1:
    case 3:
    case 5:
      lcd.setCursor(0,0);
      lcd.print(right); 
      lcd.setCursor(0,1);
      lcd.print(" "); 
    break;  
    case 2:
    case 4:
      lcd.setCursor(0,0);
      lcd.print(" "); 
      lcd.setCursor(0,1);
      lcd.print(right); 
    break;
  }
}

void UpdateValue(){
	String u = String(distance%10);
	String d = String((distance/10)%10);
	String c = String(distance/100);
	// "        ↓↓↓     "
	// "Dist:   000 cm  "
	lcd.setCursor(5,1);
	lcd.print(c+d+u);
}

void UpdateCursor(){
	lcd.setCursor(8,0);
	lcd.print("   ");
	lcd.setCursor(10 - cursor_pos,0);
	lcd.write(1);
}

void loop() {
  if(digitalRead(BUTTON_A) == LOW) {
    PressA();
  }
  else if(digitalRead(BUTTON_B) == LOW) {
    PressB();
  }
  else if(digitalRead(BUTTON_C) == LOW) {
    Serial.println("Button C is pressed");
    delay(DELAY);
  }
  else if(digitalRead(BUTTON_D) == LOW) {
    Serial.println("Button D is pressed");
    delay(DELAY);
  }
  else if(digitalRead(BUTTON_E) == LOW) {
    Serial.println("Button E is pressed");
    delay(DELAY);
  }
  else if(digitalRead(BUTTON_F) == LOW) {
    Serial.println("Button F is pressed");
    delay(DELAY);
  }
  if(digitalRead(BUTTON_K) == LOW) {
    Serial.println("Button K is pressed");
    delay(DELAY);
  }
  
  x = analogRead(PIN_ANALOG_X);
  y = analogRead(PIN_ANALOG_Y);

  if (y > 480 && y < 534 && x > 480 && x < 534){
      holding = false;
    }
   
  if (currentMenu == TOP_TEN && holding == false || currentMenu == LAST_TEN && holding == false){
    if (y > 534){
      holding = true;
      index_list = index_list - 1;
      if (index_list < 0){
        index_list = 5;
      }
      ShowScore(index_list);
    }
    else if (y < 480){
      holding = true;
      index_list = index_list + 1;
      if (index_list > 5){
        index_list = 0;
      }
      ShowScore(index_list);
    }
  }
  
  if (currentMenu == START_MENU && holding == false){
    if (y > 534){
      holding = true;
      index = index - 1;
      if (index < 0){
        index = 4;
      }
      SelectOption(index);
    }
    else if (y < 480){
      holding = true;
      index = index + 1;
      if (index > 4){
        index = 0;
      }
      SelectOption(index);
    }
  }
  
  if (currentMenu == EDIT_DISTANCE && holding == false){
    if (x > 534){
      holding = true;
      cursor_pos = cursor_pos + 1;
      if (cursor_pos > 2){
        cursor_pos = 0;
      }
      Positioning_Cursor(cursor_pos);
    }
    else if (x < 480){
      holding = true;
      cursor_pos = cursor_pos - 1;
      if (cursor_pos < 0){
        cursor_pos = 0;
      }
      Positioning_Cursor(cursor_pos);
    }
	else if (y > 534){
      holding = true;
      distance_temp = distance_temp - pow(1, cursor_pos);;
      if (distance_temp < 0){
        distance_temp = 0;
      }
      UpdateValue();
    }
    else if (y < 480){
      holding = true;
      distance_temp = distance_temp + pow(1, cursor_pos);
      if (distance_temp > 999){
        distance_temp = 999;
      }
      UpdateValue();
    }
  }
  
  if (currentMenu == EDIT_UNIT && holding == false){
    if (x < 480 || x > 534){
      holding = true;
      if (tempUnit == M_S){
        tempUnit = K_H;
      } else{
        tempUnit = M_S;
      }
      GoToUnitMenu();
    }
  }
  
  if (currentMenu == READING){
    if(startCount && digitalRead(SENSOR_2) == LOW){
      currentMillis = millis();
      float deltaTime = float(currentMillis - startMillis)/1000.0;
      float velocity = float(distance)/deltaTime;
      startCount = false;
      currentMenu = SCORE;
	  String s2 = String(velocity) + " m/s";
	  if(currentUnit == K_H){
		  String(velocity * ms_to_kmh) + " k/h";
	  }
      LcdShow("Velocidade:", s2);
      SaveScore(velocity);
    }
    if(digitalRead(SENSOR_1) == LOW){
      startCount = true;
      startMillis = millis();
      GoToReadingScreen();
    }
  }
}
