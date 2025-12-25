#!/bin/bash

# Директория для поиска, по умолчанию текущая
DIR="${1:-.}"

# Массив расширений файлов
EXTENSIONS=("*.cpp" "*.c" "*.h")

# Общий счетчик строк
total_lines=0

echo "Подсчет строк в .cpp, .c и .h файлах в $DIR с учетом .gitignore"

for ext in "${EXTENSIONS[@]}"; do
    # Используем git ls-files чтобы исключить игнорируемые файлы
    while IFS= read -r file; do
        # Проверяем, что файл реально существует (на случай, если DIR не корень репозитория)
        if [[ -f "$file" ]]; then
            lines=$(wc -l < "$file")
            echo "$file: $lines строк"
            total_lines=$((total_lines + lines))
        fi
    done < <(git -C "$DIR" ls-files --exclude-standard -co -- "$ext")
done

echo "Общее количество строк: $total_lines"

