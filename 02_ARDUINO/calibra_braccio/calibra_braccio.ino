// Useless box compatta - CALIBRAZIONE DEL BRACCIO
// Un solo MG996R: filo verde D10 collegato al segnale del servo.
// Il filo giallo gia saldato a D9 viene usato come ingresso della levetta.
// Alimentazione sul banco: RUN TOLTO, Nano dal computer, servo dal 5 V esterno.
// Senza il 5 V esterno il servo non si muove, anche se il Nano e' collegato al Mac.
// Monitor seriale a 115200, "Nessun fine riga" oppure "A capo": va bene uguale.
//
// PROCEDURA
// 1. Carica questo sketch con il braccio NON montato: il servo va a 90 gradi.
// 2. Spegni il 5 V esterno. Monta squadretta + C04 in posizione approssimativa
//    di RIPOSO e stringi le viti fuori dalla scatola. Poi installa il gruppo.
// 3. Premi '+' tre volte (6 gradi). Se la punta SALE verso lo sportello il verso
//    e' crescente; se SCENDE premi subito '-' e usa i gradi decrescenti.
// 4. Annota QUATTRO angoli per lo sketch con 29 comportamenti:
//    RIPOSO (braccio dentro, sportello appoggiato, nessun ronzio),
//    SBIRCIATA (punta appena visibile), QUASI (vicino alla levetta senza muoverla),
//    SPINTA (primo angolo che porta la levetta su OFF + 2 gradi, non di piu').
//
// Comandi: '+' / '-' = 2 gradi, '>' / '<' = 1 grado, 'r' = ritorno lento a 90,
//          's' = stacca il segnale (servo libero), 'p' = stampa, 'l' = leggi levetta.
#include <Servo.h>
const byte PIN_ARM = 10;  // verde D10 al segnale servo; D9 e usato dalla levetta
const byte PIN_LEVETTA = 9;
const int MIN_A = 10, MAX_A = 170, START = 80;
Servo arm;
int angle = START;

void stampa() {
  Serial.print(F("Angolo: "));
  Serial.print(angle);
  Serial.print(F("   levetta: "));
  Serial.println(digitalRead(PIN_LEVETTA) == LOW ? F("ON") : F("OFF"));
}

void vaiA(int target) {
  target = constrain(target, MIN_A, MAX_A);
  if (!arm.attached()) { arm.write(angle); arm.attach(PIN_ARM); }
  const int dir = target > angle ? 1 : -1;
  while (angle != target) { angle += dir; arm.write(angle); delay(15); }
  stampa();
}

void setup() {
  pinMode(PIN_LEVETTA, INPUT_PULLUP);
  Serial.begin(115200);
  arm.write(START);
  arm.attach(PIN_ARM);
  delay(500);
  Serial.println(F("Servo sul filo verde D10. Levetta sul filo giallo D9. RUN tolto."));
  Serial.println(F("Calibrazione braccio. + - > < r s p l"));
  stampa();
}

void loop() {
  if (!Serial.available()) return;
  const char c = Serial.read();
  switch (c) {
    case '+': vaiA(angle + 2); break;
    case '-': vaiA(angle - 2); break;
    case '>': vaiA(angle + 1); break;
    case '<': vaiA(angle - 1); break;
    case 'r': vaiA(START); break;
    case 's': arm.detach(); Serial.println(F("Segnale staccato: il servo e' libero.")); break;
    case 'p': case 'l': stampa(); break;
    default: break;
  }
}
