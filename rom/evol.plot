set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:14]
set ytics ("13" 1, "12" 2, "11" 3, "10" 4, "9" 5, "8" 6, "7" 7, "6" 8, "5" 9, "4" 10, "3" 11, "2" 12, "1" 13)
plot [x=0:10] \
"evol.dat" using 1:2 title "Brazi          " with linesp lw 3, \
"evol.dat" using 1:3 title "Tarlungeni     " with linesp lw 3, \
"evol.dat" using 1:4 title "Unirea Campina " with linesp lw 3, \
"evol.dat" using 1:5 title "SCM Piteºti    " with linesp lw 3, \
"evol.dat" using 1:6 title "Zagon          " with linesp lw 3, \
"evol.dat" using 1:7 title "Reghin         " with linesp lw 3, \
"evol.dat" using 1:8 title "Conpet         " with linesp lw 3, \
"evol.dat" using 1:9 title "Targoviste     " with linesp lw 3, \
"evol.dat" using 1:10 title "Fagaras        " with linesp lw 3, \
"evol.dat" using 1:11 title "Albota         " with linesp lw 3, \
"evol.dat" using 1:12 title "Cisnadie       " with linesp lw 3, \
"evol.dat" using 1:13 title "Selimbar       " with linesp lw 3, \
"evol.dat" using 1:14 title "Plopeni        " with linesp lw 3
