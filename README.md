# Useless Box Arduino Compact (v1.5)

Useless box compatta con Arduino Nano e un solo servo: levetta su **D9**, servo su **D10**, 29 comportamenti più Party Mode.

Per iniziare leggi [`INIZIA_QUI.md`](INIZIA_QUI.md) e la guida PDF in [`01_GUIDA/`](01_GUIDA/).

## Contenuto

| Cartella | Contenuto |
|---|---|
| `01_GUIDA/` | Guida PDF v1.5, istruzioni di cablaggio e posizionamento, verifiche STL |
| `02_ARDUINO/` | Sketch: prova levetta, calibrazione braccio, programma a 29 comportamenti, programma base opzionale |
| `03_STL_STAMPA/` | I nove pezzi da stampare in 3D |
| `04_STEP_MODIFICABILI/` | Gli stessi pezzi in formato STEP, modificabili |
| `05_IMMAGINI/` | Schemi di cablaggio, quote e montaggio |

`SHA256SUMS.txt` permette di verificare l'integrità dei file: `sha256sum -c SHA256SUMS.txt`.

## Ordine di lavoro

1. Stampa i nove file di `03_STL_STAMPA`.
2. Carica `prova_levetta.ino` per verificare la levetta su D9.
3. Carica `calibra_braccio.ino` e annota i quattro angoli.
4. Inserisci gli angoli in `useless_box_29_comportamenti.ino` e caricalo.

Con il computer collegato via USB il ponticello RUN va tolto; il servo si muove solo con l'alimentazione esterna a 5 V accesa.

## Licenza

Il progetto usa licenze diverse per tipo di contenuto:

| Contenuto | Licenza | File |
|---|---|---|
| Codice (`02_ARDUINO/`) | MIT | [`LICENSE`](LICENSE) |
| Hardware (`03_STL_STAMPA/`, `04_STEP_MODIFICABILI/`) | CERN-OHL-P v2 | [`LICENSE-hardware`](LICENSE-hardware) |
| Documentazione e immagini (`01_GUIDA/`, `05_IMMAGINI/`, file di testo) | CC BY-SA 4.0 | [`LICENSE-docs`](LICENSE-docs) |

© 2026 Hirpus77
