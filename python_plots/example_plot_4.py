import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

def detect_outliers(x, y, threshold=2.5):
    """
    Обнаруживает выбросы с помощью метода межквартильного размаха (IQR)
    Возвращает маску с True для выбросов
    """
    # Сначала выполняем линейную регрессию
    slope, intercept, _, _, _ = stats.linregress(x, y)
    residuals = y - (slope * x + intercept)
    
    # Вычисляем квартили и межквартильный размах
    Q1 = np.percentile(residuals, 25)
    Q3 = np.percentile(residuals, 75)
    IQR = Q3 - Q1
    
    # Определяем границы для выбросов
    lower_bound = Q1 - threshold * IQR
    upper_bound = Q3 + threshold * IQR
    
    # Возвращаем маску выбросов
    return (residuals < lower_bound) | (residuals > upper_bound)

def linear_regression_with_errors(x, y):
    """
    Выполняет линейную регрессию и вычисляет погрешности параметров
    Возвращает:
        slope, intercept - параметры прямой
        slope_err, intercept_err - погрешности параметров
        r_squared - коэффициент детерминации
    """
    n = len(x)
    
    # Вычисляем основные суммы
    sum_x = np.sum(x)
    sum_y = np.sum(y)
    sum_xy = np.sum(x * y)
    sum_x2 = np.sum(x ** 2)
    sum_y2 = np.sum(y ** 2)
    
    # Коэффициенты МНК
    slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)
    intercept = (sum_y - slope * sum_x) / n
    
    # Предсказанные значения и остатки
    y_pred = slope * x + intercept
    residuals = y - y_pred
    
    # Стандартное отклонение остатков
    sigma = np.sqrt(np.sum(residuals ** 2) / (n - 2))
    
    # Погрешности параметров
    x_mean = np.mean(x)
    S_xx = np.sum((x - x_mean) ** 2)
    
    slope_err = sigma / np.sqrt(S_xx)
    intercept_err = sigma * np.sqrt(1 / n + x_mean ** 2 / S_xx)
    
    # Коэффициент детерминации R²
    ss_total = np.sum((y - np.mean(y)) ** 2)
    ss_residual = np.sum(residuals ** 2)
    r_squared = 1 - (ss_residual / ss_total)
    
    return slope, intercept, slope_err, intercept_err, r_squared

def plot_regression_with_outliers(x, y, threshold=2.5):
    """
    Строит график с выделением выбросов и линией регрессии
    """
    # Обнаруживаем выбросы
    outlier_mask = detect_outliers(x, y, threshold)
    
    # Выполняем регрессию без выбросов
    clean_x = x[~outlier_mask]
    clean_y = y[~outlier_mask]
    
    slope, intercept, slope_err, intercept_err, r_squared = linear_regression_with_errors(clean_x, clean_y)
    
    # Создаем график
    plt.figure(figsize=(10, 6))
    
    # Рисуем нормальные точки (синие)
    plt.scatter(clean_x, clean_y, color='blue', label='Нормальные точки')
    
    # Рисуем выбросы (красные)
    if np.any(outlier_mask):
        plt.scatter(x[outlier_mask], y[outlier_mask], color='red', label='Выбросы')
    
    # Рисуем линию регрессии
    x_fit = np.linspace(min(x), max(x), 100)
    y_fit = slope * x_fit + intercept
    plt.plot(x_fit, y_fit, color='green', label=f'МНК: y = ({slope:.3f}±{slope_err:.3f})x + ({intercept:.3f}±{intercept_err:.3f})\nR² = {r_squared:.3f}')
    
    # Настраиваем график
    plt.xlabel('X', fontsize=12)
    plt.ylabel('Y', fontsize=12)
    plt.title('Линейная регрессия с выделением выбросов', fontsize=14)
    plt.legend(fontsize=10)
    plt.grid(True, linestyle='--', alpha=0.6)
    
    # Выводим статистику
    print("\n=== Результаты регрессии ===")
    print(f"Угловой коэффициент: {slope:.5f} ± {slope_err:.5f}")
    print(f"Смещение: {intercept:.5f} ± {intercept_err:.5f}")
    print(f"Коэффициент детерминации R²: {r_squared:.5f}")
    print(f"Обнаружено выбросов: {np.sum(outlier_mask)} из {len(x)} точек")
    
    plt.tight_layout()
    plt.savefig('plot_4.png')

# Пример использования
if __name__ == "__main__":
    
    x = np.array([      1,       2,       3,       4,       5,       6,       7,       8,       9,      10,      11,      12,      13,       1,       2,       3,       4,       5,       6,       7,       8,       9,      10,      11,      12,      13,       1,       2,       3,       4,       5,       6,       7,       8,       9,      10,      11,      12,      13,       1,       2,       3,       4,       5,       6,       7,       8,       9,      10,      11,      12,      13,       1,       2,       3,       4,       5,       6,       7,       8,       9,      10,      11,      12,      13, ])
    y = np.array([  25450,    2826,    2100,    3690,    3206,    4424,    6034,    4026,    5070,    6114,    5972,   14218,    7370,     734,     908,    1130,    1290,    2302,    2894,    3476,    4204,    8776,    5172,    5366,    6300,    6462,     658,     846,     970,    1268,    2312,    3000,    3606,    3866,    4314,    4788,    5312,   10254,    6650,     706,     848,    1022,    1240,    2298,    2978,    3544,    3872,    4406,    4912,    5612,    6284,    6246,     990,     996,    1050,    1266,    2346,    7166,    3998,    4324,    4296,    5960,    5346,    6264,    6332, ])

    # Запускаем анализ
    plot_regression_with_outliers(x, y, threshold=2.5)