#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 

// Inisialisasi objek untuk koneksi I2C dengan alamat LCD yang digunakan
LiquidCrystal_I2C lcd(0x27, 16, 2);
int merah = 6;
int kuning = 5;
int hijau = 4;

void setup()
{
  //LCD SETUP
  // Inisialisasi koneksi I2C
  Wire.begin();
  // Inisialisasi LCD
  lcd.begin();
  // Mengaktifkan backlight pada LCD
  lcd.backlight();
  // Menampilkan pesan awal pada LCD
  lcd.setCursor(5, 0);
  lcd.print("JOYbot");
  delay(5000);
 
  
  //MODUL TRAFFIC LIGHT SETUP
  pinMode(merah, OUTPUT);
  pinMode(kuning, OUTPUT);
  pinMode(hijau, OUTPUT);
}

void loop()
{
  //merah
  digitalWrite(merah, HIGH);
  digitalWrite(kuning, LOW);
  digitalWrite(hijau, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MERAH");
  delay(10000);

  //kuning
  digitalWrite(merah, LOW);
  digitalWrite(kuning, HIGH);
  digitalWrite(hijau, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KUNING");
  delay(2000);

   //hijau
  digitalWrite(merah, LOW);
  digitalWrite(kuning, LOW);
  digitalWrite(hijau, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HIJAU");
  delay(8000);

  //kuning
   //kuning
   digitalWrite(merah, LOW);
  digitalWrite(kuning, HIGH);
  digitalWrite(hijau, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("KUNING");
  delay(4000);
  
  
}
