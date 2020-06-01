#!/bin/bash

mkdir "database"
# shellcheck disable=SC2164
cd database
mkdir "catalog" "data" "index"
touch catalog/cata_file
cd ..
echo "Setup finished!"