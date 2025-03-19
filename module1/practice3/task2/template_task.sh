#!/bin/bash
script_name=$(basename $0)
log_file="report_${script_name%.sh}.log"
echo "[$$] $(date) Скрипт запущен" >> $log_file
sleep_time=$((5 + RANDOM % 5))
sleep $sleep_time
minutes=$((sleep_time / 60))
echo "[$$] $(date) Скрипт завершился, работал $minutes минут" >> $log_file
