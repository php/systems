<?php
# $Id$

# Generate a complete bind zone, including the mirrors

mysql_connect('localhost', 'nobody', '');
mysql_select_db('phpmasterdb');
$q = mysql_query('SELECT * from mirrors ORDER BY hostname');

echo "; this file was automatically generated from systems/php.net.zone\n\n";

echo str_replace('@SERIAL@', time(), file_get_contents('/local/systems/php.net.zone'));

echo "\n; mirrors\n";
echo "\$TTL 86400 ; 1 day\n";

if ($q) {
	while ($row = mysql_fetch_array($q)) {
		if ($row['mirrortype'] != 1 || !preg_match("!^\\w{2}\\d?.php.net$!", $row['hostname'])) {
			continue;
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


?>
