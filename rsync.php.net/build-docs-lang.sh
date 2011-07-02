[docbuilder@sc1 docbuilder]$ cat build-docs-lang.sh 
#!/bin/sh

echo "Building for " $1
#cvs up -dP $1
#cvs up
date

echo "Removing old dirs"
rm -rf html php output
date

echo "Running configure"
php /home/docbuilder/phpdoc-all/doc-base/configure.php --with-lang=$1 --disable-libxml-check --disable-segfault-speed || exit 1
date

echo "Running test and build"
phd --color false --package PHP --format php --format xhtml --format bigxhtml --format tocfeed -d /home/docbuilder/phpdoc-all/doc-base/.manual.xml || exit 2
date

cd output

echo "Moving the TOC feed to correct folder"
mv php-tocfeed php-web/feeds

echo "Zipping downloadable manuals"
gzip -9 php-bigxhtml.html -c > php_manual_$1.html.gz
tar -czf php_manual_$1.tar.gz php-chunked-xhtml
date

echo "Copying manuals to correct place"
cp -p php_manual_$1.html.gz /local/mirrors/phpweb/distributions/manual/
cp -p php_manual_$1.tar.gz  /local/mirrors/phpweb/distributions/manual/
date

echo "Copying to rsync space"
cp -rp php-web /local/mirrors/phpweb/manual
cd /local/mirrors/phpweb/manual
rm -rf $1; mv php-web $1
date

echo "Done"

