set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:17]
set ytics ("16" 1, "15" 2, "14" 3, "13" 4, "12" 5, "11" 6, "10" 7, "9" 8, "8" 9, "7" 10, "6" 11, "5" 12, "4" 13, "3" 14, "2" 15, "1" 16)
plot [x=0:17] \
"evol.dat" using 1:2 title "Vitez          " with linesp lw 3, \
"evol.dat" using 1:3 title "Sloboda Tuzla  " with linesp lw 3, \
"evol.dat" using 1:4 title "Gracanica      " with linesp lw 3, \
"evol.dat" using 1:5 title "Branitelj      " with linesp lw 3, \
"evol.dat" using 1:6 title "Lukavac        " with linesp lw 3, \
"evol.dat" using 1:7 title "Rudar Kakanj   " with linesp lw 3, \
"evol.dat" using 1:8 title "Troglav Livno  " with linesp lw 3, \
"evol.dat" using 1:9 title "Podgrmec       " with linesp lw 3, \
"evol.dat" using 1:10 title "Bosna Visoko   " with linesp lw 3, \
"evol.dat" using 1:11 title "Capljina       " with linesp lw 3, \
"evol.dat" using 1:12 title "Krajina Cazin  " with linesp lw 3, \
"evol.dat" using 1:13 title "Iskra Bugojno  " with linesp lw 3, \
"evol.dat" using 1:14 title "Banovic        " with linesp lw 3, \
"evol.dat" using 1:15 title "Bihac          " with linesp lw 3, \
"evol.dat" using 1:16 title "Gorazde        " with linesp lw 3, \
"evol.dat" using 1:17 title "Krajisnik      " with linesp lw 3
