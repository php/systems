<?php

$nyear = date("Y");
$nmonth = date("m");
$nday = date("d");
$wdays = array('08', '15', '22', '29');  

function check_date($date2check) {
  global $nday, $nmonth, $nyear, $wdays;

  /* make sure the string is right. should always be. */

  if(strlen($date2check) != 8) {
    return false;
  }

  /* get data.. should always be exact positions */

  $year = substr($date2check, 0, 4);
  $month = substr($date2check, 4, 2);
  $day = substr($date2check, 6, 2);

  /* drop anything not in this year (should already be done) */

  if($year != $nyear) {
    return false;
  }
  
  /* leave this month alone */

  if($month === $nmonth) {
    return false;
  }

  if($month < $nmonth) {

    /* we'll always keep the first of every month */

     if($day === '01'){
      return false;
    }

    /* we keep weeklies for 3 months */ 

    if(($month > ($nmonth-3)) && in_array($day, $wdays)){
      return false;
    }

    /* we've filtered out what's to be kept */

    return true;
  }
    
  /* return anything we don't know about */
	return false;

}

if ($dir = @opendir("/local/backup")) {
  while (($file = readdir($dir)) !== false && ($file != '.' || $file != '..')) {
	$str = str_replace('mirrors.', '', $file);
	$str = str_replace('repository.', '', $str);
	$str = str_replace('.tar.bz2', '', $str);

	if(check_date($str)){
		`rm -f /local/backup/$file`;
	}
  }  
  closedir($dir);
}


