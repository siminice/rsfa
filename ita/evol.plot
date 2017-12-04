set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:21]
set ytics ("20" 1, "19" 2, "18" 3, "17" 4, "16" 5, "15" 6, "14" 7, "13" 8, "12" 9, "11" 10, "10" 11, "9" 12, "8" 13, "7" 14, "6" 15, "5" 16, "4" 17, "3" 18, "2" 19, "1" 20)
plot [x=0:41] \
"evol.dat" using 1:2 title "Atalanta       " with linesp lw 3, \
"evol.dat" using 1:3 title "Bologna        " with linesp lw 3, \
"evol.dat" using 1:4 title "Cagliari       " with linesp lw 3, \
"evol.dat" using 1:5 title "Catania        " with linesp lw 3, \
"evol.dat" using 1:6 title "Cesena         " with linesp lw 3, \
"evol.dat" using 1:7 title "Chievo         " with linesp lw 3, \
"evol.dat" using 1:8 title "Fiorentina     " with linesp lw 3, \
"evol.dat" using 1:9 title "Genoa          " with linesp lw 3, \
"evol.dat" using 1:10 title "Inter          " with linesp lw 3, \
"evol.dat" using 1:11 title "Juventus       " with linesp lw 3, \
"evol.dat" using 1:12 title "Lazio          " with linesp lw 3, \
"evol.dat" using 1:13 title "Lecce          " with linesp lw 3, \
"evol.dat" using 1:14 title "Milan          " with linesp lw 3, \
"evol.dat" using 1:15 title "Napoli         " with linesp lw 3, \
"evol.dat" using 1:16 title "Novara         " with linesp lw 3, \
"evol.dat" using 1:17 title "Palermo        " with linesp lw 3, \
"evol.dat" using 1:18 title "Parma          " with linesp lw 3, \
"evol.dat" using 1:19 title "Roma           " with linesp lw 3, \
"evol.dat" using 1:20 title "Siena          " with linesp lw 3, \
"evol.dat" using 1:21 title "Udinese        " with linesp lw 3
