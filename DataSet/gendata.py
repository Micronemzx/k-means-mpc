# 根据输入的data文件，分成两个秘密分享文件
import random
import sys
def gendata(filename):
    file = open(filename, 'r')
    data1 = open("data1", 'w')
    data2 = open("data2", "w")
    for line in file.readlines():
        line = line.strip('\n')
        line = line.split(',')
        for i in range(len(line)-1):
            line[i] = float(line[i])
            d1 = random.randint(0, 100)
            d2 = line[i]-d1
            data1.write(str(d1)+' ')
            data2.write(str(d2)+' ')
        data1.write('\n')
        data2.write('\n')
    file.close()
    data1.close()
    data2.close()

if __name__ == "__main__":
    gendata(sys.argv[1])