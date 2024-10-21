#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))

# Find users file
HOSTNAME=$( hostname )
if [ -f "$DIR/$HOSTNAME/users" ]; then
	USERS_FILE="$DIR/$HOSTNAME/users"
else
	HOSTNAME=$( hostname -s )
	if [ -f "$DIR/$HOSTNAME/users" ]; then
		USERS_FILE="$DIR/$HOSTNAME/users"
	fi
fi

if [ "$USERS_FILE" = "" ]; then
	echo "Can't find users file"
	exit -1
fi

echo Using $USERS_FILE to create users.

while IFS= read -r line; do
	if [ "${line:0:1}" = "#" ]; then
		continue
	fi

	echo

	USER=$( echo $line | cut -d ":" -f 1 )
	NAME=$( echo $line | cut -d ":" -f 2 )
	GHUB=$( echo $line | cut -d ":" -f 3 )

	if id "$USER" >/dev/null 2>&1; then
		echo "$USER: found, doing nothing"
	else
		echo "$USER: not found, creating"
		useradd $USER -c "$NAME" -G sudo -m -s /bin/bash
	fi


	echo -n "- Downloading SSH keys from GitHub: "
	TMP_KEY_FILE="/tmp/tmp-$USER-key"
	wget -q -O $TMP_KEY_FILE "https://github.com/$GHUB.keys"
	if [ ! -f $TMP_KEY_FILE ]; then
		echo "FAIL"
		continue
	else
		echo "OK"

		# Copy ssh keys and set permissions
		mkdir -p /home/$USER/.ssh

		chown $USER:$USER /home/$USER/.ssh
		chmod 0700 /home/$USER/.ssh

		cp $TMP_KEY_FILE /home/$USER/.ssh/authorized_keys

		chown $USER:$USER /home/$USER/.ssh/authorized_keys
		chmod 0600 /home/$USER/.ssh/authorized_keys

		# Remove tmp key
		rm $TMP_KEY_FILE
	fi
done < $USERS_FILE
