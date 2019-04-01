<?php
# $Id$

# Generate a complete bind zone, including the mirrors

mysql_connect('localhost', 'nobody', '');
mysql_select_db('phpmasterdb');

$lct = mysql_query("SELECT UNIX_TIMESTAMP(lastchecked) FROM mirrors ORDER BY lastchecked DESC LIMIT 1");
list($checktime) = mysql_fetch_row($lct);

$q = mysql_query("SELECT mirrors.*, " .
	"(DATE_SUB(FROM_UNIXTIME($checktime), INTERVAL 3 DAY) < mirrors.lastchecked) AS up " .
	"FROM mirrors ORDER BY hostname");

echo "; this file was automatically generated from systems/php.net.zone\n\n";

echo str_replace('@SERIAL@', time(), file_get_contents('/local/systems/php.net.zone'));

echo "\n; mirrors\n";
echo "\$TTL 3600 ; 1 hour\n";

$doneForCC = array();

if ($q) {
	while ($row = mysql_fetch_array($q)) {
		if ($row['mirrortype'] != 1 || !preg_match("!^\\w{2}\\d?.php.net$!", $row['hostname'])) {
			continue;
		}

		// For load balancing
		if (preg_match('/\w{2}/',$row['load_balanced'])) {
			if (!array_key_exists($row['load_balanced'], $doneForCC)) {
				echo $row['load_balanced'] . '.php.net.  IN CNAME STAR-php-net.ax4z.com.' . PHP_EOL;
				$doneForCC[$row['load_balanced']] = true;
			}
		}


		if (preg_match("!^\\d+\\.\\d+\\.\\d+\\.\\d+$!", $row['cname'])) {
			continue;
		} else {
			$type = 'IN CNAME';
			if (substr($row['cname'], -1) != '.')
				$row['cname'] .= '.';
		}

		if (substr($row['hostname'], -1) != '.')
			$row['hostname'] .= '.';

		printf("%s IN CNAME STAR-php-net.ax4z.com.\n", $row['hostname']);

	}
} else {
	exit(1);
}
