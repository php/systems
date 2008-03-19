<?php # $Id$

/*

   This is a CLI script; call it thus:
   
	php gen-phpweb-sqlite-db.php database DOCUMENT_ROOT MANUAL_PREFIX [implied_lang]

   eg:
   	php gen-phpweb-sqlite-db.php database /local/Web/sites/phpweb /manual

*/

$DOCUMENT_ROOT = $argv[2];
$DOCUMENT_ROOT_LEN = strlen($DOCUMENT_ROOT);
$MANUAL_PREFIX = $argv[3];
$implied_lang = isset($argv[4]) ? $argv[4] : false;

$db_name = $argv[1];

if (!file_exists($db_name)) {
	$s = sqlite_open($db_name);

	sqlite_query($s, "CREATE TABLE fs (
lang char(5) not null,
prefix char(32) not null,
keyword char(128) not null,
name varchar(238) not null,
prio int not null
);");
	sqlite_query($s, "CREATE INDEX map ON fs (lang,keyword)");
	sqlite_query($s, "BEGIN");
} else {
	$s = sqlite_open($db_name);
	/* purge old results */
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
	static $sections = array('intro.', 'book.', 'ref.', 'function.', 'class.', 'feature-',
		'control-structures.', 'language.',
		"about.", "faq.", "features.", 
	);
	global $DOCUMENT_ROOT_LEN;
	
	$dot = 0;
	echo "Lang: $lang\n";

	$d = opendir($dir);
	if (!$d) {
		return;
	}
	readdir($d); readdir($d); // skip . and ..
	while (($f = readdir($d)) !== false) {
		if ($f === 'CVS' || $f === 'figures') {
			continue;
		}
		$file = $dir . DIRECTORY_SEPARATOR . $f;

		if (is_dir($file)) {
			scan($file, $lang);
		} else {
			/* which section/prefix does this fall under ? */
			$prefix = "";
			$keyword = $f;
		
			$doc_rel = substr($file, $DOCUMENT_ROOT_LEN);
		
			$x = strrpos($keyword, '.');
			if ($x !== false) {
				$keyword = substr($keyword, 0, $x);
			}
			
			foreach ($sections as $prio => $section) {
				if (!strncmp($f, $section, strlen($section))) {
					$keyword = substr($keyword, strlen($section));
					$prefix = $section;
					break;
				}
			}
			if (++$dot % 76 == 0) {
				$dot = 0;
				echo "\n";
			}
			echo ".";
//			echo "$lang: $keyword\n";
			sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '$keyword', '$doc_rel', $prio)");
			sqlite_query($s, "INSERT INTO fs (lang, prefix, keyword, name, prio) values ('$lang', '$prefix', '" . metaphone($keyword) . "', '$doc_rel', ".($prio+10).")");
		}
	}
	closedir($d);
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


?>
