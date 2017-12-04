set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [0:23]
set ytics ("22" 1, "21" 2, "20" 3, "19" 4, "18" 5, "17" 6, "16" 7, "15" 8, "14" 9, "13" 10, "12" 11, "11" 12, "10" 13, "9" 14, "8" 15, "7" 16, "6" 17, "5" 18, "4" 19, "3" 20, "2" 21, "1" 22)
plot [x=0:57] \
"evol.dat" using 1:2 title "Leeds          " with linesp lw 3, \
"evol.dat" using 1:3 title "Man.United     " with linesp lw 3, \
"evol.dat" using 1:4 title "Sheffield Wed. " with linesp lw 3, \
"evol.dat" using 1:5 title "Arsenal        " with linesp lw 3, \
"evol.dat" using 1:6 title "Man.City       " with linesp lw 3, \
"evol.dat" using 1:7 title "Liverpool      " with linesp lw 3, \
"evol.dat" using 1:8 title "Aston Villa    " with linesp lw 3, \
"evol.dat" using 1:9 title "Nottingham     " with linesp lw 3, \
"evol.dat" using 1:10 title "QPR            " with linesp lw 3, \
"evol.dat" using 1:11 title "Crystal Palace " with linesp lw 3, \
"evol.dat" using 1:12 title "Sheffield Utd. " with linesp lw 3, \
"evol.dat" using 1:13 title "Everton        " with linesp lw 3, \
"evol.dat" using 1:14 title "Chelsea        " with linesp lw 3, \
"evol.dat" using 1:15 title "Milton Keynes  " with linesp lw 3, \
"evol.dat" using 1:16 title "Oldham         " with linesp lw 3, \
"evol.dat" using 1:17 title "Tottenham      " with linesp lw 3, \
"evol.dat" using 1:18 title "Norwich        " with linesp lw 3, \
"evol.dat" using 1:19 title "Southampton    " with linesp lw 3, \
"evol.dat" using 1:20 title "Coventry       " with linesp lw 3, \
"evol.dat" using 1:21 title "Luton          " with linesp lw 3, \
"evol.dat" using 1:22 title "Notts County   " with linesp lw 3, \
"evol.dat" using 1:23 title "West Ham       " with linesp lw 3
