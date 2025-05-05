import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

def remove_outliers(x, y, threshold=2.0, max_iter=5):
    """
    Итеративно удаляет выбросы на основе остатков регрессии.
    Возвращает очищенные массивы x и y.
    
    Параметры:
        x, y - входные данные,
        threshold - порог для определения выбросов (в единицах стандартного отклонения),
        max_iter - максимальное число итераций.
    """
    x_clean = x.copy()
    y_clean = y.copy()
    
    for _ in range(max_iter):
        n = len(x_clean)
        
        # Линейная регрессия
        a, b, _, _, _ = stats.linregress(x_clean, y_clean)
        y_pred = a * x_clean + b
        residuals = y_clean - y_pred
        
        # Стандартное отклонение остатков
        sigma = np.std(residuals)
        
        # Находим индексы точек, которые НЕ являются выбросами
        mask = np.abs(residuals) < threshold * sigma
        
        # Если новых выбросов нет, завершаем
        if np.all(mask):
            break
        
        # Обновляем данные, исключая выбросы
        x_clean = x_clean[mask]
        y_clean = y_clean[mask]
    
    return x_clean, y_clean

def linear_regression_with_errors(x, y):
    """
    Вычисляет параметры прямой (a, b), их погрешности и R².
    Возвращает:
        a, b - параметры прямой,
        a_err, b_err - стандартные погрешности,
        r_squared - коэффициент детерминации,
        equation - строку с уравнением.
    """
    n = len(x)
    
    # Основные суммы
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(x * y)
    sum_x2 = np.sum(x ** 2)
    sum_y2 = np.sum(y ** 2)
    
    # Коэффициенты МНК
    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
    b = (sum_y - a * sum_x) / n
    
    # Предсказанные значения и остатки
    y_pred = a * x + b
    residuals = y - y_pred
    
    # Стандартное отклонение остатков
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

def plot_regression_with_outliers_removed(x, y, threshold=2.0):
    """
    Строит график данных, линию регрессии и удаляет выбросы.
    """
    # Удаляем выбросы
    x_clean, y_clean = remove_outliers(x, y, threshold)
    
    # Регрессия на очищенных данных
    a, b, a_err, b_err, r_squared, equation = linear_regression_with_errors(x_clean, y_clean)
    
    # График
    plt.figure(figsize=(10, 6))
    
    # Исходные точки (выбросы выделены красным)
    mask_outliers = ~np.isin(x, x_clean)
    plt.scatter(x[~mask_outliers], y[~mask_outliers], color='blue', label='Данные (без выбросов)')
    plt.scatter(x[mask_outliers], y[mask_outliers], color='red', label='Выбросы', alpha=0.5)
    
    # Линия регрессии
    x_fit = np.linspace(min(x_clean), max(x_clean), 100)
    y_fit = a * x_fit + b
    plt.plot(x_fit, y_fit, color='green', label=f'МНК (без выбросов): {equation}')
    
    # Настройки
    plt.xlabel('X', fontsize=12)
    plt.ylabel('Y', fontsize=12)
    plt.title(f'Линейная регрессия с удалением выбросов (порог: {threshold}σ)', fontsize=14)
    plt.legend(fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # Вывод статистики
    print("\nРезультаты регрессии (без выбросов):")
    print(f"Угловой коэффициент (a) = {a:.4f} ± {a_err:.4f}")
    print(f"Смещение (b) = {b:.4f} ± {b_err:.4f}")
    print(f"Коэффициент детерминации R² = {r_squared:.4f}")
    print(f"Уравнение: {equation}")
    print(f"Удалено выбросов: {len(x) - len(x_clean)}")
    
    plt.savefig('plot_3.png')

# Пример использования
#if __name__ == "__main__":
# Пример данных с выбросами
x = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
y = np.array([2.1, 3.9, 6.2, 7.8, 10.1, 12.0, 13.9, 15.8, 18.1, 20.0, 5.0, 25.0])  # 5.0 и 25.0 - выбросы

plot_regression_with_outliers_removed(x, y, threshold=2.0)