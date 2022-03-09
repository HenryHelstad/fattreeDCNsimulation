import sys
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
numArg = len(sys.argv)

plt.ion()
plt.figure(figsize=(18,12), dpi=80)

def readindata(file):
    arr = []
    f = open(file)
    lines = f.readlines()

    for line in lines[4:]:
        arr.append(line.split())

    x = []
    y = []
    for i in arr:
        #print(i)
        x.append(float(i[0]))
        y.append(float(i[3]))
    endtime = x[-1]
    #print( endtime)
    return(x,y, endtime)
average = 0
for i in sys.argv[1:] :
    x,y, time = readindata(i)
    plt.scatter(x,y,s=10)
    throughput = float(1000000)/time
    average += throughput
    print( "thoughput for: ", i , " ", float(1000000)/time, " Bytes per sec")

print("average throughput: ", average/(numArg-1))
plt.xticks(np.arange(min(x), max(x)+50, 10.0))
plt.yticks(np.arange(min(y), max(y)+5000, 1000.0))

plt.title("TCP Vegas")
plt.ylabel("TCP window")
plt.xlabel("time")
plt.savefig('foo.png')

