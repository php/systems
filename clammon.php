<?php # $Id$
set_time_limit(0);

/* poke clamav; if we don't get a response kill it */

while (true) {

	$s = @fsockopen("127.0.0.1", 3310, $en = null, $es = null, 5);

	if ($s) {
		fwrite($s, "VERSION\r\n");
		stream_set_timeout($s, 5);
		$x = fgets($s);
		fclose($s);

		if (strlen($x)) {
			sleep(360);
			continue;
		}		
	}
	system("killall -9 clamd");
	sleep(30);
}

?>
