#!/bin/csh

foreach f (belarus estonia faeroe finland georgia iceland ireland-republic kazakhstan latvia lithuania norway sweden)

 echo 'Editting u-'$f'...'
 sed -e 's/2017/2018/' u-$f > temp
 mv -f temp u-$f
 chmod a+x u-$f

end
