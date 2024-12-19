set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:13]
set ytics ("12" 1, "11" 2, "10" 3, "9" 4, "8" 5, "7" 6, "6" 7, "5" 8, "4" 9, "3" 10, "2" 11, "1" 12)
plot [x=0:22] \
"evol.dat" using 1:2 title "Crvena Zvezda  " with linesp lw 3, \
"evol.dat" using 1:3 title "Hajduk Split   " with linesp lw 3, \
"evol.dat" using 1:4 title "Partizan       " with linesp lw 3, \
"evol.dat" using 1:5 title "Vojvodina      " with linesp lw 3, \
"evol.dat" using 1:6 title "OFK            " with linesp lw 3, \
"evol.dat" using 1:7 title "Sarajevo       " with linesp lw 3, \
"evol.dat" using 1:8 title "Dinamo Zagreb  " with linesp lw 3, \
"evol.dat" using 1:9 title "Lok.Zagreb     " with linesp lw 3, \
"evol.dat" using 1:10 title "Vardar         " with linesp lw 3, \
"evol.dat" using 1:11 title "Subotica       " with linesp lw 3, \
"evol.dat" using 1:12 title "Mostar         " with linesp lw 3, \
"evol.dat" using 1:13 title "NK Zagreb      " with linesp lw 3
