import numpy as np
import matplotlib.pyplot as plt

def linear_regression(x, y):
    """
    Вычисляет параметры прямой (угловой коэффициент и смещение) методом наименьших квадратов.
    Возвращает a (угловой коэффициент), b (смещение) и строку с уравнением прямой.
    """
    n = len(x)
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(x * y)
    sum_x2 = np.sum(x ** 2)
    
    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
    b = (sum_y - a * sum_x) / n
    
    equation = f"y = {a:.4f}x + {b:.4f}"
    return a, b, equation

def plot_points_with_regression(x, y):
    """
    Строит график точек (x, y) и линию регрессии.
    """
    # Вычисляем параметры прямой
    a, b, equation = linear_regression(x, y)
    
    # Создаем график
    plt.figure(figsize=(8, 6))
    plt.scatter(x, y, color='blue', label='Исходные точки')
    
    # Строим линию регрессии
    x_reg = np.linspace(min(x), max(x), 100)
    y_reg = a * x_reg + b
    plt.plot(x_reg, y_reg, color='red', label=f'Линия МНК: {equation}')
    
    # Настройки графика
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.title('График точек и линия регрессии')
    plt.legend()
    plt.grid(True)
    plt.savefig('plot_1.png')
    # plt.show()
	
    # Выводим параметры
    print("Параметры прямой:")
    print(f"Угловой коэффициент (a): {a:.4f}")
    print(f"Смещение (b): {b:.4f}")
    print(f"Уравнение прямой: {equation}")

# Пример использования
#if name == "__main__":
    # Пример данных (замените на свои)
x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
y = np.array([2.1, 3.9, 6.1, 8.0, 10.1, 12.2, 13.8, 16.1, 17.9, 20.0])

plot_points_with_regression(x, y)
