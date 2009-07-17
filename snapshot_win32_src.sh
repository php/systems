base_www_dir=/home/web/windows.php.net/docroot/downloads/snaps

if [ $# -ne 1 ]
then
 echo "Usage: $0 <branch name>" 
 exit 1
fi

if [ -z "$1" ]
then
	echo Invalid branch name
fi

if [ "$1" = "5.2" ]
then
	branch="branches/PHP_5_2"
	webdir="php-5.2"
fi

if [ "$1" = "5.3" ]
then
	branch="branches/PHP_5_3"
	webdir="php-5.3"
fi

if [ "$1" = "6.0" ]
then
	branch="trunk"
	webdir="php-6.0"
fi

if [ "$branch" = "" ]
then
	echo Invalid branch name
	exit 1
fi
tmp_dirname=$webdir-src-`date +%Y%m%d%H%M`
svn co http://svn.php.net/repository/php/php-src/$branch $tmp_dirname &> /dev/null 
#cvs -d :pserver:cvsread@cvs.php.net:/repository export $branch -d $tmp_dirname $module &> /dev/null 

zip -r $base_www_dir/$webdir/$tmp_dirname.zip $tmp_dirname &> /dev/null
rm -rf $tmp_dirname
rm $base_www_dir/$webdir/$webdir-src-latest.zip

ln $base_www_dir/$webdir/$tmp_dirname.zip $base_www_dir/$webdir/$webdir-src-latest.zip

SSH_DIR=$base_www_dir/$webdir
cd $base_www_dir/$webdir
for VER in "5.2" "5.3" "6.0"; do
	for i in "php-$VER-src-2*.zip"; do
		list=`ls -r $i 2>/dev/null`
		cd $SSH_DIR
		count=0
		for j in $list; do
			let count=count+1
			if [ "$count" -gt "5" ]; then
				rm -f $j
			fi
		done
	done
done

exit 0

