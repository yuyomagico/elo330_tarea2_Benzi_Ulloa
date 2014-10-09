set multiplot layout 3, 1 title "Sonidos" font ",14"
set tmargin 2
set title "Original"
unset key
plot "data.dat" using 1:2 with lines
set title "Amplified"
unset key
plot "data.dat" using 1:3  with lines
set title "Restored"
unset key
plot "data.dat" using 1:4  with lines
unset multiplot