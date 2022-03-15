#!/bin/bash
echo "$1"
for i in {1..10}
do
    mkdir "$1$i"
    ./waf --run tcplarge1
    mv *.pcap "$1$i"
done
   

