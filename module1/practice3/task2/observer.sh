#!/bin/bash
config_file="config.txt"
log_file="observer.log"
while read -r script_path; do
    if ! pgrep -f "bash $script_path" > /dev/null; then
        nohup bash "$script_path" &
        echo "$(date) Перезапущен $script_path" >> $log_file
    fi
done < "$config_file"
