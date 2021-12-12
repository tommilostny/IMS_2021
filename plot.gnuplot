set title "Globální stav čipů na trhu"
set xlabel "Čas [roky]"
set ylabel "Počet čipů"
set xtics ("2020" 0, "2021" 8760, "2022" 17520, "2023" 26280, "2024" 35040, "2025" 43800, "2026" 52560, "2027" 61320, "2028" 70080, "2029" 78840, "2030" 87600)

plot "chipshortage.txt" using 1:2 title "Čipy na skladě",\
"chipshortage.txt" using 1:3 title "Deficit čipů"

set size ratio 0.6
set term png
set output "chipshortage.png"
replot
set term x11
