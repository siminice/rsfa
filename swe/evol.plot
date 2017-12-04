set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.8 box
set grid
f1(x) = 100;
f2(x) = 200;
f3(x) = 300;
plot [y=1925:2011] \
"evol.dat" using 1:2 title "IFK Göteborg" with lines lw 2, \
"evol.dat" using 1:3 title "Göteborg AIS " with lines lw 2, \
"evol.dat" using 1:4 title "Örgryte JS Göteborg" with lines lw 2, \
"evol.dat" using 1:5 title "BK Häcken Göteborg" with lines lw 2, \
f1(y) notitle lw 3, f2(y) notitle lw 3, f3(y) notitle lw 3
