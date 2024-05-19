#!/bin/bash

WORKING_DIRECTORY="/home/pico"
DIRECTORY_TO_WATCH="${WORKING_DIRECTORY}/posts"

rm -f "${DIRECTORY_TO_WATCH}/*"

inotifywait -m -e create "${DIRECTORY_TO_WATCH}" --format '%w%f' | while read NEW_FILE
do
  sh "${WORKING_DIRECTORY}/on_change.sh" "${NEW_FILE}"
done
