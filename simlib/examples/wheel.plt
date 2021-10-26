############################################################################
# GnuPlot script for model wheel
#
# Data format: Time y v
#
reset  # defaults

#set output "wheel.ps"; set term postscript

set grid
set style data lines

set title "Simple wheel dynamics"
set key
set xlabel "Time [s]"
set ylabel "y [m],  v [m/s]"
plot "wheel.dat" title "y", \
     "wheel.dat" using 1:3 title "v" with dots
pause 1

set ylabel "y [m]"
plot "wheel.dat" title "y"
pause 1

set title "Wheel dynamics - phase space"
set nokey
set xlabel "y [m]"
set ylabel "v [m/s]"
plot "wheel.dat" using 2:3 with lines
pause 1

