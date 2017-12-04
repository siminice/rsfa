set term post eps color "Helvetica 20" 
set output "evol.eps" 
set key below spacing 0.95 box
set grid
set yrange [100:300]
f2(x) = 200;
plot [y=1963:2012] \
f2(y) notitle lt -1
