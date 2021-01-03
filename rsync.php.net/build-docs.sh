#!/bin/bash

BASE_DIR=/home/docbuilder/phpdoc-git
cd $BASE_DIR

langs=`/usr/local/bin/php -r 'include "/local/mirrors/phpweb/include/languages.inc"; echo implode( " ", array_keys( $ACTIVE_ONLINE_LANGUAGES ) );'`;

if [ ! -d $BASE_DIR ]; then
    mkdir $BASE_DIR;
fi

# Update the core module it self
cd /home/docbuilder/phpdoc-git
if [ ! -d $BASE_DIR/doc-base ]; then
    git clone https://git.php.net/repository/doc/base.git $BASE_DIR/doc-base
fi
git -C $BASE_DIR/doc-base fetch origin && git -C $BASE_DIR/doc-base reset --hard origin/master

# All translations depend on english being up2date
if [ ! -d $BASE_DIR/en ]; then
    git clone https://git.php.net/repository/doc/en.git $BASE_DIR/en
fi
git -C $BASE_DIR/en fetch origin && git -C $BASE_DIR/en reset --hard origin/master

for i in $langs; do
  # Update the translation files
  if [ ! -d /home/docbuilder/phpdoc-git/$i ]; then
    git clone https://git.php.net/repository/doc/$i.git $BASE_DIR/$i
  fi
  git -C $BASE_DIR/$i fetch origin && git -C $BASE_DIR/$i reset --hard origin/master
  /bin/bash ../build-docs-lang.sh $i >>/tmp/log-$i 2>&1;
    if [ $? -ne 0 ]; then
        echo "The $i build is broken :("
        php -r 'mail("phpdoc@lists.php.net", "Br0ken build", "The '$i'-build is br0ken. Please fix", "From: Doc Builder <phpdoc@lists.php.net>", "-fnoreply@php.net");'
    fi;
done

/usr/local/bin/php /local/bin/gen-phpweb-sqlite-db.php /local/mirrors/phpweb/backend/manual-lookup.sqlite /local/mirrors/phpweb /manual >/dev/null


