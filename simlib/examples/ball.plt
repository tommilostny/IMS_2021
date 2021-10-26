############################################################################
# GnuPlot script for model ball
#
# Data format: Time y v
#
reset  # defaults

#set output "ball.ps"; set term postscript

set style data lines
#set grid

set title "Bouncing ball"
set key
set xlabel "Time [s]"
set ylabel "Height [m]"
plot "ball.dat" using 1:2 title "Ball height", 0 notitle with dots
pause 1

#end
