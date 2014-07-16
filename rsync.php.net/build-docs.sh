#!/bin/bash

cd /home/docbuilder/phpdoc-all

## This doesn't do anything at all.. why is it here?
svn up --ignore-externals >/tmp/log-svn 2>&1 
# Update the core module it self
svn up doc-base >/tmp/log-svn 2>&1 
# All translations depend on english being up2date
svn up en >/tmp/log-svn 2>&1 

langs=`/usr/local/bin/php -r 'include "/local/mirrors/phpweb/include/languages.inc"; echo implode( " ", array_keys( $ACTIVE_ONLINE_LANGUAGES ) );'`;

for i in $langs; do
# Update the translation files
    svn up $i >/tmp/log-$i 2>&1
    /bin/bash ../build-docs-lang.sh $i >>/tmp/log-$i 2>&1;
    if [ $? -ne 0 ]; then
        echo "The $i build is broken :("
        php -r 'mail("phpdoc@lists.php.net", "Br0ken build", "The '$i'-build is br0ken. Please fix", "From: Doc Builder <phpdoc@lists.php.net>", "-fnoreply@php.net");'
    fi;

done;

echo "Remove old languages from rsync space"
cd /local/mirrors/phpweb/manual
read -d '' SCRIPT <<"EOF"
include "../include/languages.inc";
$current_dirs   = array_map("basename", glob("??", GLOB_ONLYDIR));
$dirs_to_delete = array_intersect($current_dirs, array_keys($INACTIVE_ONLINE_LANGUAGES));
echo implode(" ", $dirs_to_delete);
EOF
stale_langs=`/usr/local/bin/php -r "${SCRIPT}"`
rm -rf $stale_langs

/usr/local/bin/php /local/bin/gen-phpweb-sqlite-db.php /local/mirrors/phpweb/backend/manual-lookup.sqlite /local/mirrors/phpweb /manual >/dev/null


