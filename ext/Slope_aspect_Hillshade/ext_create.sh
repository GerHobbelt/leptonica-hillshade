#!/bin/bash

RASQL="rasql --user rasadmin --passwd rasadmin"

$RASQL -q 'create function dtm.hillshade (array a)
returns array
language cpp
extern "dtm_hillshade.so"'

$RASQL -q 'create function dtm.slope (array a)
returns array
language cpp
extern "dtm_hillshade.so"'

$RASQL -q 'create function dtm.aspect (array a)
returns array
language cpp
extern "dtm_hillshade.so"'
