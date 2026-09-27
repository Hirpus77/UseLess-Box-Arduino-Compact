# Useless box compatta v1.5 - INIZIA QUI

Questa cartella contiene una sola copia di ogni file corrente.

## Ordine consigliato

1. Leggi `01_GUIDA/Guida_useless_box_compatta_v1.5_D9_D10.pdf`.
2. Stampa soltanto i nove file presenti in `03_STL_STAMPA`.
3. Esegui prima `02_ARDUINO/prova_levetta/prova_levetta.ino`.
4. Esegui `02_ARDUINO/calibra_braccio/calibra_braccio.ino` e annota i quattro angoli.
5. Inserisci gli angoli in `02_ARDUINO/useless_box_29_comportamenti/useless_box_29_comportamenti.ino`.
6. Metti `ABILITATO = true`, prova i comportamenti 1, 8 e 26, poi rimetti `COMPORTAMENTO_TEST = 0`.

## Pin definitivi

- D10 / verde: segnale servo.
- D9 / giallo: levetta.
- blu GND: centrale COM della levetta.
- rosso 5V: giunto RUN e distributore +5 V.
- nero GND: distributore GND.
- D2: libero.

Con il Mac collegato RUN deve essere tolto. Il servo si muove soltanto con il 5 V esterno collegato e acceso.
