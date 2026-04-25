<Qucs Schematic 26.1.0>
<Properties>
  <View=20,-12,879,487,1.18675,0,0>
  <Grid=10,10,1>
  <DataSet=ejercicio3.dat>
  <DataDisplay=ejercicio3.dpl>
  <OpenDisplay=0>
  <Script=ejercicio3.m>
  <RunScript=0>
  <showFrame=0>
  <FrameText0=Título>
  <FrameText1=Dibujado por:>
  <FrameText2=Fecha:>
  <FrameText3=Revisión:>
</Properties>
<Symbol>
</Symbol>
<Components>
  <.DC DC1 1 100 50 0 34 0 0 "26.85" 0 "0.001" 0 "1 pA" 0 "1 uV" 0 "no" 0 "150" 0 "no" 0 "none" 0 "CroutLU" 0>
  <GND * 1 210 440 0 0 0 0>
  <R R1 1 430 110 -26 15 0 0 "100 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R4 1 460 320 15 -26 0 1 "470 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R3 1 530 180 -26 15 0 0 "220 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <R R5 1 700 320 15 -26 0 1 "330 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <Vdc V1 1 210 340 18 -26 0 1 "9 V" 1>
  <IProbe Pr2 1 280 110 -26 16 0 0>
  <IProbe Pr3 1 700 230 -41 -26 0 3>
  <R R2 1 380 180 -26 15 0 0 "150 Ohm" 1 "26.85" 0 "0.0" 0 "0.0" 0 "26.85" 0 "european" 0>
  <IProbe Pr4 1 250 180 -26 16 0 0>
  <IProbe Pr6 1 460 240 -41 -26 0 3>
  <IProbe Pr7 1 210 260 16 -26 0 1>
  <IProbe Pr8 1 640 180 -26 -38 0 2>
</Components>
<Wires>
  <310 110 400 110 "" 0 0 0 "">
  <210 110 210 180 "" 0 0 0 "">
  <210 180 220 180 "" 0 0 0 "">
  <210 180 210 230 "" 0 0 0 "">
  <410 180 460 180 "" 0 0 0 "">
  <460 110 700 110 "" 0 0 0 "">
  <700 110 700 180 "" 0 0 0 "">
  <560 180 610 180 "" 0 0 0 "">
  <700 180 700 200 "" 0 0 0 "">
  <210 370 210 440 "" 0 0 0 "">
  <210 370 460 370 "" 0 0 0 "">
  <700 350 700 370 "" 0 0 0 "">
  <460 180 460 210 "" 0 0 0 "">
  <460 350 460 370 "" 0 0 0 "">
  <460 370 700 370 "" 0 0 0 "">
  <210 110 250 110 "" 0 0 0 "">
  <700 260 700 290 "" 0 0 0 "">
  <280 180 350 180 "" 0 0 0 "">
  <460 180 500 180 "" 0 0 0 "">
  <460 270 460 290 "" 0 0 0 "">
  <210 290 210 310 "" 0 0 0 "">
  <670 180 700 180 "" 0 0 0 "">
</Wires>
<Diagrams>
</Diagrams>
<Paintings>
  <Text 220 10 12 #000000 0 "En este simulador el voltaje de cada resistencia es la resta del voltaje de entrada y\nel voltaje de salida, por eso solo se usa para corrientes, los voltajes se muestran\nen la simulación de SimulIDE">
</Paintings>
