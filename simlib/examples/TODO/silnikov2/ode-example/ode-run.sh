#
# example of using program "ode" from GNU Plotutils
#

PROGRAM=`which ode`
if [ -z "$PROGRAM" -o ! -x $PROGRAM ]; then
    echo Program ode is not installed
    exit 1
fi

F=.silnikov.ode
STEP="1e-5"     # EDIT and update variable NNN accordingly
NNN=5000        # print output each NNN-th step ("decimation")

echo STEP=$STEP
echo Print each ${NNN}th step
echo

cat >"$F" <<__END__
a = 0.4
b = 0.65
c = 0
d = 1

x1' = x2
x1  = 0.1234
x2' = x3
x2  = 0.2
x3' = -a*x3 - x2 + b*x1*(1 - c*x1 - d*x1*x1)
x3  = 0.1

print t, x1, x2
step 0, 350
__END__

FILTER() {
    awk "{if(NR%$NNN==1)print}"
}

echo "Running test 1..."
time ode --runge-kutta    $STEP  <$F | FILTER >ode-rk.dat
echo "Running test 2..."
time ode --adams-moulton  $STEP  <$F | FILTER >ode-am.dat

rm -f "$F"

gnuplot <<__END__
set key
set style data lines

#set terminal png large; set output "silnikov-ode.png"
set terminal pdf; set output "silnikov-ode.pdf"
set title "Silnikov equation, various methods, step = $STEP"
plot [][-1.6:2.6] \
    "ode-rk.dat", \
    "ode-am.dat"

#set terminal png large; set output "silnikov-ode-phase.png"
set title "Silnikov equation, various methods, step = $STEP"
plot [-1.6:1.6][-1.6:1.6] \
    "ode-rk.dat" using 2:3, \
    "ode-am.dat" using 2:3

__END__

echo Done.
