#! /bin/sh

. /etc/profile

d=aliases

echo "select username,email from users where enable = 1 and email != '' order by username" | \
mysql -u nobody phpmasterdb | \
ssh mailout.php.net "cat - > $d.new && test -s $d.new && mv $d.new $d"
