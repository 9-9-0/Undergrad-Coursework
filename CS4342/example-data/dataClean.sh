#!/bin/bash
for i in $(cat bigData.csv | cut -d ',' -f2); do if [ "$i" = "a" ]; then echo 1; else echo 0; fi; done > bigData2Ans.csv
