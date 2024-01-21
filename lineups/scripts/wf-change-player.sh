#!/bin/bash

echo '#!/bin/bash' > wfcp.sh
echo "" >> wfcp.sh
echo 'FILES=$(grep '"$1"' seasons/*/* | cut -d ':' -f 1)' >> wfcp.sh
echo "" >> wfcp.sh
echo 'for f in ${FILES}' >> wfcp.sh
echo 'do' >> wfcp.sh
echo '  echo $f' >> wfcp.sh
echo "  sed -i 's/$1/$2/g'"' $f' >> wfcp.sh
echo 'done' >> wfcp.sh

chmod +x wfcp.sh
cat wfcp.sh
