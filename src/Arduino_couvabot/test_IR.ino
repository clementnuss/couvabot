// Define pins
#define ENABLE_IR_PIN 38
#define N_IR_SENSORS   8

int i;
int IR_sensor[N_IR_SENSORS];

void setup()
{
  pinMode(0, INPUT);
  pinMode(ENABLE_IR_PIN, OUTPUT);
  Serial.begin(9600);

  // Enable IR sensors
  digitalWrite(ENABLE_IR_PIN, 1);

  Serial.println("Program Starting");
  // wait for the long string to be sent
  delay(100);
}

void loop()
{
  // Read sensors
  for (i = 0; i < 8; i++) {
    IR_sensor[i] = analogRead(i);
    delay(10);
  }

  // Display the results
  Serial.println("IR sensors values:");
  for (i = 0; i < 8; i++) {
    Serial.print("\t");
    Serial.print("Pin");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(IR_sensor[i]);
  }
  Serial.println();

  // Wait a bit
  delay(10); //wait for the string to be sent
}
