import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

def linear_regression_with_errors(x, y):
    """
    Вычисляет параметры прямой (a, b), их погрешности и коэффициент детерминации R².
    Возвращает:
        a, b - параметры прямой,
        a_err, b_err - стандартные погрешности параметров,
        r_squared - коэффициент детерминации,
        equation - строку с уравнением.
    """
    n = len(x)
    
    # Вычисляем основные суммы
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(x * y)
    sum_x2 = np.sum(x ** 2)
    sum_y2 = np.sum(y ** 2)
    
    # Коэффициенты МНК
    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
    b = (sum_y - a * sum_x) / n
    
    # Предсказанные значения y
    y_pred = a * x + b
    
    # Остатки (разница между реальными и предсказанными y)
    residuals = y - y_pred
    
    # Среднеквадратичная ошибка (стандартное отклонение остатков)
    sigma = np.sqrt(np.sum(residuals ** 2) / (n - 2))
    
    # Погрешности параметров
    x_mean = np.mean(x)
    S_xx = np.sum((x - x_mean) ** 2)
    
    a_err = sigma / np.sqrt(S_xx)
    b_err = sigma * np.sqrt(1 / n + x_mean ** 2 / S_xx)
    
    # Коэффициент детерминации R²
    ss_total = np.sum((y - np.mean(y)) ** 2)
    ss_residual = np.sum(residuals ** 2)
    r_squared = 1 - (ss_residual / ss_total)
    
    equation = f"y = ({a:.4f} ± {a_err:.4f})x + ({b:.4f} ± {b_err:.4f})"
    
    return a, b, a_err, b_err, r_squared, equation

def plot_regression(x, y):
    """
    Строит график точек, линию регрессии и выводит статистику.
    """
    # Вычисляем параметры регрессии
    a, b, a_err, b_err, r_squared, equation = linear_regression_with_errors(x, y)
    
    # Создаём график
    plt.figure(figsize=(10, 6))
    plt.scatter(x, y, color='blue', label='Данные')
    
    # Линия регрессии
    x_fit = np.linspace(min(x), max(x), 100)
    y_fit = a * x_fit + b
    plt.plot(x_fit, y_fit, color='red', label=f'МНК: {equation}')
    
    # Настройки графика
    plt.xlabel('X', fontsize=12)
    plt.ylabel('Y', fontsize=12)
    plt.title('Линейная регрессия с погрешностями параметров', fontsize=14)
    plt.legend(fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # Вывод статистики в консоль
    print("\nРезультаты регрессии:")
    print(f"Угловой коэффициент (a) = {a:.4f} ± {a_err:.4f}")
    print(f"Смещение (b) = {b:.4f} ± {b_err:.4f}")
    print(f"Коэффициент детерминации R² = {r_squared:.4f}")
    print(f"Уравнение: {equation}")
    plt.savefig('plot_2.png')
    #plt.show()


# Пример использования
# Пример данных (замените на свои)
x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
y = np.array([2.1, 3.9, 6.2, 7.8, 10.1, 12.0, 13.9, 15.8, 18.1, 20.0])

plot_regression(x, y)