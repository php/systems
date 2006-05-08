<?php
/* Developed by Ilia Alshanetsky (ilia@prohost.org) 
   Modified by Kornel Lesinski (kornel@ideadesigners.co.uk)
*/

/* Rules */
$__RULES = array(
	'source'=> array('.', 2, 2, array('.gz'=>1, '.bz2'=>1), 'header'=>'Source Distribution', 'info'=>array(
			'STABLE4'=>array('header'=>'Stable (4.4.x-dev)'),
			'STABLE5'=>array('header'=>'Stable (5.2.x-dev)'),
			'UNSTABLE'=>array('header'=>'Latest CVS (6.0-dev)'),
		)),
	'win32'=> array('win32', 8, 4, array('.zip'=>1), 'header'=>'Win32 Packages',	'info'=>array(
				'STABLE4'=>array(
					'header'=>'Stable (4.4.x-dev)', 
					'pecl'=>array('http://pecl4win.php.net/list.php/4_4','PECL extensions for the 4.4.x win32 branch'),
					'compilelog'=>'win32/compile-STABLE.log',
					'snaplog'=>'win32/snapshot-STABLE.log',
					),
				'STABLE5'=>array(
					'header'=>'Stable (5.2.x-dev)', 
					'pecl'=>array('http://pecl4win.php.net/list.php/5_1','PECL extensions for the 5.1.x branch'),
					'snaplog'=>'win32/snapshot-5.2.log',
					'debugpack'=>'/win32/php5.2-dbgpack-win32-latest.zip',
					),
				'UNSTABLE'=>array(
					'header'=>'Latest CVS (6.0-dev)', 
					'pecl'=>array('http://pecl4win.php.net/list.php/6_0','PECL extensions for the 6.0 win32 branch'),
					'snaplog'=>'win32/snapshot-6.0.log',
					'debugpack'=>'/win32/php6.0-dbgpack-win32-latest.zip',
					),
			)),
);

$__CTIME = time();
$__LB_TIME = array();


header("Content-Type:text/html;charset=ISO-8859-1");
?><!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
	<title>PHP Snapshots</title>
	<style type="text/css">
		html {font: 0.75em verdana, sans-serif;}
		body {margin:0;padding:0;font-size:100.01%}
		div.distr  {overflow:auto; clear:left; padding: 5px 10px 10px}
		div.distr div {width:33%;min-width:12em; float:left;}
		#index {display:none}
		h1 {margin:0;font-size:190%;padding: 18px 0 0 133px; min-height: 49px; background: #99c 3px 50% url(http://static.php.net/www.php.net/images/php.gif) no-repeat; border-bottom: 1px solid #336;}
		* html h1 {height: 49px;}
		#datetime {margin:0;padding:5px;color:white;text-align:right;background:#669; font-size: 9px; border-bottom: 1px solid #336;}
		
		h2,h3 {color:#006; font-size: 135%; margin:0.3em 0 0.4em;}
		h3,#nextsnap h2 {font-size: 120%;}
	
		
		ul,li {margin:0;padding:0;list-style:none;}
		li {margin-bottom:1em;}
		li.first {background: #f3f5ff; padding: 3px 0;}

		.date,.filelink {white-space:nowrap;}
		.filelink {margin-right:0.5em;}
		
		#snaps {padding: 0 0 3px 200px; position:relative;}
		* html #snaps {height:1%;}
		#nextsnap {
			position:absolute;
			width:190px; padding:0 5px;
			height:100%;
			height: expression(this.offsetParent.offsetHeight);
			left:0;top:0; border-right: 1px dashed #777; background:#f0f0f0; margin-right:-1px;
		}
		#nextsnap dt {margin-top:1em;}
		#nextsnap dd {margin:0;padding:0;}
		#nextsnap em {color: red;font-style:normal;}
		th {text-align:left;}
		
		a img {border:0;float:left;margin-right: 3px}
		p {clear:left;}
		
		#footer {padding: 2px; font-size: 0.9em; background:#ccc; border-top:#336 1px solid; text-align:right;}
	
		</style>
</head>
<body>
<h1>PHP Snapshots</h1>
<p id="datetime"><?php echo pdate(time()); ?></p>
<?php

function next_snap_prediction($current_time, $last_time, $freq, $sn_log='', $stable='')
{
	$to = ($last_time + ($freq * 3600)) - $current_time;
	
	if ($to < 0) {
		$stable = $stable ? '-STABLE' : '';
		if ($to > -600) {
			$res = '<em>any moment now.</em>';
		} else {
			$res = '<em>please consult /dev/urandom</em>';
		}
	} else {
		$h = floor($to/3600);
		$m = floor(($to - $h * 3600) / 60);
		
		if ($h) {
			$res = $h . ' hour'.($h==1?'':'s').' and ' . $m . ' minute'.($m==1?'':'s');
		} else {
			$res = $m . ' minute'.($m==1?'':'s');		
		}
	}
	
	return $res;
}

function get_ext($file_name)
{
	return strrchr($file_name, '.');
}

function get_name($file_name)
{
	return preg_replace('!^.*?/(php[\d.]+).*?\.(.+)$!','\1 (\2)',$file_name);
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
	$files = array('STABLE4'=>array(),'STABLE5'=>array(),'UNSTABLE'=>array());

	if ($dir = opendir($job[0]))
	{
		while ($file = readdir($dir)) {
		
			if ($file=='.' || $file=='..') continue;
		
			$file = $job[0] . '/' . $file;
		
			if (is_link($file) || !is_file($file) || !isset($job[3][get_ext($file)]) || strpos($file, 'dbgpack')) {
				continue;
			}
			if (strpos($file, 'php4-STABLE') || strpos($file, 'php4-win32-STABLE')) {
				$files['STABLE4'][get_ts($file)][] = $file;
			} else if (strpos($file, 'php5.2')) {
				$files['STABLE5'][get_ts($file)][] = $file;
			} else if (strpos($file, 'php6')) {
				$files['UNSTABLE'][get_ts($file)][] = $file;
			}	
		}
	
		closedir($dir);

		krsort($files['STABLE4']);
		krsort($files['STABLE5']);
		krsort($files['UNSTABLE']);
	}

	return $files;
}

echo '<ul id="index">';
foreach($__RULES as $rulename => $rules)
{
	echo '<li>'.$rules['header'].'<ul>';
	foreach(draw_directory($rules) as $section => $result)
	{
		echo '<li><a href="#'.strtolower($rulename.'-'.$section).'">'.$rules ['info'][$section]['header'].'</a></li>';
	}
	echo '</ul></li>';
}
echo '<li><a href="#nextsnap">Next snapshot prediction</a></ul><div id="snaps">';

foreach($__RULES as $rulename => $rules)
{
	echo '<div class="distr"><h2>'.$rules['header'].'</h2>';
	foreach(draw_directory($rules) as $section => $result)
	{
		echo '<div id="'.strtolower($rulename.'-'.$section).'"><h3>'.$rules ['info'][$section]['header'].'</h3>';
		echo '<ul>';
		foreach ($result as $tm => $files) 
		{
			if (!isset($__LB_TIME[$rulename.$section]))
			{
				$__LB_TIME[$rulename.$section] = $tm;
				echo '<li class="first">';
			}
			else echo '<li>';
			
			sort($files);
			foreach($files as $file) 
			{
				echo '<span class="filelink"><a href="'.$file.'">'.get_name($file).'</a> ('.print_fsize($file).'M)</span> ';
			}
			echo '<br><b>Built On:</b> <span class="date">'.pdate($tm).'</span></li>';
		}
		echo '</ul>';
		
		if (isset($rules ['info'][$section]['compilelog']))
		{
			echo '<p><a href="'.$rules ['info'][$section]['compilelog'].'">Compile log</a></p>';
		}
		if (isset($rules ['info'][$section]['snaplog']))
		{
			echo '<p><a href="'.$rules ['info'][$section]['snaplog'].'">Snapshot log</a></p>';
		}		
		if (isset($rules ['info'][$section]['debugpack']))
		{
			echo '<p><a href="'.$rules ['info'][$section]['debugpack'].'">Debug pack</a></p>';
		}	
		if (isset($rules ['info'][$section]['pecl']))
		{
			echo '<p><a href="'.$rules ['info'][$section]['pecl'][0].'"><img src="http://snaps.php.net/images/pecl-icon.png" alt="">'.$rules ['info'][$section]['pecl'][1].'</a></p>';
		}

		echo '</div>';
	}
	echo '</div>';
}

?>			

<div id="nextsnap">
	<h2>Next snapshot</h2>
	<dl><dt><b>STABLE source</b> in</dt><dd><?php 
echo next_snap_prediction($__CTIME, $__LB_TIME['sourceSTABLE4'], $__RULES['source'][1]); ?>
		</dd>	<dt><b>Latest CVS source</b> in</dt><dd><?php 
echo next_snap_prediction($__CTIME, $__LB_TIME['sourceUNSTABLE'], $__RULES['source'][2]); ?>
		</dd>			<dt><b>STABLE 4.4.x Win32</b> in</dt><dd><?php 
echo next_snap_prediction($__CTIME, $__LB_TIME['win32STABLE4'], $__RULES['win32'][1], 'win32', 1); ?>
		</dd>			<dt><b>STABLE 5.2.x Win32</b> in</dt><dd><?php 
echo next_snap_prediction($__CTIME, $__LB_TIME['win32STABLE5'], 8, 'win32', 1); ?>
		</dd>		<dt><b>Latest CVS Win32</b> in</dt><dd><?php 
echo next_snap_prediction($__CTIME, $__LB_TIME['win32UNSTABLE'], $__RULES['win32'][2], 'win32'); ?>		
		</dd></dl></div>
	</div>
	<div id="footer">
		<a href="http://cvs.php.net/viewcvs.cgi/systems/snaps_index.php?view=markup">source</a> | <a href="http://php.net">php.net</a> | <a href="http://www.php.net/anoncvs.php">anonymous CVS</a> | 		<a href="http://www.php.net/downloads.php">stable releases</a>
	</div>
</body>
</html>
