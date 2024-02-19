#!/bin/bash

RASQL="rasql --user rasadmin --passwd rasadmin"


for f in hillshade slope aspect; do
  $RASQL -q "delete function dtm.$f" --out string 
done
