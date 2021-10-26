############################################################################
# GnuPlot script for model heating
#
# Data format: Time  temperature relay_state
#
reset  # defaults

#set output "heating.ps"; set term postscript

set grid
set style data lines

set title "Heating system response"
set key
set xlabel "Time"
set ylabel "t"
plot "heating.dat" using 1:2 title "Temperature", "" using 1:3 title "Relay" with steps lw 2 lt 1

pause 1

