===============================================================================
NÁVOD NA POUŽITÍ:
===============================================================================

1) Zkopírujte Váš .zip archiv pripravený na odevzdání do WIS do této složky.

2) Hodnotu svého loginu vložte do promenné LOGIN ve skriptu test.bat.

3) Spustte test projektu voláním skriptu test.bat.

===============================================================================
VÝSTUPY:
===============================================================================

standardní výstup - Strucný prehled prubehu a výsledku testu.

work/ - Složka obsahující pracovní soubory vytvorené v prubehu testu.

work/log/ - Složka obsahující podrobnosti o prubehu jednotlivých kroku testu.

POZN1: Test nekontroluje obsah dokumentace, ale pouze její existenci.
POZN2: Výsledky testu jsou pouze orientacní a mohou se lišit od konecného
       hodnocení projektu. V zásade ale, pokud test objeví nekterý nedostatek, 
       je vysoce pravdepodobné, že se tento nedostatek objeví i pri hodnocení.


===============================================================================
CO TEST.BAT DELÁ:
===============================================================================

1) Nejprve si rozbalí vstupní .zip archiv a zkontroluje, zda obsahuje všechny
   požadované soubory ve správných složkách.

2) Spustí simulaci projektu v prostredí ModelSim.

3) Spustí syntézu projektu v prostredí Xilinx ISE.

4) Provede ohodnocení dosažených výsledku. Porovná odpovedi v log souborech 
   získaných v kroku 2) se vzorovými odpovedmi a skontroluje úspech syntézy 
   v kroku 3).
