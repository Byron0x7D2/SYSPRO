#!/bin/bash

awk '{print $3}' $1 | sed "s/sdi//" | awk '$0 %2 == 0' > even.txt
awk '{print $3}' $1 | sed "s/sdi//" | awk '$0 %2 != 0' > odd.txt
