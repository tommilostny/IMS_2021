
set key
set style data lines

#set terminal png large; set output "silnikov-ode.png"
set terminal pdf; set output "silnikov-ode.pdf"
set title "Silnikov equation, various methods, step = 1e-5"
plot [][-1.6:2.6] \
    "ode-rk.dat", \
    "ode-am.dat"

#set terminal png large; set output "silnikov-ode-phase.png"
set title "Silnikov equation, various methods, step = 1e-5"
plot [-1.6:1.6][-1.6:1.6] \
    "ode-rk.dat" using 2:3, \
    "ode-am.dat" using 2:3

