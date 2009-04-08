<?php
/*
 $Id$

This script scans files within the generated PHP Manual (phpweb theme)
and creates shortcut lookups for them with their metaphone based
companions. Mirrors with SQLite then check against this data. So,
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

if (!file_exists($db_name)) {
	if (!$s = sqlite_open($db_name)) {
		return;
	}
	create_url_database($s);
} else {
	if (!$s = sqlite_open($db_name)) {
		return;
	}
	sqlite_query($s, "BEGIN");
	sqlite_query($s, "delete from fs");
}

if ($implied_lang !== false) {
	scan($DOCUMENT_ROOT . $MANUAL_PREFIX, $implied_lang);
} else {
	scan_langs($DOCUMENT_ROOT . $MANUAL_PREFIX);
}

sqlite_query($s, "COMMIT");
exit(0);

function scan($dir, $lang)
{
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
		$skips = array('CVS', 'images', 'toc', '.', '..');
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
				'mysqli-result.' => 'mysqli-',
				'mysqli.'        => 'mysqli-',
			);
			foreach ($hackme as $class => $procedural) {
				if (false !== strpos($keyword, $class)) {
					$tmp = str_replace($class, $procedural, $keyword);
					sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '$tmp', '$doc_rel', " . ($prio+5).")");
					sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '". metaphone($tmp) ."', '$doc_rel', " . ($prio+15).")");
					break;
				}
			}

			++$count;

			sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '$keyword', '$doc_rel', $prio)");
			sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '" . metaphone($keyword) . "', '$doc_rel', ".($prio+10).")");

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
		if ($f === 'CVS') {
			continue;
		}
		$file = $root . DIRECTORY_SEPARATOR . $f;

		if (is_dir($file)) {
			scan($file, $f);
		}
	}
	closedir($d);
}

function create_url_database ($s) {
	$q = "
		CREATE TABLE fs (
			lang char(5) not null,
			prefix char(32) not null,
			keyword char(128) not null,
			name varchar(238) not null,
			prio int not null
		);
		";
	sqlite_query($s, $q);
	sqlite_query($s, "CREATE INDEX map ON fs (lang,keyword)");
	sqlite_query($s, "BEGIN");
}