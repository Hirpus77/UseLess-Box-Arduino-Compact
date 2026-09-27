# Cablaggio definitivo con distributori da 5 poli e giunto da 2 poli

Questa revisione usa i componenti già acquistati:

- due distributori a leva da 5 poli;
- due giunti a leva da 2 poli;
- i cinque fili già saldati alla basetta A;
- la slitta C10 e la basetta 50 × 34 mm già previste dal progetto.

D2 resta libero. Non servono una seconda piastra, nuovi fori o modifiche alla scatola.

## 1. Mappa dei cinque fili

| Colore | Pin Nano | Destinazione |
|---|---|---|
| Verde | D10 | Giunto da 2 poli, poi segnale `S` del servo |
| Giallo | D9 | Piedino laterale `ON` della levetta |
| Blu | GND | Piedino centrale `COM` della levetta |
| Rosso | 5V | RUN, poi distributore `+5 V` da 5 poli |
| Nero | GND | Distributore GND da 5 poli |

Tutti i programmi leggono la levetta da D9 con `INPUT_PULLUP`.

## 2. Levetta

- centrale `COM`: blu GND;
- laterale che chiude il contatto nella posizione ON: giallo D9;
- altro laterale: libero e isolato.

Se ON e OFF risultano invertiti, spostare soltanto il giallo sull'altro piedino laterale.

## 3. Servo

Al servo arrivano esattamente tre collegamenti:

1. verde D10 attraverso il giunto da 2 poli al segnale `S`;
2. rosso dal distributore +5 V al rosso centrale del servo;
3. nero dal distributore GND al nero o marrone del servo.

## 4. Distributore +5 V da 5 poli

Montarlo sulla faccia della basetta rivolta verso il centro della scatola. Usare quattro sedi:

1. positivo dell'alimentatore esterno;
2. positivo del servo;
3. positivo del condensatore;
4. uscita di RUN.

La quinta sede resta libera. L'altro lato di RUN va al filo rosso 5V del Nano.

## 5. Distributore GND da 5 poli

Montarlo sulla faccia opposta della stessa basetta, rivolta verso la parete destra. Usare quattro sedi:

1. negativo dell'alimentatore esterno;
2. GND nero o marrone del servo;
3. negativo del condensatore, identificato dalla fascia;
4. filo nero GND del Nano.

La quinta sede resta libera. Il blu non entra nel distributore: va direttamente al COM della levetta.

## 6. Giunti da 2 poli

- giunto D10, prima sede: verde proveniente da D10;
- giunto D10, seconda sede: filo del contatto `S` del servo;
- giunto RUN, prima sede: rosso proveniente dal 5V del Nano;
- giunto RUN, seconda sede: ingresso del distributore +5 V.

I due giunti restano su cavetti corti fissati con una fascetta al bordo superiore della C10. Nel giunto D10 non collegare 5 V o GND.

## 7. Preparazione della basetta C10

Questa operazione si esegue fuori dalla scatola.

1. Verificare che ciascun distributore non superi 30 × 19 × 10 mm.
2. Posizionare i due distributori schiena contro schiena, nella zona centrale della basetta.
3. Mettere il +5 V sulla faccia che guarderà il centro della scatola.
4. Mettere il GND sulla faccia che guarderà la parete destra.
5. Fissarli insieme alla basetta con una fascetta sottile. Non affidarsi soltanto al biadesivo.
6. Fissare il condensatore orizzontale nella fascia superiore della faccia centrale.
7. Lasciare RUN e giunto D10 su cavetti corti, legati al bordo superiore e raggiungibili dopo aver tolto il coperchio.
8. Cablare completamente il distributore GND sul lato parete: dopo il montaggio non sarà comodo aprire le leve.
9. Inserire la basetta nelle guide C10 con i componenti orientati come indicato nello schema.
10. Usare le fascette previste dalla C10 senza stringere fino a deformare la basetta.
11. Fissare la C10 nei due fori esistenti con le due M3 × 8 previste.

Le quote esatte sono nelle tavole `04_piastra_C10_quote_mm.png` e
`05_ingombri_C10_scatola_quote_mm.png`.

## 8. Posizioni nella scatola

- C09: basetta A e Nano;
- C10: basetta 50 × 34 mm con i due distributori sulle facce opposte;
- C05: servo;
- C06: ingresso del cavo 5 V bloccato con due fascette;
- coperchio: levetta.

Non stampare alcuna C11: è stata eliminata dopo la verifica di collisione.

## 9. Percorso dei cavi

1. Dalla C06 portare rosso e nero lungo la parete destra fino alla C10.
2. Portare verde, rosso e nero dalla C10 verso il servo restando aderenti al fondo.
3. Portare rosso e nero del Nano dalla C09 ai due distributori.
4. Portare giallo D9 e blu GND direttamente alla levetta.
5. Lasciare un arco di servizio per sollevare il coperchio.
6. Nessun filo deve attraversare la zona di movimento del braccio.

## 10. Controlli senza alimentazione

1. +5 V e GND non devono essere in continuità.
2. Nero Nano, negativo alimentatore e GND servo devono essere in continuità.
3. Verde D10 e segnale `S` del servo devono essere in continuità attraverso il giunto da 2 poli.
4. Blu GND e COM della levetta devono essere in continuità.
5. Levetta ON: giallo D9 e blu GND devono essere in continuità.
6. Levetta OFF: giallo D9 e blu GND devono essere separati.
7. D2 deve risultare scollegato.
8. RUN tolto: il 5V del Nano deve essere separato dal +5 V esterno.
9. RUN inserito: il 5V del Nano deve essere collegato al +5 V esterno.

## 11. Prova con il Mac

1. Spegnere il 5 V esterno.
2. Togliere RUN.
3. Collegare il Nano al Mac.
4. Caricare `prova_levetta.ino` e aprire il monitor seriale a 115200 baud.
5. Verificare ON e OFF.
6. Caricare `calibra_braccio.ino`.
7. Collegare il servo a impianto spento.
8. Accendere il 5 V esterno per muovere il servo.

Durante la calibrazione: Mac collegato, RUN tolto, alimentatore esterno acceso.

## 12. Uso autonomo

1. Spegnere il 5 V esterno.
2. Scollegare la USB-C dal Mac.
3. Inserire RUN.
4. Accendere il 5 V esterno.

## 13. Pin usati dal codice

```cpp
const byte PIN_ARM = 10;
const byte PIN_LEVETTA = 9;
```
