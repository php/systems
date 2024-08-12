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

mkdir -p $backup

crontab -u root -l > $backup/crontab

echo "done."

echo -n "Putting new configuration into place..."

crontab -u root - < /local/systems/cron-box-crontab

# apache2
rm -f /etc/apache2/sites-enabled/*.conf
ln -s /etc/apache2/sites-enabled /local/systems/bk2/apache2/*.conf
service apache2 reload

echo "done."
