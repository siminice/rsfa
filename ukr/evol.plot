set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:17]
set ytics ("16" 1, "15" 2, "14" 3, "13" 4, "12" 5, "11" 6, "10" 7, "9" 8, "8" 9, "7" 10, "6" 11, "5" 12, "4" 13, "3" 14, "2" 15, "1" 16)
plot [x=0:31] \
"evol.dat" using 1:2 title "Shakhtar       " with linesp lw 3, \
"evol.dat" using 1:3 title "Dinamo Kiev    " with linesp lw 3, \
"evol.dat" using 1:4 title "Metalist       " with linesp lw 3, \
"evol.dat" using 1:5 title "Dnipro         " with linesp lw 3, \
"evol.dat" using 1:6 title "Arsenal        " with linesp lw 3, \
"evol.dat" using 1:7 title "Tavriya        " with linesp lw 3, \
"evol.dat" using 1:8 title "Met Donetsk    " with linesp lw 3, \
"evol.dat" using 1:9 title "Vorskla        " with linesp lw 3, \
"evol.dat" using 1:10 title "Kryvbas        " with linesp lw 3, \
"evol.dat" using 1:11 title "Odessa         " with linesp lw 3, \
"evol.dat" using 1:12 title "Mariupol       " with linesp lw 3, \
"evol.dat" using 1:13 title "Volyn          " with linesp lw 3, \
"evol.dat" using 1:14 title "Zaria Lugansk  " with linesp lw 3, \
"evol.dat" using 1:15 title "Karpaty Lvov   " with linesp lw 3, \
"evol.dat" using 1:16 title "Obolon         " with linesp lw 3, \
"evol.dat" using 1:17 title "Oleksandria    " with linesp lw 3
