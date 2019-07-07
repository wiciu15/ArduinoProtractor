/*
Program wyswietla wartosci kata z akcelerometru na wyswietlaczu OLED w formie tekstu oraz poruszajacych sie linii.

Pozycja linii oraz napisow obliczana jest co okreslona liczbe probek na podstawie sredniej z poprzednich n-odczytow
Mozliwe jest rozszerzenie programu o filtr Kalmana lecz przeszkoda jest wyswietlacz zuzywajacy duzo czasu na rysowanie
co uniemozliwia przecyzyjne filtrowanie gdyż potrzebny jest jak najmniejszy przyrost czasu dt

Wiktor Burdecki
CC-BY 4.0
*/

#include <Wire.h> //biblioteka I2C
#include <Adafruit_GFX.h> //biblioteka do rysownaia ksztaltow
#include <Adafruit_SSD1306.h> //biblioteka do obslugi wyswietlacza(zmodyfikowana na potrzeby podrobki ekranu)
#include <MPU6050.h> //biblioteka do obslugi zyroskopu (https://github.com/jarzebski/Arduino-MPU6050)

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);  //deklaracja obiektu wyswietlacza i zyroskopu
MPU6050 mpu;

float pitch=0; //srednie wartosci katow
float roll=0;


float sumPitch=0; //suma do usrednienia
float sumRoll=0;


float onepitch=0; //pojedynczy odczyt z akcelerometru po przeliczeniu na stopnie
float oneroll=0;


float rollOffset=7; // dodajemy offset zeby na rownej powierzchnii otrzmac odczyt 0;0
float pitchOffset=-4.5;

int n=0; //zmienna liczaca probki
int probki=100;  //ilosc probek do obliczenia sredniej, im wiecej tym bardziej wygladzony
                //wynik ale tez opoznione taktowanie wyswietlacza



//////////////////////////konsola startowa////////////////////////
  void startLog(String(text), int linia){
    display.setTextSize(1);
    display.setCursor(1,linia);
    display.print(String(text));
    delay(100);
    for(int n=1;n<3;n++){   //petla dajaca obracajace sie znaki
    display.clearDisplay();
    display.setCursor(1,linia);
    display.print(String(text) + String(" /"));
    display.display();
    delay(50);
    display.clearDisplay();
    display.setCursor(1,linia);
    display.print(String(text) + String(" -"));
    display.display();
    delay(50);
    display.clearDisplay();
    display.setCursor(1,linia);
    display.print(String(text) + String(" \\")); //komplitor traktuje pojedynczy backslash jako znak specjalny, wyrzuca blad
    display.display();
    delay(50);
    display.clearDisplay();
    display.setCursor(1,linia);
    display.print(String(text) + String(" |")); 
    display.display();
    delay(50);
    }
    display.clearDisplay();
    display.setCursor(1,linia);
    display.print(String(text) + String(" ok"));
    display.display();
    delay(400);
    
  }

void setup()   { 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // inicjalizacja ekranu, adres 0x3C = 0x78 bo podrobiony ekran, potrzebna zmodyfikowana biblioteka
  display.clearDisplay(); //domyslnie pokazuje sie splash screen adafruit, czyscimy go nim sie pokaze    
  display.setTextWrap(false); //bez zawijania tekstu
  display.setTextColor(WHITE);  //bialy=piksel zapalony
  startLog("ekran",1);  //wiadomosc o pomyslnej inicjalizacji ekranu

  mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G); //inicjalizalizacja zyroskopu do 2000 stopni/s i do 2g przeciazenia
  startLog("zyroskop",1); //potwierdzenie inicjalizacji zyroskopu
  
  display.clearDisplay(); //czyscimy konsole
  
   
}

////////////////////////////////////////////////////////////////////////////////////  
/////////////////////////////glowna petla programu//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void loop() {
    
  Vector acc = mpu.readNormalizeAccel(); //odczytujemy aktualne wartosci przyspieszenia jako wektor
    
  //zamiana odczytu z akcelerometru na stopnie
  onepitch = (atan2(acc.XAxis, acc.YAxis)*180.0)/M_PI;  
  oneroll = (atan2(acc.ZAxis, sqrt(square(acc.YAxis) + square(acc.XAxis)))*180.0)/M_PI;
 

  sumPitch=sumPitch+onepitch;  //dodajemy odczyt do sumy
  sumRoll=sumRoll+oneroll;
  
  n++;   
    
  if(n==probki){  
//////////obliczenie sredniej///////////
    pitch=(sumPitch/probki)+pitchOffset; //obliczamy srednia z odczytow
    roll=(sumRoll/probki)+rollOffset;
    
    n=0;
    sumPitch=0;           //zerujemy sume i zmienna n
    sumRoll=0;
    
/////////////rysowanie na wyswietlaczu//////////    
    display.clearDisplay();
     display.drawFastHLine(0,2,128,WHITE);
    display.drawFastVLine(64,1,6,WHITE);
    display.drawFastVLine((sin(pitch*M_PI/180)*64)+63,4,6,WHITE); //sinus przy malych wartosciach szybciej sie
    display.drawFastVLine((sin(pitch*M_PI/180)*64)+64,4,6,WHITE); // zmienia przez po pozycja kreski bedzie bardziej czytelna
    display.drawFastVLine((sin(pitch*M_PI/180)*64)+65,4,6,WHITE);
    display.setTextSize(4);
    display.setCursor(28,16);   
  
    if(abs(roll)<10){  //jezeli urzadzenie jest pochylone w osi Y (ekran nie jest pionowo) nie wyświetla wyniku
    display.print(abs(pitch),1);                 
    }
  
    else{
        display.print("--.-"); 
        }
      display.display();
      
  }
 }
