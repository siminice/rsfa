set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:13]
set ytics ("12" 1, "11" 2, "10" 3, "9" 4, "8" 5, "7" 6, "6" 7, "5" 8, "4" 9, "3" 10, "2" 11, "1" 12)
plot [x=0:22] \
"evol.dat" using 1:2 title "The Strongest  " with linesp lw 3, \
"evol.dat" using 1:3 title "San Jose Oruro " with linesp lw 3, \
"evol.dat" using 1:4 title "Or Petrolero   " with linesp lw 3, \
"evol.dat" using 1:5 title "Blooming       " with linesp lw 3, \
"evol.dat" using 1:6 title "Real Potosi    " with linesp lw 3, \
"evol.dat" using 1:7 title "Sucre          " with linesp lw 3, \
"evol.dat" using 1:8 title "Aurora         " with linesp lw 3, \
"evol.dat" using 1:9 title "Nacional       " with linesp lw 3, \
"evol.dat" using 1:10 title "Bolivar        " with linesp lw 3, \
"evol.dat" using 1:11 title "La Paz FC      " with linesp lw 3, \
"evol.dat" using 1:12 title "Mamore         " with linesp lw 3, \
"evol.dat" using 1:13 title "Guabira        " with linesp lw 3
