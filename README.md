# Spiegazione breve dei file presenti

## Produzione della simulazione
Compilando ed eseguendo il file `main142.cpp` in `root` si producono i file `.root` delle simulazioni, i quali verranno automaticamente rinominati manualmente in `simu_data/main142_X_1eY.cpp` (è necessario che si crei una cartella denominata `simu_data`), con "X" il caso interessato (`A`, `B`, `C`, `D`, `E`, `F` e `G`), con i seguenti valori di `norm`:
- [`A`] "default"   		= 119.6
- [`B`] "deuterone"			= 183.5597586
- [`C`] "antideuterone"			= 200.51252128
- [`D`] "fit #1 di [`A`,`B`]"	= 137.26665
- [`E`] "coalescenza"		= 1 (mod. coalescenza)
- [`F`] "fit `1/x`" 			= 133.581
- [`G`] "fit #2 di [`A`,`B`]" = 140.46721

e con "Y" invece l'ordine del numero di simulazioni fatte (e.g. 1 mln = 1e6 => Y=6).

Il file `main142_mmmt.cpp` invece implementa il multithreading per poter sfruttare al meglio i core dei cpu simulando in parallelo.

## Analisi delle simulazioni

Per analizzare i file delle simulazioni ci sono i seguenti file:

- `analyse.cpp`: si mettono a confronto i file `.root` delle simulazioni prodotti e quelli di CERN, eseguendo:
  - analisi dello spettro di produzione di (anti)protoni
  - rapporto tra protoni nel caso di produzione di deuteroni attivata e disattivata
  - analisi dello spettro di produzione di (anti)deuteroni
  - rapporto tra i dati simulati e i dati sperimentali
  - rapporto tra i deuteroni e antideuteroni
- `def_opt.cpp`: si va a confrontare i dati di simulazione con il modello default (`A`) e con il modello ottimizzato (`B`, `D`, `F` o `G`).
- `deuteron_analyse.cpp`: si analizzano i vari canali di produzione dei deuteroni, ossia si cerca di capire quali canali hanno contribuito di più o di meno alla produzione deuteronica, il loro spettro...
- `antideuteron_analyse.cpp`: stessa cosa di `deuteron_analyse.cpp` ma per i antideuteroni
- `fit_norm.cpp`: si va a fittare il parametro `norm` necessaria per le simulazione, al valore dell'energia di centro di massa $\sqrt s$ = 13 TeV, attraverso due metodi:
  - fit lineare
  - fit 1/x (caso `F`)
- `format.cpp`: si formatta il file `.root` preso in input eseguendo una divisione degli istogrammi presenti nel file root, rendendoli in pratica dei PDF.
- `ratio_CXS.cpp`: si va a confrontare la simulazione del modello di coalescenza (caso `E`) con quelle dei modelli di sez. d'urto efficace (`A`, `B`, `D`, `F` o `G`) eseguendo un rapporto tra i due istogrammi

## Codice LaTeX
Il materiale LaTeX della tesi è contenuta nella cartella `thesis`, nella quale si deve eseguire il file `main.tex`. Si sfrutta la modularità, ossia si divide in più file il codice sorgente per comodità (che si trovano nella cartella `thesis/tex`), senza dover scrivere l'intera tesi in un unico file (assolutamente non raccomandato).

Nel file `leo_preamble.sty` invece si immettono tutti i pacchetti usati e le (ri)definizioni di comandi.