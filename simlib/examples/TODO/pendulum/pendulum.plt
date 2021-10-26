############################################################################
# Model: Pendulum
#

set output "pendulum.pdf";  set term pdf
set xlabel font ",10"; set ylabel font ",10"; set tics font ",10"
#set output "pendulum-1t.png";  set term pngcairo

set nogrid
set style data lines
set key
set title "Pendulum model"
set xlabel "Time [s]";  set ylabel "angle [rad], angular velocity [rad.s^{-1}]"
plot "pendulum.dat" using 1:2 title "angle", \
     "pendulum.dat" using 1:3 title "angular velocity" with dots

#set output "pendulum-2p.png";  set term pngcairo
set nokey
set title "Pendulum model - phase space"
set xlabel "angle [rad]";  set ylabel "angular velocity [rad.s^{-1}]"
plot "pendulum.dat" using 2:3

#set output "pendulum-3z.png";  set term pngcairo
set title "Pendulum model - phase space  (zoomed)"
#set grid
plot [3.14155:3.14162][] "pendulum.dat" using 2:3
#end
