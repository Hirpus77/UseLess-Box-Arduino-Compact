# Verifica STL e ingombri del montaggio elettrico

## File verificati nell'assieme

- C01 corpo finale v1.7, interno 63 × 152 × 50 mm;
- C05 staffa servo;
- C09 slitta basetta A;
- C10 slitta verticale rinforzata V2;
- basetta 50 × 34 mm inserita nella C10;
- due distributori da 5 poli modellati con ingombro conservativo massimo 30 × 19 × 10 mm;
- condensatore modellato con ingombro 20 × 10 × 10 mm.

## Posizione verificata

I due distributori vengono fissati schiena contro schiena sulle due facce della stessa basetta 50 × 34 mm:

- distributore +5 V sulla faccia rivolta verso il centro della scatola;
- distributore GND sulla faccia rivolta verso la parete destra;
- ingombro lungo y: 81,5–111,5 mm;
- ingombro in altezza: z 8–27 mm;
- condensatore sulla fascia superiore della faccia centrale: z 29–39 mm.

La faccia rivolta verso la parete deve essere cablata prima di inserire la basetta nella C10.

## Risultati delle intersezioni CAD

Volume di intersezione con C01, C05, C09 e C10:

- distributore +5 V: 0 mm³;
- distributore GND: 0 mm³;
- condensatore: 0 mm³.

## Margini minimi calcolati

- distributore GND–parete destra: 3,7 mm;
- distributore +5 V–C09: 9,6 mm;
- zona distributori–C05: 17,4 mm;
- distributori–appoggio inferiore C10: 2,0 mm;
- distributori–condensatore: 2,0 mm;
- condensatore–coperchio: 11,0 mm.

## Decisione

La seconda piastra C11 è stata eliminata perché interferiva con la base e con il montante anteriore della C10. Non va stampata.

La soluzione verificata usa C01, C09 e C10 già esistenti e non richiede alcuna modifica alla scatola né nuovi STL.

La verifica vale per distributori non più grandi di 30 × 19 × 10 mm. Se i pezzi acquistati superano anche una sola di queste misure, occorre ridisegnare il supporto.

## Integrità dei file STL attivi

Controllo mesh eseguito sui file correnti:

| Pezzo | Chiuso | Normali coerenti | Corpi |
|---|---:|---:|---:|
| C01 corpo finale v1.7 | sì | sì | 1 |
| C02 coperchio finale v1.7 | sì | sì | 1 |
| C03 sportello | sì | sì | 1 |
| C04 braccio D8,9 | sì | sì | 1 |
| C05 staffa servo | sì | sì | 1 |
| C06 fermacavo | sì | sì | 1 |
| C08 piedino | sì | sì | 1 |
| C09 slitta A | sì | sì | 1 |
| C10 slitta B rinforzata V2 | sì | sì | 1 |
