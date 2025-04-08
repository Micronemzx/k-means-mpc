import sys

if __name__ == "__main__":
    file_name = sys.argv[1]
    with open(file_name, 'r') as f:
        lines = f.readlines()
        center = dict()
        cnt = dict()
        for line in lines:
            data = line.strip().split(',')
            if data[-1] not in center:
                center[data[-1]] = [0] * (len(data)-1)
                cnt[data[-1]] = 0
            for i in range(len(data)-1):
                center[data[-1]][i] += float(data[i])
                
            cnt[data[-1]] += 1
        for key in center:
            for i in range(len(center[key])):
                center[key][i] /= cnt[key]
        for key in center:
            print(key, center[key])
