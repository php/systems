<?php
/* Developed by Ilia Alshanetsky (ilia@prohost.org) */

/* Rules */
$__RULES = array(
	'source'	=> array('.', 2, 2, array('.gz'=>1, '.bz2'=>1)),
	'win32'		=> array('win32', 8, 4, array('.zip'=>1))
);

$__CTIME = time();
$__LB_TIME = array();

function next_snap_prediction($current_time, $last_time, $freq, $sn_log='', $stable='')
{
	$to = ($last_time + ($freq * 3600)) - $current_time;
	
	if ($to < 0) {
		$stable = $stable ? '-STABLE' : '';
		if ($sn_log == 'win32' && strpos(file_get_contents("win32/snapshot".$stable.".log"), "Base build failed")) {
			$res = '<font color="red">Win32 build failed. Consult <a href="win32/compile'.$stable.'.log">compile.log</a> for failure reason.</font>';
		} else if ($to > -600) {
			$res = '<font color="red">any moment now.</font>';
		} else {
			$res = '<font color="red">please consult /dev/urandom</font>';
		}
	} else {
		$h = floor($to/3600);
		$m = floor(($to - $h * 3600) / 60);
		
		if ($h) {
			$res = $h . ' hour(s) and ' . $m . ' minute(s)';
		} else {
			$res = $m . ' minute(s)';		
		}
	}
	
	return $res;
}

function get_ext($file_name)
{
	return strrchr($file_name, '.');
}

function stz_to_ts($value)
{
	$year = substr($value, 0, 4);
	$month = substr($value, 4, 2);
	$day = substr($value, 6, 2);
	$hour = substr($value, 8, 2);
	$min = substr($value, 10, 2);

	return gmmktime($hour, $min, 0, $month, $day, $year);	
}

function print_fsize($file)
{
	return sprintf("%.1f", filesize($file)/1024/1024);
}

function get_ts($file_name)
{
	$s = strrpos($file_name, '-') + 1;
	$e = strpos($file_name, '.', $s);
	
	$tm_s = substr($file_name, $s, ($e-$s));
	
	return stz_to_ts($tm_s);
}

function pdate($time)
{
	return gmdate("M d, Y H:i T", $time);
}

function draw_directory($job)
{
	$files = array();

	$dir = opendir($job[0]);
	readdir($dir); readdir($dir);
	
	while ($file = readdir($dir)) {
		$file = $job[0] . '/' . $file;
		
		if (is_link($file) || !is_file($file) || !isset($job[3][get_ext($file)])) {
			continue;
		}
		
		if (strpos($file, 'STABLE')) {
			$files['STABLE'][get_ts($file)][] = $file;
		} elseif (strpos($file, '5.0')) {
			$files['5.0'][get_ts($file)][] = $file;
		} else {
			if (strpos($file, 'php5')) {
				$files['UNSTABLE'][get_ts($file)][] = $file;
			}
		}	
	}
	
	closedir($dir);
	
	if (isset($files['STABLE'])) {
		krsort($files['STABLE']);
	}
	
	if (isset($files['5.0'])) {
		krsort($files['5.0']);
	}

	if (isset($files['UNSTABLE'])) {
		krsort($files['UNSTABLE']);
	}

	return $files;
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<html>
<head>
<title>PHP Snapshots</title>
<style type="text/css">
<!--
body {
	margin: 2px;
}
.HeaderBorder {
	border-bottom: 1px solid black;
}
hr {
	 color: green;
	 background-color: green;
	 height: 1px;
	 border: 0px;
}
.cotents {
	font-family: Tahoma, Verdana, "Myriad Web", Syntax, sans-serif;
	font-size: 11px;
}
//-->
</style>
</head>
<body>
<div align="center">
<h1>PHP Snapshots</h1>
<table width="100%" border=0>
<tr>
	<td align="center" valign="top">
	<fieldset>
		<legend align="center">Source Distribution</legend>
	<div>
		<?php $results = draw_directory($__RULES['source']); ?>	
		<table width="100%" cellspacing=5 cellpadding=0>
			<tr style="border-bottom: 1px solid black">
				<td width="33%" class="HeaderBorder"><b>Stable (4.3.x-dev)</b></td>
				<td width="34%" class="HeaderBorder"><b>Latest CVS (5.0.x-dev)</b></td>
			</tr>
			<tr>
				<td valign="top" class="cotents">
<?php
	foreach ($results['STABLE'] as $tm => $file) {
		if (!isset($__LB_TIME['s_st'])) {
			$__LB_TIME['s_st'] = $tm;
		}
		
		sort($file);
		echo '<b>Download:</b> <a href="'.$file[0].'">'.get_ext($file[0]).'</a> ('.print_fsize($file[0]).'M) 
			<a href="'.$file[1].'">'.get_ext($file[1]).'</a> ('.print_fsize($file[1]).'M)
			<br><b>Built On:</b> ' .pdate($tm). '<br><hr>';
	}
?>
				</td>
				
				<td valign="top" class="cotents">
<?php
	foreach ($results['UNSTABLE'] as $tm => $file) {
		if (!isset($__LB_TIME['s_cv'])) {
			$__LB_TIME['s_cv'] = $tm;
		}
		sort($file);
		echo '<b>Download:</b> <a href="'.$file[0].'">'.get_ext($file[0]).'</a> ('.print_fsize($file[0]).'M) 
			<a href="'.$file[1].'">'.get_ext($file[1]).'</a> ('.print_fsize($file[1]).'M)
			<br><b>Built On:</b> ' .pdate($tm). '<br><hr>';
	}
?>				
				</td>
			</tr>
		</table>
	</div>
	</fieldset>
	</td>
</tr>
</table>
<br>

<table width="100%" border=0>
<tr>
	<td align="center" valign="top">
	<fieldset>
		<legend align="center">Win32 Package</legend>
	<div>
		<?php $results = draw_directory($__RULES['win32']); ?>	
		<table width="100%" cellspacing=5 cellpadding=0>
			<tr style="border-bottom: 1px solid black">
				<td width="33%" class="HeaderBorder"><b>Stable (4.3.x-dev)</b></td>
				<td width="33%" class="HeaderBorder"><b>Stable (5.0.x-dev)</b></td>
				<td width="33%" class="HeaderBorder"><b>Latest CVS (5.1.x-dev)</b></td>
			</tr>
			<tr>
				<td valign="top" class="cotents">
<?php
	foreach ($results['STABLE'] as $tm => $file) {
		if (!isset($__LB_TIME['w32_st'])) {
			$__LB_TIME['w32_st'] = $tm;
		}
	
		echo '<a href="'.$file[0].'">Download</a> ('.print_fsize($file[0]).'M)<br><b>Built On:</b> ' .pdate($tm). '<br><hr>';
	}
?>
		<table border=0>
		<tr><td valign="middle">
		<a href="win32/PECL_STABLE/">
		<img src="/images/pecl-icon.png" border="0" align="middle" alt="PECL">
		</a>
		</td><td valign="middle" class="cotents">
                <a href="win32/PECL_STABLE/">
 		PECL extensions for the stable win32 branch.
		</a>
		</td></tr></table>
				</td>


<td valign="top" class="cotents"> 
<?php 
foreach ($results['5.0'] as $tm => $file) { 
if (!isset($__LB_TIME['5.0'])) { 
$__LB_TIME['5.0'] = $tm; 
} 

echo '<a href="'.$file[0].'">Download</a> ('.print_fsize($file[0]).'M)<br><b>Built On:</b> ' .pdate($tm). '<br><hr>';
} 
?>
      <table border=0>
		<tr><td valign="middle">
		<a href="win32/PECL_5_0/">
		<img src="/images/pecl-icon.png" border="0" align="middle" alt="PECL">
		</a>
		</td><td valign="middle" class="cotents">
                <a href="win32/PECL_5_0/">
 		PECL extensions for the Stable 5.0 branch.
		</a>
       </td></tr></table>
</td>



				
				<td valign="top" class="cotents">
<?php
	foreach ($results['UNSTABLE'] as $tm => $file) {
		if (!isset($__LB_TIME['w32_cv'])) {
			$__LB_TIME['w32_cv'] = $tm;
		}
	
		echo '<a href="'.$file[0].'">Download</a> ('.print_fsize($file[0]).'M)<br><b>Built On:</b> ' .pdate($tm). '<br><hr>';
	}
?>				
		<table border=0>
		<tr><td valign="middle">
		<a href="win32/PECL_UNSTABLE/">
		<img src="/images/pecl-icon.png" border="0" align="middle" alt="PECL">
		</a>
		</td><td valign="middle" class="cotents">
                <a href="win32/PECL_UNSTABLE/">
 		PECL extensions for the unstable win32 branch.
		</a>
		</td></tr></table>
				</td>
			</tr>
		</table>
	</div>
	</fieldset>
	</td>
</tr>
</table>
<br>
<table border=0>
<tr>
	<td align="left" valign="top">
	<fieldset>
		<legend align="center">General Info</legend>
	<div class="cotents" align="center">
		<table cellspacing=7 cellpadding=0 border=0 width="98%" style="border: 1px dashed blue">
			<tr>
				<td> </td>
				<td class="cotents" align="center"><b>Stable 4.3.x</b></td>
				<td class="cotents" align="center"><b>Stable 5.0.x</b></td>
				<td class="cotents" align="center"><b>Latest CVS</b></td>
			</tr>
			<tr>
				<td class="cotents"><b>Win32 Package</b></td>
				<td align="center" nowrap class="cotents"><a href="win32/compile-STABLE.log">Compile Log</a> || <a href="win32/snapshot-STABLE.log">Snapshot Log</a></td>
				<td align="center" nowrap class="cotents"><a href="win32/snapshot-5.0.log">Snapshot Log</a></td>
				<td align="center" nowrap class="cotents"><a href="win32/snapshot.log">Snapshot Log</a></td>
			</tr>
		</table><br>

		<table cellspacing=7 cellpadding=0 border=0 width="98%" style="border: 1px dashed green">
			<tr><td class="cotents">
				Next <b>STABLE source snapshot</b> in: <?php echo next_snap_prediction($__CTIME, $__LB_TIME['s_st'], $__RULES['source'][1]); ?><br>
				Next <b>Latest CVS source snapshot</b> in: <?php echo next_snap_prediction($__CTIME, $__LB_TIME['s_cv'], $__RULES['source'][2]); ?><br><br>
				
				Next <b>STABLE 4.3.x Win32 snapshot</b> in: <?php echo next_snap_prediction($__CTIME, $__LB_TIME['w32_st'], $__RULES['win32'][1], 'win32', 1); ?><br>
				Next <b>STABLE 5.0.x Win32 snapshot</b> in: <?php echo next_snap_prediction($__CTIME, $__LB_TIME['5.0'], 8, 'win32', 1); ?><br>
				Next <b>Latest CVS Win32 snapshot</b> in: <?php echo next_snap_prediction($__CTIME, $__LB_TIME['w32_cv'], $__RULES['win32'][2], 'win32'); ?>
				
			</td></tr>
		</table>
	</div>
	</fieldset>
	</td>
	<td align="left" valign="top">&nbsp;</td>
</tr>
</table>
</div>
</body>
</html>
