#!/bin/bash

PATTERN="raz"
COUNTER_SUCCESS=0
COUNTER_FAIL=0
LOG_FILE="log.txt"
echo "" > $LOG_FILE

TEST_FILES=("test.txt" "west.txt" "eest.txt")

declare -A my_program_outputs
declare -A native_outputs

for option in -e -i -v -c -l -n -s -h -o; do
    my_program_outputs[$option]=""
    native_outputs[$option]=""

    for TEST_FILE in "${TEST_FILES[@]}"; do
        echo "Testing option '$option' with file '$TEST_FILE'"

        MY_PROGRAM_CMD="./s21_grep $option $PATTERN $TEST_FILE"
        NATIVE_CMD="grep $option $PATTERN $TEST_FILE"

        my_program_output=$($MY_PROGRAM_CMD)
        native_output=$($NATIVE_CMD)

        my_program_outputs[$option]+="$my_program_output"$'\n'
        native_outputs[$option]+="$native_output"$'\n'

        trimmed_my_program_output=$(echo "$my_program_output" | xargs)
        trimmed_native_output=$(echo "$native_output" | xargs)

        if [[ "$trimmed_my_program_output" == "$trimmed_native_output" ]]; then
            ((COUNTER_SUCCESS++))
        else
            echo "$option with $TEST_FILE: Output differs" >> $LOG_FILE
            ((COUNTER_FAIL++))
        fi
    done
done

echo "SUCCESS: $COUNTER_SUCCESS"
echo "FAIL: $COUNTER_FAIL"

echo "Consolidated Outputs for each option:"
for option in -e -i -v -c -l -n -s -h -o; do
    echo "Option: $option"
    echo "My Program Output:"
    echo "${my_program_outputs[$option]}"
    echo "Native Program Output:"
    echo "${native_outputs[$option]}"
    echo "-----------------------------------------"
done