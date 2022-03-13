from scapy.all import *
import sys
import datetime 
from pprint import pprint
import numpy as np
import matplotlib.pyplot as plt
import statistics

l = len(sys.argv)

def enumerateList(packetList):
    a = []
    for s in packetList:
        #s.show()
        if Raw in s and TCP in s and IP in s:
            #datetime.datetime.fromtimestamp(s[TCP].time)
            a.append((s[TCP].time , s[IP].id, s[Raw].load))
            #s.show()
    return a        

if l ==1:
    print("Need More Arguments")
    print("example: \n\tpython3 file\path\source file\path\destination")

if l == 2:
    packetsList = rdpcap(sys.argv[1])
    a = enumerateList(packetsList)
    pprint(a)
if l == 3:   
    sourceList = rdpcap(sys.argv[1])
    sinkList = rdpcap(sys.argv[2])
    arrSource = enumerateList(sourceList)
    arrSink = enumerateList(sinkList)
    letThereBeLight = sourceList[0][TCP].time
    x = []
    y = []
    for sink in arrSink:
        result = [i for i, v in enumerate(arrSource) if v[1] == sink[1]]

        if len(result) > 1:
            print("----------------------")
            print("source ---")
            for i in result:
                print(arrSource[i])
            print("sink -----")
            print(sink) 

        else:
            #print("----------------------")
            #print("dela t: ", sink[0] - arrSource[result[0]][0]) 
            #print("exit time with respect to init ", sink[0] - letThereBeLight)
            x.append(arrSource[result[0]][0] - letThereBeLight)
            y.append(sink[0] - arrSource[result[0]][0])
    plt.scatter(x,y,s=10)
    plt.show()
         

