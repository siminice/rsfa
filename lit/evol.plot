set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:11]
set ytics ("10" 1, "9" 2, "8" 3, "7" 4, "6" 5, "5" 6, "4" 7, "3" 8, "2" 9, "1" 10)
plot [x=0:17] \
"evol.dat" using 1:2 title "Jonava         " with linesp lw 3, \
"evol.dat" using 1:3 title "Trakai         " with linesp lw 3, \
"evol.dat" using 1:4 title "Nevezis        " with linesp lw 3, \
"evol.dat" using 1:5 title "Granitas       " with linesp lw 3, \
"evol.dat" using 1:6 title "Polonija       " with linesp lw 3, \
"evol.dat" using 1:7 title "Kaunas         " with linesp lw 3, \
"evol.dat" using 1:8 title "Palanga        " with linesp lw 3, \
"evol.dat" using 1:9 title "Silute         " with linesp lw 3, \
"evol.dat" using 1:10 title "Venta          " with linesp lw 3, \
"evol.dat" using 1:11 title "Lifosa         " with linesp lw 3
