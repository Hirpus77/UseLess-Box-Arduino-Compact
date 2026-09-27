// Useless box compatta - un solo MG996R: il braccio apre lo sportello
// spingendolo da sotto e poi abbatte la levetta.
// Cablaggio definitivo: verde D10 = segnale servo; giallo D9 = levetta.
// D9 = levetta (COM a GND, ON a D9), LED "L" su D13.
// Col Mac: RUN tolto e 5 V esterno acceso quando il servo deve muoversi.
// 1) Esegui calibra_braccio.ino e scrivi qui i tre angoli misurati.
// 2) Metti ABILITATO = true e carica. Poi spegni il 5 V esterno, scollega il Mac,
//    inserisci RUN e riaccendi il 5 V esterno.
#include <Servo.h>

const bool ABILITATO = false;
const int ARM_RIPOSO    = 90;   // braccio dentro, sportello chiuso, niente ronzio
const int ARM_SBIRCIATA = 125;  // punta appena fuori (valore di esempio)
const int ARM_SPINTA    = 160;  // levetta scattata + 2..3 gradi (valore di esempio)

const byte PIN_ARM = 10;
const byte PIN_LEVETTA = 9;
const byte PIN_LED = 13;
const unsigned long ANTIRIMBALZO_MS = 30;
const unsigned long TENUTA_SPINTA_MS = 220;   // tempo massimo contro la levetta
const unsigned long STACCA_DOPO_MS = 600;     // poi il servo viene "staccato"

Servo arm;
int armAt = ARM_RIPOSO;
bool ultimoGrezzo = false, stabileOn = false, prontoPerOn = false;
unsigned long cambiatoAlle = 0, fermoDalle = 0;
byte comportamento = 0;

bool calibrazioneValida() {
  const bool crescente   = ARM_RIPOSO < ARM_SBIRCIATA && ARM_SBIRCIATA < ARM_SPINTA;
  const bool decrescente = ARM_RIPOSO > ARM_SBIRCIATA && ARM_SBIRCIATA > ARM_SPINTA;
  const int corsa = abs(ARM_SPINTA - ARM_RIPOSO);
  return (crescente || decrescente) && corsa >= 45 && corsa <= 110 &&
         ARM_RIPOSO >= 5 && ARM_RIPOSO <= 175 && ARM_SPINTA >= 5 && ARM_SPINTA <= 175;
}

bool levettaOn() { return digitalRead(PIN_LEVETTA) == LOW; }

void aggiornaLevetta() {
  const bool g = levettaOn();
  if (g != ultimoGrezzo) { ultimoGrezzo = g; cambiatoAlle = millis(); }
  if (millis() - cambiatoAlle >= ANTIRIMBALZO_MS) stabileOn = g;
  if (!stabileOn) prontoPerOn = true;
}

void collega() {
  if (!arm.attached()) { arm.write(armAt); arm.attach(PIN_ARM); }
}

// Muove a passi di 1 grado. msGrado piccolo = veloce.
// Se durante l'andata l'utente rimette OFF, si ferma (ritorna true).
bool muovi(int target, unsigned int msGrado, bool fermatiSeOff) {
  collega();
  const int dir = target > armAt ? 1 : -1;
  while (armAt != target) {
    armAt += dir;
    arm.write(armAt);
    delay(msGrado);
    if (fermatiSeOff && !levettaOn()) return true;
  }
  return false;
}

// Frazione della corsa fra riposo e spinta (0..100 %)
int angoloA(int percento) {
  return ARM_RIPOSO + (long)(ARM_SPINTA - ARM_RIPOSO) * percento / 100;
}

void aRiposo(unsigned int msGrado) {
  muovi(ARM_RIPOSO, msGrado, false);
  fermoDalle = millis();
}

void spingi(unsigned int msGrado) {
  if (muovi(ARM_SPINTA, msGrado, true)) { aRiposo(6); return; }
  const unsigned long t0 = millis();
  while (levettaOn() && millis() - t0 < TENUTA_SPINTA_MS) delay(5);
  if (levettaOn()) {
    // non ha abbattuto la levetta: non insiste, segnala e torna dentro
    digitalWrite(PIN_LED, HIGH);
    Serial.println(F("Levetta ancora ON: aumenta ARM_SPINTA di 2 gradi."));
  } else {
    digitalWrite(PIN_LED, LOW);
  }
  aRiposo(5);
}

void comporta(byte quale) {
  Serial.print(F("Comportamento ")); Serial.println(quale + 1);
  switch (quale) {
    case 0:                                   // deciso
      spingi(4);
      break;
    case 1:                                   // pigro: aspetta, poi lento
      delay(1500);
      if (!levettaOn()) return;
      spingi(12);
      break;
    case 2:                                   // sbircia, rientra, poi colpisce
      if (muovi(ARM_SBIRCIATA, 8, true)) { aRiposo(6); return; }
      delay(900);
      muovi(angoloA(10), 8, false);
      delay(700);
      if (!levettaOn()) { aRiposo(6); return; }
      spingi(3);
      break;
    case 3:                                   // esitante: si avvicina e trema
      if (muovi(angoloA(80), 7, true)) { aRiposo(6); return; }
      for (byte i = 0; i < 3; i++) {
        muovi(angoloA(70), 6, false);
        muovi(angoloA(80), 6, false);
      }
      delay(300);
      spingi(8);
      break;
    default:                                  // doppio colpo di sportello
      muovi(angoloA(45), 4, false); muovi(angoloA(15), 4, false);
      muovi(angoloA(45), 4, false); muovi(angoloA(15), 4, false);
      delay(400);
      if (!levettaOn()) { aRiposo(6); return; }
      spingi(4);
      break;
  }
}

void setup() {
  pinMode(PIN_LEVETTA, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  ultimoGrezzo = levettaOn(); stabileOn = ultimoGrezzo;
  prontoPerOn = !stabileOn;   // acceso con levetta su ON: aspetta un OFF
  cambiatoAlle = millis();
  if (!ABILITATO || !calibrazioneValida()) {
    Serial.println(F("FERMO: scrivi i 3 angoli, poi ABILITATO = true."));
    digitalWrite(PIN_LED, HIGH);
    return;
  }
  randomSeed(analogRead(A0));
  arm.write(ARM_RIPOSO);
  arm.attach(PIN_ARM);
  delay(400);
  fermoDalle = millis();
  Serial.println(F("Useless box compatta pronta."));
}

void loop() {
  if (!ABILITATO || !calibrazioneValida()) return;
  aggiornaLevetta();
  if (stabileOn && prontoPerOn) {
    prontoPerOn = false;
    comporta(comportamento);
    comportamento = (comportamento + 1 + random(0, 2)) % 5;
  }
  // a riposo il servo viene staccato: niente ronzio e meno corrente
  if (arm.attached() && armAt == ARM_RIPOSO && millis() - fermoDalle > STACCA_DOPO_MS) arm.detach();
}
