# 生成乘法三元组
import random
trifile1 = open('triple1', 'w')
trifile2 = open('triple2', 'w')
rfile1 = open('randomshare1','w')
rfile2 = open('randomshare2','w')
rsignfile1 = open('rsignshare1', 'w')
rsignfile2 = open('rsignshare2', 'w')

n = 1000000
maxi  = 2**16
for i in range(n):
    a = random.randint(2, maxi)
    b = random.randint(2, maxi)
    c = a * b
    a1 = random.randint(1, a)
    b1 = random.randint(1, b)
    c1 = random.randint(1, c)
    trifile1.write(str(a1) + ' ' + str(b1) + ' ' + str(c1) + '\n')
    trifile2.write(str(a-a1) + ' ' + str(b-b1) + ' ' + str(c-c1) + '\n')

    r = random.randint(1,maxi)
    r1 = random.randint(1,r)
    r2 = r-r1
    rfile1.write(str(r1) + '\n')
    rfile2.write(str(r2) + '\n')
    
    rsign = random.randint(1,2)
    if rsign == 2: 
        rsign = -1
    rsign1 = random.randint(1,maxi)
    rsign2 = rsign - rsign1 
    rsignfile1.write(str(rsign1) + '\n')
    rsignfile2.write(str(rsign2) + '\n')

