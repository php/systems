#! /bin/sh
#
# creates aliases.

. /etc/profile

a=/etc/aliases
p=/etc/aliases-03phpmasterdb

# dont copy+paste the next line, it contains a TAB
tail -n +2 /home/emailsync/aliases |sed "s,	,: ," > $p.new

if ! test -s $p.new; then
  exit 1
fi

mv $p.new $p

cat $a-* > $a.new

if ! test -s $a.new; then
  exit 1
fi

if diff $a.new $a >/dev/null; then
	rm -f $a.new
	exit 0
fi

diff -u $a $a.new | mail -s "changes to php-smtp2:aliases" systems@php.net

# give it time to deliver the email

sleep 10

mv $a.new $a

newaliases
