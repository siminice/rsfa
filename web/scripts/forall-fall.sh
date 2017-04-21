#!/bin/csh

foreach f ( albania andorra armenia austria azerbaijan belgium bosnia-herzegovina bulgaria croatia cyprus czech-republic denmark england france georgia germany greece hungary israel italy luxembourg macedonia-fyr malta moldova montenegro netherlands northern-ireland poland portugal romania russia san-marino scotland serbia slovakia slovenia spain switzerland turkey ukraine wales )

  echo $f
  ./change-2014 elog get-$f

end
