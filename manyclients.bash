#!/bin/bash

for N in {1..5}
do
    ruby client.rb &
done
wait
# bash ./manyclients.bash