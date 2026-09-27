// Useless box compatta - PROVA LEVETTA (prima di collegare il servo)
// RUN tolto: il Nano e' alimentato solo dal cavo USB-C del computer.
// Levetta: COM -> GND, contatto scelto -> D9, terzo piedino libero.
// Obiettivo: con la leva inclinata verso lo SPORTELLO (verso il retro)
// il monitor seriale deve scrivere ON e il LED "L" deve accendersi.
// Se succede il contrario, sposta il filo di D9 sull'altro piedino esterno.
const byte PIN_LEVETTA = 9;
const byte PIN_LED = 13;
bool ultimoStato = false;

void setup() {
  pinMode(PIN_LEVETTA, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("Prova levetta: leva verso lo sportello = ON"));
  ultimoStato = digitalRead(PIN_LEVETTA) == LOW;
  digitalWrite(PIN_LED, ultimoStato ? HIGH : LOW);
  Serial.println(ultimoStato ? F("ON") : F("OFF"));
}

void loop() {
  const bool stato = digitalRead(PIN_LEVETTA) == LOW;
  if (stato != ultimoStato) {
    delay(35);
    if ((digitalRead(PIN_LEVETTA) == LOW) != stato) return;
    ultimoStato = stato;
    digitalWrite(PIN_LED, stato ? HIGH : LOW);
    Serial.println(stato ? F("ON") : F("OFF"));
  }
}
