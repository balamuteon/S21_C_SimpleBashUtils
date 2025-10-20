#!/bin/bash

INPUT_FILE=("bytes.txt" "asci.txt")

if [[ ! -f "$INPUT_FILE" ]]; then
    echo "Ошибка: файл $INPUT_FILE не найден!"
    exit 1
fi

NATIVE_CAT="cat"
CUSTOM_CAT="./s21_cat"

TMP_DIR="test_output"
mkdir -p "$TMP_DIR"

FLAGS=("-b" "-e" "-n" "-s" "-t") # "-E" "-T"

REPORT_FILE="$TMP_DIR/test_report.txt"
> "$REPORT_FILE"

run_test() {
    local flag=$1
    local native_output="$TMP_DIR/native_output_$flag.txt"
    local custom_output="$TMP_DIR/custom_output_$flag.txt"
    
    $NATIVE_CAT $flag "$INPUT_FILE" > "$native_output"
    
    $CUSTOM_CAT $flag "$INPUT_FILE" > "$custom_output"
    
    if diff -q "$native_output" "$custom_output" > /dev/null; then
        echo "Тест с флагом $flag: УСПЕШНО" >> "$REPORT_FILE"
    else
        echo "Тест с флагом $flag: ПРОВАЛ" >> "$REPORT_FILE"
        echo "Различия:" >> "$REPORT_FILE"
        diff "$native_output" "$custom_output" >> "$REPORT_FILE"
    fi
}

for flag in "${FLAGS[@]}"; do
    run_test "$flag"
done

cat "$REPORT_FILE"

rm -r "$TMP_DIR"
