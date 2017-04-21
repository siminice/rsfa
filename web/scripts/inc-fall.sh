#!/bin/csh

foreach f ( albania andorra armenia austria azerbaijan belgium bosnia-herzegovina bulgaria croatia cyprus czech-republic denmark england france georgia germany gibraltar greece hungary israel italy kosovo luxembourg macedonia-fyr malta moldova montenegro netherlands northern-ireland poland portugal romania russia san-marino scotland serbia slovakia slovenia spain switzerland turkey ukraine wales )
 echo 'Editting update-'$f'...'
 sed -e 's/2016/2017/' u-$f > temp
 mv -f temp u-$f
 chmod a+x u-$f
end
