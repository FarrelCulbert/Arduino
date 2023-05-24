// Pin yang digunakan untuk menghubungkan raindrop modul
int raindropPin = A0;

void setup() {
  // Menginisialisasi Serial Monitor
  Serial.begin(9600);
}

void loop() {
  // Membaca nilai analog dari modul raindrop
  int raindropValue = analogRead(raindropPin);

  // Menampilkan nilai bacaan pada Serial Monitor
  Serial.print("Nilai Raindrop: ");
  Serial.println(raindropValue);

  // Menentukan status hujan berdasarkan nilai bacaan
  if (raindropValue < 500) {
    Serial.println("Hujan");
  } else {
    Serial.println("Tidak Hujan");
  }

  // Jeda sejenak sebelum membaca nilai berikutnya
  delay(500);
}
