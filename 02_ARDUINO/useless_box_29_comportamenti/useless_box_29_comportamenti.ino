/*
 * USELESS BOX COMPATTA - 29 comportamenti + Party Mode + personalita'
 * Porting dello sketch UselessBox.ino (17 gennaio 2026, Nano + SG90)
 * sulla box compatta: 1 servo MG996R, braccio curvo che apre lo sportello.
 *
 * COLLEGAMENTI (come useless_box_compatta.ino)
 *   filo verde D10 = segnale del solo servo MG996R
 *   filo giallo D9 = ingresso levetta
 *   D9 = levetta (COM a GND, ON a D9)     LED "L" su D13
 *   Banco col Mac: RUN tolto; Nano dal Mac; servo dal 5 V esterno.
 *   Autonomo: Mac scollegato; RUN inserito; 5 V esterno acceso.
 *
 * DIFFERENZE RISPETTO ALLO SKETCH DI GENNAIO
 *   - Levetta con INPUT_PULLUP: ON = LOW (a gennaio: pull-down, ON = HIGH).
 *   - Le posizioni non sono piu' 10..170 gradi ma PERCENTUALI della corsa
 *     fra ARM_RIPOSO (0 %) e ARM_SPINTA (100 %), misurati con calibra_braccio.
 *   - Tutte le finte si fermano al massimo ad ARM_QUASI (punta vicina alla
 *     levetta ma senza muoverla). Solo spingi() va oltre.
 *   - spingi() si ferma appena la levetta passa a OFF; se arriva a SPINTA e la
 *     levetta e' ancora ON aspetta al massimo 0,22 s e torna (niente stallo).
 *   - Se l'utente rimette OFF durante una finta o una pausa, il braccio rientra.
 *   - I comportamenti che a gennaio "spegnevano due volte" ora la seconda volta
 *     arrivano solo ad ARM_QUASI: la levetta e' gia' OFF, spingerla ancora
 *     manderebbe il servo in stallo contro il fine corsa.
 *   - Tempi scalati: ogni velocita' 1..8 dura quanto a gennaio sull'intera corsa
 *     (min 4 ms/grado per non lanciare lo sportello).
 *   - Codice segreto per il Party Mode: 3 "sfarfallate" (ON e subito OFF entro
 *     0,35 s) in meno di 3 s. A gennaio servivano 5 ON in 3 s, impossibili
 *     mentre il braccio lavora.
 *
 * CALIBRAZIONE: usa calibra_braccio.ino e annota QUATTRO angoli:
 *   RIPOSO    braccio dentro, sportello appoggiato, nessun ronzio
 *   SBIRCIATA punta appena fuori dallo sportello
 *   QUASI     ultimo angolo in cui la levetta NON si muove ancora
 *   SPINTA    levetta scattata su OFF + 2 gradi (non di piu')
 * Poi ABILITATO = true.
 */
#include <Servo.h>

const bool ABILITATO = true;
// 0 = scelta automatica; 1..29 = forza quel comportamento per il collaudo.
// Dopo le prove rimettere sempre 0.
const byte COMPORTAMENTO_TEST = 0;
const int ARM_RIPOSO    = 80;   // valori di ESEMPIO: sostituiscili con i tuoi
const int ARM_SBIRCIATA = 104;
const int ARM_QUASI     = 152;
const int ARM_SPINTA    = 158;

const byte PIN_ARM = 10;
const byte PIN_LEVETTA = 9;
const byte PIN_LED = 13;

const unsigned long ANTIRIMBALZO_MS = 30;
const unsigned long TENUTA_SPINTA_MS = 220;
const unsigned long STACCA_DOPO_MS = 600;
const unsigned long PERSONALITA_MS = 20000;   // memoria personalita' (gennaio: 20 s)
const unsigned long FINESTRA_SEGRETO_MS = 3000;
const unsigned long SFARFALLATA_MS = 350;
const byte SFARFALLATE_PER_PARTY = 3;
const byte DURATA_PARTY = 4;                  // attivazioni in Party Mode

Servo arm;
int armAt = ARM_RIPOSO;          // angolo attuale (gradi servo)
bool dopoSpinta = false;         // dopo lo spegnimento i movimenti non si annullano
int pSbirciata = 40, pQuasi = 80;

bool stabileOn = false, ultimoGrezzo = false, prontoPerOn = false;
unsigned long cambiatoAlle = 0, fermoDalle = 0, ultimaAttivazione = 0;
int attivazioni = 0;
bool partyMode = false;
byte partyRestanti = 0;
byte sfarfallate = 0;
unsigned long primaSfarfallata = 0;

// ------------------------------------------------------------ utilita'
bool levettaOn() { return digitalRead(PIN_LEVETTA) == LOW; }

int corsa() { return abs(ARM_SPINTA - ARM_RIPOSO); }

int percento(int angolo) {
  return (long)(angolo - ARM_RIPOSO) * 100 / (ARM_SPINTA - ARM_RIPOSO);
}

int angolo(int pct) {
  return ARM_RIPOSO + (long)(ARM_SPINTA - ARM_RIPOSO) * pct / 100;
}

bool calibrazioneValida() {
  const bool cresc = ARM_RIPOSO < ARM_SBIRCIATA && ARM_SBIRCIATA < ARM_QUASI && ARM_QUASI < ARM_SPINTA;
  const bool decr  = ARM_RIPOSO > ARM_SBIRCIATA && ARM_SBIRCIATA > ARM_QUASI && ARM_QUASI > ARM_SPINTA;
  return (cresc || decr) && corsa() >= 45 && corsa() <= 110 &&
         ARM_RIPOSO >= 5 && ARM_RIPOSO <= 175 && ARM_SPINTA >= 5 && ARM_SPINTA <= 175;
}

void collega() {
  if (!arm.attached()) { arm.write(armAt); arm.attach(PIN_ARM); }
}

// Velocita' 1..8 come a gennaio: stessa DURATA sull'intera corsa.
unsigned int msPerGrado(byte vel) {
  unsigned int durata = vel <= 2 ? 6400 : vel <= 4 ? 1600 : vel <= 6 ? 533 : 160;
  unsigned int ms = durata / corsa();
  return ms < 4 ? 4 : ms;
}

// Pausa che si interrompe se l'utente rimette OFF (prima della spinta).
bool pausa(unsigned long ms) {
  const unsigned long t0 = millis();
  while (millis() - t0 < ms) {
    if (!dopoSpinta && !levettaOn()) return true;
    delay(2);
  }
  return false;
}

// Va alla percentuale pct (limitata ad ARM_QUASI). true = annullato.
bool vai(int pct, byte vel) {
  pct = constrain(pct, 0, pQuasi);
  const int target = angolo(pct);
  const unsigned int ms = msPerGrado(vel);
  collega();
  const int dir = target > armAt ? 1 : -1;
  while (armAt != target) {
    armAt += dir;
    arm.write(armAt);
    delay(ms);
    if (!dopoSpinta && !levettaOn()) return true;
  }
  return false;
}

// Scatto immediato (come myServo.write di gennaio), limitato ad ARM_QUASI.
bool scatta(int pct, unsigned long attesa) {
  pct = constrain(pct, 0, pQuasi);
  collega();
  armAt = angolo(pct);
  arm.write(armAt);
  return pausa(attesa);
}

// Spinta vera: si ferma appena la levetta va su OFF.
void spingi(byte vel) {
  const unsigned int ms = msPerGrado(vel);
  collega();
  const int dir = ARM_SPINTA > armAt ? 1 : -1;
  while (armAt != ARM_SPINTA && levettaOn()) {
    armAt += dir;
    arm.write(armAt);
    delay(ms);
  }
  const unsigned long t0 = millis();
  while (levettaOn() && millis() - t0 < TENUTA_SPINTA_MS) delay(5);
  if (levettaOn()) {
    digitalWrite(PIN_LED, HIGH);
    Serial.println(F("Levetta ancora ON: aumenta ARM_SPINTA di 2 gradi."));
    // si stacca subito dalla levetta: niente stallo nel resto del comportamento
    const int dq = angolo(pQuasi) > armAt ? 1 : -1;
    while (armAt != angolo(pQuasi)) { armAt += dq; arm.write(armAt); delay(4); }
  } else {
    digitalWrite(PIN_LED, LOW);
  }
  dopoSpinta = true;
}

void aRiposo(byte vel) {
  const unsigned int ms = msPerGrado(vel);
  collega();
  const int dir = ARM_RIPOSO > armAt ? 1 : -1;
  while (armAt != ARM_RIPOSO) { armAt += dir; arm.write(armAt); delay(ms); }
  fermoDalle = millis();
}

void lampeggia(byte volte) {
  for (byte i = 0; i < volte; i++) {
    digitalWrite(PIN_LED, HIGH); delay(200);
    digitalWrite(PIN_LED, LOW);  delay(200);
  }
}

// Se una mossa viene annullata (utente su OFF) il comportamento finisce;
// il rientro lo fa sempre esegui().
#define V(p, v)   do { if (vai((p), (v))) return; } while (0)
#define S(p, ms)  do { if (scatta((p), (ms))) return; } while (0)
#define W(ms)     do { if (pausa(ms)) return; } while (0)
#define SPINGI(v) spingi(v)

// Punti della corsa (percentuali). A gennaio: INSIDE=10, OUTSIDE=170.
const int P_BUSSA = 18;   // alza appena lo sportello (bussa da sotto)
#define P_SBIRCIA pSbirciata
#define P_QUASI   pQuasi

// ------------------------------------------------------------ 29 comportamenti
void c01_Normale()        { V(0,5); SPINGI(5); W(300); }
void c02_Lento()          { SPINGI(2); W(300); }
void c03_Veloce()         { SPINGI(8); W(200); }
void c04_VelocitaVariabile() { V(50,3); SPINGI(7); W(300); }
void c05_FintaSingola()   { V(20,5); W(500); SPINGI(5); W(300); }
void c06_DoppiaFinta()    { V(15,5); W(400); V(30,5); W(400); SPINGI(5); W(300); }
void c07_FintaConRitorno(){ V(38,5); W(300); V(25,4); W(200); SPINGI(6); W(300); }
void c08_Cucu()           { V(P_SBIRCIA,4); W(400); V(0,6); W(1000); SPINGI(7); W(300); }
void c09_Tremante() {
  int p = 0;
  while (p < P_QUASI - 5) {
    p += 5;
    S(p, 50); S(p - 2, 50); S(p + 2, 50);
  }
  SPINGI(8); W(300);
}
void c10_PigroEstremo()   { W(random(3000, 5000)); SPINGI(8); W(200); }
void c11_DoppioSpegnimento() {
  SPINGI(6); W(300); V(0,6); W(500);
  V(P_QUASI,7); W(300);             // seconda volta: controlla soltanto
}
void c12_Indeciso()       { V(P_QUASI,5); W(800); V(0,5); W(2000); SPINGI(6); W(300); }
void c13_ToccToc() {
  for (byte i = 0; i < 3; i++) { S(P_BUSSA, 150); S(0, 150); }
  W(500); SPINGI(6); W(300);
}
void c14_Balletto() {
  for (byte i = 0; i < 4; i++) { V(22,6); V(34,6); }
  SPINGI(6); W(300);
}
void c15_Beffa()          { V(P_QUASI,8); W(800); SPINGI(2); W(300); }
void c16_SbattiSportello() {
  SPINGI(6); W(300);
  for (byte i = 0; i < 3; i++) { V(25,8); W(100); V(P_QUASI - 10,8); W(100); }
}
void c17_Sbirciatina()    { V(P_QUASI - 5,1); W(500); SPINGI(8); W(300); }
void c18_ColpiMultipli() {
  SPINGI(6); W(300); V(50,5); W(500);
  V(P_QUASI,6); W(300);             // a gennaio ripremeva: qui solo controllo
}
void c19_Confuso()        { V(P_QUASI,5); W(2000); V(P_QUASI - 15,4); W(800); SPINGI(5); W(300); }
void c20_Sbadiglio()      { V(50,3); W(3000); SPINGI(1); W(300); }
void c21_Impaziente() {
  SPINGI(8); W(150);
  for (byte i = 0; i < 2; i++) { S(P_QUASI - 10, 80); S(P_QUASI, 80); }
}
void c22_Stanco()         { SPINGI(1); W(1000); }
void c23_FalsePartenze() {
  for (byte i = 0; i < 2; i++) { V(31,5); W(300); V(0,5); W(400); }
  SPINGI(6); W(300);
}
void c24_Caos() {
  for (byte i = 0; i < 8; i++) { V(random(0, P_QUASI), random(3, 8)); W(random(100, 500)); }
  SPINGI(6); W(300);
}
void c25_Furtivo()        { SPINGI(1); W(500); }
void c26_Collaudo()       { V(P_QUASI,5); W(1000); V(0,5); W(1500); SPINGI(6); W(300); }
void c27_PartenzaPigra()  { W(5000); SPINGI(8); W(200); }
void c28_Balbettante() {
  int p = 0;
  while (p + 6 < P_QUASI) { p += 6; V(p,5); W(200); }
  SPINGI(5); W(300);
}
void c29_Ciao() {
  SPINGI(6); W(500);
  for (byte i = 0; i < 3; i++) { S(P_QUASI - 10, 200); S(P_QUASI, 200); }
  W(300);
}

void esegui(int quale) {
  Serial.print(F("Comportamento ")); Serial.println(quale);
  dopoSpinta = false;
  switch (quale) {
    case 1: c01_Normale(); break;          case 2: c02_Lento(); break;
    case 3: c03_Veloce(); break;           case 4: c04_VelocitaVariabile(); break;
    case 5: c05_FintaSingola(); break;     case 6: c06_DoppiaFinta(); break;
    case 7: c07_FintaConRitorno(); break;  case 8: c08_Cucu(); break;
    case 9: c09_Tremante(); break;         case 10: c10_PigroEstremo(); break;
    case 11: c11_DoppioSpegnimento(); break; case 12: c12_Indeciso(); break;
    case 13: c13_ToccToc(); break;         case 14: c14_Balletto(); break;
    case 15: c15_Beffa(); break;           case 16: c16_SbattiSportello(); break;
    case 17: c17_Sbirciatina(); break;     case 18: c18_ColpiMultipli(); break;
    case 19: c19_Confuso(); break;         case 20: c20_Sbadiglio(); break;
    case 21: c21_Impaziente(); break;      case 22: c22_Stanco(); break;
    case 23: c23_FalsePartenze(); break;   case 24: c24_Caos(); break;
    case 25: c25_Furtivo(); break;         case 26: c26_Collaudo(); break;
    case 27: c27_PartenzaPigra(); break;   case 28: c28_Balbettante(); break;
    case 29: c29_Ciao(); break;
    default: c01_Normale(); break;
  }
  aRiposo(5);   // qualunque cosa sia successa, rientra e lo sportello ricade
  dopoSpinta = false;
}

// ------------------------------------------------------------ personalita' (come a gennaio)
int scegliComportamento() {
  const int tiro = random(100);
  const int bias = attivazioni >= 10 ? 20 : attivazioni >= 5 ? 10 : 0;
  if (tiro < 60 - bias) return random(1, 5);          // 1-4   normali
  if (tiro < 80 - bias) return random(5, 11);         // 5-10  finte
  if (tiro < 95 + bias / 2) return random(11, 21);    // 11-20 teatrali
  return random(21, 30);                              // 21-29 strani
}

int scegliParty() {
  const int tiro = random(100);
  if (tiro < 50) return random(11, 21);
  if (tiro < 80) return random(21, 30);
  return random(5, 11);
}

void attivaParty() {
  partyMode = true;
  partyRestanti = DURATA_PARTY;
  Serial.println(F("PARTY MODE!"));
  lampeggia(3);
}

// Dopo un ON aspetta SFARFALLATA_MS: se l'utente rimette subito OFF e' una
// "sfarfallata" (conta per il codice segreto) e il braccio non parte.
bool eraUnaSfarfallata() {
  const unsigned long t0 = millis();
  while (millis() - t0 < SFARFALLATA_MS) {
    if (!levettaOn()) {
      const unsigned long ora = millis();
      if (ora - primaSfarfallata > FINESTRA_SEGRETO_MS) { sfarfallate = 1; primaSfarfallata = ora; }
      else sfarfallate++;
      if (sfarfallate >= SFARFALLATE_PER_PARTY) { sfarfallate = 0; attivaParty(); }
      return true;
    }
    delay(5);
  }
  return false;
}

void gestisciAttivazione() {
  if (eraUnaSfarfallata()) return;
  ultimaAttivazione = millis();
  attivazioni++;
  int quale;
  if (COMPORTAMENTO_TEST >= 1 && COMPORTAMENTO_TEST <= 29) {
    quale = COMPORTAMENTO_TEST;
  } else if (partyMode) {
    quale = scegliParty();
    if (--partyRestanti == 0) { partyMode = false; lampeggia(2); }
  } else {
    quale = scegliComportamento();
  }
  esegui(quale);
}

// ------------------------------------------------------------ setup / loop
void setup() {
  pinMode(PIN_LEVETTA, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(115200);
  ultimoGrezzo = levettaOn();
  stabileOn = ultimoGrezzo;
  prontoPerOn = !stabileOn;        // accesa con levetta su ON: aspetta un OFF
  cambiatoAlle = millis();
  if (!ABILITATO || !calibrazioneValida()) {
    Serial.println(F("FERMO: scrivi i 4 angoli, poi ABILITATO = true."));
    digitalWrite(PIN_LED, HIGH);
    return;
  }
  pSbirciata = percento(ARM_SBIRCIATA);
  pQuasi = percento(ARM_QUASI);
  randomSeed(analogRead(A0));
  arm.write(ARM_RIPOSO);
  arm.attach(PIN_ARM);
  delay(400);
  fermoDalle = millis();
  lampeggia(3);                    // come a gennaio: tre lampi = pronta
  Serial.println(F("Servo sul filo verde D10; levetta su D9; RUN tolto col Mac."));
  Serial.println(F("Useless box compatta: 29 comportamenti pronti."));
}

void loop() {
  if (!ABILITATO || !calibrazioneValida()) return;
  const bool g = levettaOn();
  if (g != ultimoGrezzo) { ultimoGrezzo = g; cambiatoAlle = millis(); }
  if (millis() - cambiatoAlle >= ANTIRIMBALZO_MS) stabileOn = g;
  if (!stabileOn) prontoPerOn = true;

  if (stabileOn && prontoPerOn) {
    prontoPerOn = false;
    gestisciAttivazione();
  }
  if (millis() - ultimaAttivazione > PERSONALITA_MS) attivazioni = 0;
  if (arm.attached() && armAt == ARM_RIPOSO && millis() - fermoDalle > STACCA_DOPO_MS) arm.detach();
}
