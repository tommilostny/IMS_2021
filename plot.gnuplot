set xlabel "Čas"
set ylabel "Počet čipů"
set xtics ("2020" 0, "2021" 8760, "2022" 17520, "2023" 26280, "2024" 35040, "2025" 43800, "2026" 52560, "2027" 61320, "2028" 70080, "2029" 78840, "2030" 87600)

plot "chipshortage.txt" using 1:2 title "Čipy na skladě",\
"chipshortage.txt" using 1:3 title "Objednané čipy čekající na dodání"