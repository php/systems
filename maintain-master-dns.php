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

if ($q) {
	while ($row = mysql_fetch_array($q)) {
		if ($row['mirrortype'] != 1 || !preg_match("!^\\w{2}\\d?.php.net$!", $row['hostname']) || $row['active'] != 1) {
			continue;
		}

		// For load balancing		
		if (preg_match('/\w{2}/',$row['load_balanced']) && $row['up']) {
			$ipv4 = gethostbyname($row['cname']);
			if ($ipv4 != $row['cname']) {
				echo $row['load_balanced'].' IN A '.$ipv4.PHP_EOL;
				echo 'www.'.$row['load_balanced'].' IN A '.$ipv4.PHP_EOL;
			}
		}


		if (preg_match("!^\\d+\\.\\d+\\.\\d+\\.\\d+$!", $row['cname'])) {
			$type = 'IN A';
		} else {
			$type = 'IN CNAME';
			if (substr($row['cname'], -1) != '.')
				$row['cname'] .= '.';
		}

		if (substr($row['hostname'], -1) != '.')
			$row['hostname'] .= '.';

		printf("%s %s %s\n", $row['hostname'], $type, $row['cname']);
		printf("www.%s %s %s\n", $row['hostname'], $type, $row['cname']);

	}
} else {
	exit(1);
}
