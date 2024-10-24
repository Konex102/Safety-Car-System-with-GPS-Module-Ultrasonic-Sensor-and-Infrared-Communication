#include <IRrecv.h>
#include <IRremoteESP8266.h>

# define RECV_PIN 25
# define Buzzer   27

IRrecv irrecv(RECV_PIN);
decode_results results;

// Dot dan dash untuk bunyi buzzer
const int dot_length = 200;
const int dash_length = 600;

void setup(){
  Serial.begin(115200);
  pinMode(Buzzer,OUTPUT);
  irrecv.enableIRIn();
}

void loop(){
  if (irrecv.decode(&results))
  {
    if(results.value == 0xB18D){
      // Kode unik saat kecepatan normal
      Serial.println("Kecepatan kendaraan normal");
      playBuzzer("- . . . .");
      delay(1000);
    }else if(results.value == 0xB24D){
      // Kode unik saat kecepatan naik
      Serial.println("Kecepatan kendaraan tinggi");
      playBuzzer("- - . . .");
      delay(1000);
    }else if(results.value == 0xB42D){
      // Kode unik saat kecepatan melaju
      Serial.println("Kecepatan kendaraan melaju");
      playBuzzer("- - - . .");
      delay(1000);
    }else if(results.value == 0xB66D){
      // Kode unik saat kecepatan kendaraan melambat
      Serial.println("Kecepatan kendaraan melambat");
      playBuzzer("- - . . -");
      delay(1000);
    }else if(results.value == 0xCA53){
      // Kode unik saat posisi berdekatan
      Serial.println("Posisi terlalu berdekatan");
      playBuzzer("- . - . -");
      delay(1000);
    }else if(results.value == 0xCC33){
      // Kode unik saat tabrakan
      Serial.println("Terjadi tabrakan dengan kendaraan");
      playBuzzer("- - - - .");
      delay(1000);
    }
    irrecv.resume();
  }
}

// Buzzer function
void playBuzzer(const char* morseSound){
  for (int i = 0; morseSound[i] != '\0'; i++){
    if(morseSound[i] == '.'){
      DotSound();
    }else if(morseSound[i] == '-'){
      DashSound();
    }else if(morseSound[i] == ' '){
      delay(200);
    }
    delay(300);
  }
}
// Bunyi dot
void DotSound(){
  tone(Buzzer,1000,dot_length);
  delay(dot_length);
}
// Bunyi dash
void DashSound(){
  tone(Buzzer,1000,dash_length);
  delay(dash_length);
}
