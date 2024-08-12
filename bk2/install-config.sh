#!/bin/sh

# exit if something fails
set -e

if [ "$(id -u)" != "0" ]; then
    echo $0 must be run as root
    exit 1
fi

ts=$(date +"%Y-%m-%d-%T")
backup=/tmp/systems-$ts

echo -n "Backing up existing files to $backup..."

crontab -u root -l > $backup/crontab

echo -n "Putting new configuration into place..."

crontab -u root < /local/systems/cron-box-crontab

echo "done."
