import os
import re

def find_longest_include_streak(directory):
    # Регулярное выражение для поиска строк с #include (игнорирует пробелы до и после)
    include_regex = re.compile(r'^\s*#\s*include\s*[<"].*[>"]\s*$')
    
    max_streak = 0
    best_file = None
    best_start_line = 0
    
    # Поддерживаемые расширения файлов (можно дополнить)
    valid_extensions = ('.cpp', '.c', '.h', '.hpp', '.cc', '.cxx')

    # Обход всех файлов в папке и её подпапках
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(valid_extensions):
                file_path = os.path.join(root, file)
                
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        lines = f.readlines()
                except Exception:
                    continue  # Пропускаем файлы, которые не удается прочитать

                current_streak = 0
                current_start = 0
                
                for line_idx, line in enumerate(lines, start=1):
                    # Проверяем, является ли строка инклудом
                    if include_regex.match(line):
                        if current_streak == 0:
                            current_start = line_idx
                        current_streak += 1
                    else:
                        # Если серия прервалась, проверяем, не рекордная ли она
                        if current_streak > max_streak:
                            max_streak = current_streak
                            best_file = file_path
                            best_start_line = current_start
                        current_streak = 0
                
                # Проверка на случай, если файл заканчивается серией инклудов
                if current_streak > max_streak:
                    max_streak = current_streak
                    best_file = file_path
                    best_start_line = current_start

    return best_file, max_streak, best_start_line

if __name__ == "__main__":
    # Укажите путь к вашей папке (например, '.' для текущей папки)
    target_directory = input("Введите путь к папке (или оставьте пустым для текущей): ").strip() or "."
    
    print(f"\nСканирую папку: {os.path.abspath(target_directory)}...\n")
    file_path, streak, start_line = find_longest_include_streak(target_directory)
    
    if best_file := file_path:
        print("🔥 Самая длинная серия #include найдена!")
        print(f"📁 Файл: {best_file}")
        print(f"📊 Количество подряд: {streak}")
        print(f"📍 Начиная со строки: {start_line}")
    else:
        print("❌ Файлы с исходным кодом или директивами #include не найдены.")
