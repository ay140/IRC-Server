#!/bin/bash
PID=$1
INTERVAL=${2:-5}

if [ -z "$PID" ]; then
  echo "Usage: $0 <pid> [interval]"
  exit 1
fi

while true; do
  lsof -p $PID | wc -l
  sleep $INTERVAL
done