#!/bin/bash

RASQL="rasql --user rasadmin --passwd rasadmin"

#$RASQL -q "select min_cells(dtm.hillshade(c)) from DTM as c" --out string
#$RASQL -q "select max_cells(dtm.hillshade(c)) from DTM as c" --out string

for f in hillshade slope aspect; do
  $RASQL -q "select encode(dtm.$f(c), \"png\") from DTM as c" --out file --outfile $f
done

