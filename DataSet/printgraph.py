import matplotlib.pyplot as plt
import numpy as np

# 定义x轴和y轴数据
x_axis_data = [150,300,500]
y_axis_data = [2.45, 5.11, 8.74]

# 使用plot函数绘制折线图
plt.plot(x_axis_data, y_axis_data, 'b*--', alpha=0.5, linewidth=1, label='k=4')

# 添加图例、x轴和y轴标签
plt.legend()
plt.xlabel('数据点量')
plt.ylabel('运行时间')


# 显示图表
plt.show()