from scapy.all import *
import sys
import datetime 
from pprint import pprint
import numpy as np
import matplotlib.pyplot as plt

import random
import math
import scipy.stats as st
import warnings

CONFIDENCE_LEVEL = .9
TCP_TYPE = "DCTCP"
l = len(sys.argv)
nameSource = ["tcplarge-2-0.pcap" , "tcplarge-0-0.pcap", "tcplarge-5-0.pcap", "tcplarge-6-0.pcap"]
nameSink = ["tcplarge-13-0.pcap", "tcplarge-17-0.pcap", "tcplarge-16-0.pcap", "tcplarge-14-0.pcap"]
numSIM = 25

def getdata(pathsource, pathsink):
    sourceList = rdpcap(pathsource)
    sinkList = rdpcap(pathsink)
    arrSource = enumerateList(sourceList)
    arrSink = enumerateList(sinkList)
    letThereBeLight = sourceList[0][TCP].time
    x = []
    y = []
    heatDeath = sinkList[-1][TCP].time
    
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
     
    return (sum(y)/len(y)), (len(sinkList)*1024), (heatDeath - letThereBeLight)
    
def error_interval(degrees_freedom, sampleEst, sampleStandardError):
    confidenceInterval = st.t.interval(alpha=CONFIDENCE_LEVEL, df=degrees_freedom,
            loc=sampleEst, scale=sampleStandardError)
    return(confidenceInterval)

def raw_estimation(data):
    i = 1
    errorInterval = [(0,0)]
    ret = 0
    sumSn = 0
    xbar = []
    for d in data:
        ret += d
        xbar.append(ret / i)
        sumSn += (d - (ret/i))**2
        
        if i != 1:
            errorInterval.append(error_interval(i, (ret/i)  , (math.sqrt( sumSn/i )/math.sqrt(i-1))))
        i+=1
    return xbar , errorInterval

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

if l == 2:#file name instead
     pathDir = sys.argv[1]
     dataTIMEINSYS = []
     dataTHROUGH = []
     for i in range(1,numSIM):
        path = pathDir + str(i) + "/"
        dataSUM = 0
        sumDataSunk = 0
        netTime = 0
        for i in range(0,4):
            pathSource = path + nameSource[i]
            pathSink = path + nameSink[i]
            #print(pathSource, pathSink)  
            avgtime, dataSunk, totalTime = getdata(pathSource, pathSink)
            dataSUM += avgtime
            sumDataSunk += dataSunk
            netTime = max(netTime, abs(totalTime))

        dataTIMEINSYS.append(dataSUM / numSIM)
        dataTHROUGH.append(sumDataSunk / netTime)
        
     #pprint(dataTIMEINSYS)
     #pprint(dataTHROUGH) 
     est, err = raw_estimation(dataTIMEINSYS)
     arr = [] 
     for i in err:
        arr.append(abs(i[1]-i[0])) 
     plt.scatter(range(1,len(est)+1), est)
     #print(len(range(1,numSIM)), len(est), len(arr))
     plt.errorbar(range(1, len(est)+1), est, yerr=arr, fmt="o")
     est, err = raw_estimation(dataTIMEINSYS)
     arr = [] 
     for i in err:
        arr.append(abs(i[1]-i[0])) 
     plt.scatter(range(1,len(est)+1), est)
     #print(len(range(1,numSIM)), len(est), len(arr))
     plt.errorbar(range(1, len(est)+1), est, yerr=arr, fmt="o")
     #     
     plt.title(TCP_TYPE + " Network Delay")
     plt.ylabel("seconds")
     plt.xlabel("Number of Simulations (N)")
     plt.savefig(pathDir + "NetworkDelay.eps")
     plt.savefig(pathDir + "NetworkDelay.svg")
     plt.clf()
     est, err = raw_estimation(dataTHROUGH)
     arr = [] 
     for i in err:
        arr.append(abs(i[1]-i[0]))
     plt.scatter(range(1,len(est)+1), est)
     print(len(range(1,numSIM)), len(est), len(arr))
     plt.errorbar(range(1, len(est)+1), est, yerr=arr, fmt="o")
     plt.title( TCP_TYPE + " Throughput")
     plt.ylabel("Bytes Per Second")
     plt.xlabel("Number of Simulations (N)")
     plt.savefig(pathDir + "Throughput.eps")
     plt.savefig(pathDir + "Throughput.svg")


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
         

