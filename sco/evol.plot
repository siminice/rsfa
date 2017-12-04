set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.8 box
set grid
f1(x) = 100;
f2(x) = 200;
f3(x) = 300;
plot [y=1891:2007] \
"evol.dat" using 1:2 title "Third Lanark" with lines lw 2, \
"evol.dat" using 1:3 title "Clyde FC Glasgow" with lines lw 2, \
"evol.dat" using 1:4 title "East Fife Methil" with lines lw 2, \
f1(y) notitle lw 3, f2(y) notitle lw 3, f3(y) notitle lw 3
