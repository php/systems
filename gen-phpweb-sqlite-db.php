<?php
/*
 $Id$

This script scans files within the generated PHP Manual (phpweb theme)
and creates shortcut lookups for them with their metaphone based
companions. Mirrors with pdo_sqlite then check against this data. So,
this allows the likes of "php.net/foo" to work.

This script is known to be imperfect, and future plans will use PhD to
generate this data based on XML ids thus including ini settings,
predefined constants, functions, etc.

Usage:
	php gen-phpweb-sqlite-db.php database DOCUMENT_ROOT MANUAL_PREFIX [implied_lang]

Example:
	php gen-phpweb-sqlite-db.php database /local/Web/sites/phpweb /manual

Each week this information is rendered on the rsync box.
*/

$DOCUMENT_ROOT		= $argv[2];
$DOCUMENT_ROOT_LEN	= strlen($DOCUMENT_ROOT);
$MANUAL_PREFIX		= $argv[3];
$implied_lang		= isset($argv[4]) ? $argv[4] : false;
$db_name			= $argv[1];

if (file_exists($db_name)) {
	unlink($db_name);
}

$dbh = new PDO( "sqlite:$db_name", '', '' );
$dbh->setAttribute( PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION );

$sql = "CREATE TABLE fs (
			lang char(5) not null,
			prefix char(32) not null,
			keyword char(128) not null,
			name varchar(238) not null,
			prio int not null
		);";

try {
	$res = $dbh->query( $sql );
	$res = $dbh->query( 'CREATE INDEX map ON fs (lang,keyword)' );

	// Insert the underscore function, _(), which is an alias to gettext(), before anything else (Bug #63490)
	$dbh->exec("INSERT INTO fs(lang,prefix,keyword,name,prio) VALUES('en','function.','_','/manual/en/function.gettext.php',1)");

} catch ( PDOException $e ) {
	echo 'Error: Cannot create db table. Here is the error message: ' . $e->getMessage() . PHP_EOL;
	exit;
}

if (empty($DOCUMENT_ROOT) || empty($MANUAL_PREFIX) || empty($db_name)) {
	echo 'Usage:' . PHP_EOL;
	echo '  php gen-phpweb-sqlite-db.php database DOCUMENT_ROOT MANUAL_PREFIX [implied_lang]' . PHP_EOL;
	echo '  php gen-phpweb-sqlite-db.php database /local/Web/sites/phpweb /manual' . PHP_EOL;
	exit;
}

$dbh->beginTransaction();

if ($implied_lang !== false) {
	scan($DOCUMENT_ROOT . $MANUAL_PREFIX, $implied_lang);
} else {
	scan_langs($DOCUMENT_ROOT . $MANUAL_PREFIX);
}

$dbh->commit();
exit(0);

function scan($dir, $lang)
{
	global $dbh;
	global $s;
	global $DOCUMENT_ROOT_LEN;
	static $sections = array(
		'book.', 'ref.', 'function.', 'class.', 'feature-',
		'control-structures.', 'language.',
		'about.', 'faq.', 'features.',
	);
	
	$count = 0;
	echo "Lang: $lang\n";

	$d = opendir($dir);
	if (!$d) {
		return;
	}

	while (($f = readdir($d)) !== false) {

		// Directories to skip
		$skips = array('.svn', 'feeds', 'images', 'toc', '.', '..');
		if (in_array($f, $skips)) {
			continue;
		}

		$file = $dir . DIRECTORY_SEPARATOR . $f;

		if (is_dir($file)) {
			scan($file, $lang);
		} else {
			/* which section/prefix does this fall under ? */
			$prefix = "";
			$keyword = $f;

			// Get filepath relative to the manual/
			$doc_rel = substr($file, $DOCUMENT_ROOT_LEN);

			// Keyword is filename minus the extension
			$x = strrpos($keyword, '.');
			if ($x !== false) {
				$keyword = substr($keyword, 0, $x);
			}
			
			// Skip PHP 4 domxml (book.domxml). It uses function. syntax, unlike book.dom
			if (0 === strpos($keyword, 'function.dom') && false === strpos($keyword, 'simplexml')) {
				continue;
			}
			if (0 === strpos($keyword, 'function.xpath') || 0 === strpos($keyword, 'function.xptr')) {
				continue;
			}

			/* Example:
			- section:	book.
			- keyword:	about.formats
			- f:		about.formats.php
			- doc_rel:	/en/about.formats.php
			- x: 13
			*/
			foreach ($sections as $prio => $section) {
				/* Example:
				- before: keyword: about.formats prefix:        section: about. f: about.formats.php
				- after:  keyword: formats       prefix: about. section: about. f: about.formats.php
				*/
				if (!strncmp($f, $section, strlen($section))) {
					$keyword = substr($keyword, strlen($section));
					$prefix = $section;
					break;
				}
			}

			// Hack until PhD generates ids from the DocBook files, and error.php uses them
			// $hackme contains class [method] prefixes with their function counterparts
			$hackme = array(
				'mysqli.'        => 'mysqli-',
				'mysqli-result.' => 'mysqli-',
				'mysqli-stmt.'   => 'mysqli-stmt-'
			);
			foreach ($hackme as $class => $procedural) {
				if (false !== strpos($keyword, $class)) {
					$tmp = str_replace($class, $procedural, $keyword);
					$dbh->exec("INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '$tmp', '$doc_rel', " . ($prio+5).")");
					$dbh->exec("INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '". metaphone($tmp) ."', '$doc_rel', " . ($prio+15).")");
					break;
				}
			}

			++$count;

			$dbh->exec("INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '$keyword', '$doc_rel', $prio)");
			$dbh->exec("INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '" . metaphone($keyword) . "', '$doc_rel', ".($prio+10).")");

		}
	}
	closedir($d);
	
	echo "Added entries for $count files\n";
	echo "\n";
}

function scan_langs($root)
{
	global $skip_dirs;

	$d = opendir($root);
	if (!$d) {
		return;
	}
	readdir($d); readdir($d);
	while (($f = readdir($d)) !== false) {
		if ($f === '.svn') {
			continue;
		}
		$file = $root . DIRECTORY_SEPARATOR . $f;

		if (is_dir($file)) {
			scan($file, $f);
		}
	}
	closedir($d);
}

