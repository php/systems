<?php
/*
   +----------------------------------------------------------------------+
   | PHP Infrastucture                                                    |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: John Mertic <jmertic@php.net>                               |
   |          Edin Kadribasic <edink@php.net>                             |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Show source */
if ($_SERVER['QUERY_STRING'] === "source") {
  highlight_file($_SERVER['SCRIPT_FILENAME']);
  die();
}
ini_set( 'display_errors', 1 );
ini_set( 'error_reporting', 'E_ALL' );
/* All times on this site are UTC */
date_default_timezone_set("UTC");

/* Configure branches and distributions */
$b = array();

/* 5.3 */
$b["5.3"] = 
  array(
  "Source" => array(
    "freq" => 3600 * 2,
    "glob" => array(
      "php-5.3-dev (tar.bz2)" => "php5.3-2*.tar.bz2",
      "php-5.3-dev (tar.gz)"  => "php5.3-2*.tar.gz",
      "php-5.3-dev (tar.xz)"  => "php5.3-2*.tar.xz",
      ),
  ),
  );

$b["5.4"] = 
  array(
  "Source" => array(
    "freq" => 3600 * 2,
    "glob" => array(
      "php-5.4-dev (tar.bz2)" => "php5.4-2*.tar.bz2",
      "php-5.4-dev (tar.gz)"  => "php5.4-2*.tar.gz",
      "php-5.4-dev (tar.xz)"  => "php5.4-2*.tar.xz",
      ),
  ),
  );

$b["5.5"] =
  array(
  "Source" => array(
    "freq" => 3600 * 2,
    "glob" => array(
      "php-5.5-dev (tar.bz2)" => "php5.5-2*.tar.bz2",
      "php-5.5-dev (tar.gz)"  => "php5.5-2*.tar.gz",
      "php-5.5-dev (tar.xz)"  => "php5.5-2*.tar.xz",
      ),
  ),
  );

$b["5.6"] =
  array(
  "Source" => array(
    "freq" => 3600 * 2,
    "glob" => array(
      "php-5.6-dev (tar.bz2)" => "php5.6-2*.tar.bz2",
      "php-5.6-dev (tar.gz)"  => "php5.6-2*.tar.gz",
      "php-5.6-dev (tar.xz)"  => "php5.6-2*.tar.xz",
      ),
  ),
  );

/* Git master */
$b["master"] = 
  array(
  "Source" => array(
    "freq" => 3600 * 2,
    "glob" => array(
      "php-master-dev (tar.bz2)" => "php-master-2*.tar.bz2",
      "php-master-dev (tar.gz)"  => "php-master-2*.tar.gz",
      "php-master-dev (tar.xz)"  => "php-master-2*.tar.xz",
      ),
  ),
  );

function get_file_info($glob, $freq = 0)
{
  static $cache = array();

  if (isset($cache[$glob])) {

    return $cache[$glob];

  } else {

    $ret = new stdClass;

    $g = glob($glob);

    $test = trim(`ls $glob`);
    $g = split( "\n", $test );
    if (!is_array($g)) {
      return $ret;
    }

    $g = array_reverse($g);

    $ret->files = array();
    $now = time();
    $first = true;

    foreach($g as $filename) {

      $f = new stdClass;

      $f->name = $filename;
      $f->size = @filesize($f->name);
      $f->size_str = sprintf("%.1f",$f->size/1024/1024);

      if (preg_match("((\\d{12}))", $f->name, $m)) {
	$t = strtotime($m[1]);
	$f->time = $t;
	$f->time_str = str_replace(" ", "&nbsp;", date("M d, Y H:i T", $f->time));

	if ($freq) {
	  
	  $to = $t + $freq - $now;

	  if ($to < 0) {
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

	  $f->next = $res;
	}

      }

      $ret->files[] = $f;
    }

    return $ret;
  }
}

function display_next_snap($conf)
{
  foreach($conf as $branch_name => $branch) {
    foreach($branch as $distro_name => $distro) {
      foreach($distro["glob"] as $glob) break;
      $info = get_file_info($glob, $distro["freq"]);
      echo "<dt><b>PHP {$branch_name} {$distro_name}</b> in</dt><dd>{$info->files[0]->next}</dd>";
    }
  }
}

// Testing. Use 404 eventually, and probably better downloading mechanism.
// Example usage: http://snaps.php.net/?latest
// Example usage of tomorrow: http://snaps.php.net/latest
// @todo consider adding information to $b, and using that instead
$qs = trim($_SERVER['QUERY_STRING']);
if (!empty($qs)) {
  $shortcuts = array(
    'php-master' => array('master', 'latest', 'latestmaster'),
    'php5.3'    => array('53', 'latest53'),
    'php5.4'    => array('54', 'latest54'),
    'php5.5'	=> array('55', 'latest55'),
  );
  foreach ($shortcuts as $sc_branch_name => $sc_options) {
    if (in_array($qs, $sc_options)) {
      header('HTTP/1.1 302 Found');
      header('Location: http://snaps.php.net/'. $sc_branch_name . '-latest.tar.bz2');
      exit;
    }
  }
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
                      "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
    <head>
        <title>PHP Sources Snapshots</title>
        <style type="text/css" media="all">
        @import url("http://static.php.net/www.php.net/styles/site.css");
        @import url("http://static.php.net/www.php.net/styles/phpnet.css");
        </style>
        <!--[if IE]><![if gte IE 6]><![endif]-->
        <style type="text/css" media="print">
        @import url("http://static.php.net/www.php.net/styles/print.css");
        </style>
        <!--[if IE]><![endif]><![endif]-->
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
        <link rel="shortcut icon" href="http://static.php.net/www.php.net/favicon.ico" />
    </head> 
    <body>
        <div id="headnav">
            <a href="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>"><img src="http://static.php.net/www.php.net/images/php.gif"
            alt="PHP" width="120" height="67" /></a>
            <div id="headmenu">
                <a href="http://www.php.net/downloads.php">downloads</a> |
                <a href="http://www.php.net/docs.php">documentation</a> |
                <a href="http://www.php.net/FAQ.php">faq</a> |
                <a href="http://www.php.net/support.php">getting help</a> |
                <a href="http://www.php.net/mailing-lists.php">mailing lists</a> |
                <a href="http://bugs.php.net/">reporting bugs</a>
            </div>
        </div>
        <div id="headsearch" style="margin-bottom: 0;">
            <p>
                <span><?php echo date("M d, Y H:i T") ?></span>
            </p>
        </div>
        <div id="layout_2">
            <div id="leftbar">
                <h3>Snapshot Builds</h3>
                <p>
                    You can find the latest development sources
                    packages for PHP 5, and master here. Source code is 
                    checked out of Git and packaged every 2 hours.
                </p>
<!--
                <p>
		   <strong> For Windows builds, see <a href="http://windows.php.net/snapshots/">Windows Snapshot page</a>.</strong>
                </p>
-->
                <p>
                    These packages are <strong>NOT</strong> intended for 
                    production use; please use the packages at 
                    <a href="http://www.php.net/downloads">PHP downloads</a>.
                </p>

                <p>
                 URL shortcuts: Downloading the latest snapshot for specific PHP branches is easy.
                 Example URLs:
                 <a href="?53">http://snaps.php.net/?53</a> (latest 5.3 snap),
                 <a href="?master">http://snaps.php.net/?master</a> (latest master snap), and 
                 URLs like <a href="php-master-latest.tar.bz2">php-master-latest.tar.bz2</a> also
                 work.
                </p>
                
                <h3>Next snapshot</h3>
                <dl><?php display_next_snap($b); ?></dl>

            </div>
            
            <div id="content" class=".">
                <div id="snaps">
                    <h1>PHP Snapshots</h1>
<?php   
if (isset($_GET['branch']) && isset($_GET['distro']) && isset($b[$_GET['branch']][$_GET['distro']])) {

  $distro = $b[$_GET['branch']][$_GET['distro']];

  echo "<h2>PHP {$_GET['branch']} {$_GET['distro']}</h2>\n";

  echo '<table border="0" cellpadding="3" cellspacing="2" class="standard"><tr>';

  foreach ($distro["glob"] as $title => $glob) {
    echo "<th>$title</th>\n";
  }

  echo "</tr><tr>";

  foreach ($distro["glob"] as $title => $glob) {
    $info = get_file_info($glob);
    echo "<td valign=\"top\">";
    for ($i=0; $i<count($info->files); $i++) {
      echo "<span><a href=\"{$info->files[$i]->name}\">{$title}</a></span><br />\n";
      echo "<span style=\"font-size: .8em\">Size: {$info->files[$i]->size_str}M</span><br />\n";
      echo "<span style=\"font-size: .8em\">{$info->files[$i]->time_str}</span><br />\n";
      echo "<br />";
    }
    echo "</td>";
  }

  echo "</tr></table>";

} else {

  foreach($b as $branch_name => $branch) {

    echo "<h2>PHP $branch_name</h2>\n";
    echo '<table border="0" cellpadding="3" cellspacing="2" class="standard">';
    echo "<tr>\n";

    foreach($branch as $distro_name => $distro) {
      echo "<th>$distro_name</th>";
    }

    echo "</tr>\n<tr>\n";

    foreach($branch as $distro_name => $distro) {

      echo "<td valign=\"top\">\n";
      
      foreach ($distro["glob"] as $title => $glob) {
	$info = get_file_info($glob);
	$f = $info->files[0];

	echo "<span><a href=\"{$f->name}\">{$title}</a> ({$f->size_str}M)</span><br />\n";
      }
      
      echo "<br /></td>\n";
      
    }

    echo "</tr>\n<tr>\n";

    foreach($branch as $distro_name => $distro) {
      echo "<td>\n";
      foreach ($distro["glob"] as $glob) break;
      $info = get_file_info($glob);
      $f = $info->files[0];

      echo "<b>Built on:</b> " . $f->time_str ."<br />\n";
      echo "<span class=\"filelink\"><a href=\"" . htmlentities($_SERVER['PHP_SELF']). "?branch=" . urlencode($branch_name) . "&distro=" . urlencode($distro_name) . "\">Previous Builds</a></span><br />\n";

      echo "</td>\n";
      
    }

    echo "</tr>\n</table>\n";

  }
}   

?>
                </div>
            </div>
            <div class="cleaner">&nbsp;</div>
        </div>
        
        <div id="footnav">
            <a href="<?php echo htmlentities($_SERVER['PHP_SELF']) . "?source" ?>">show source</a>   
        </div>
        
        <div id="pagefooter">
            <div id="copyright">
                <a href="http://www.php.net/copyright.php">Copyright &copy;2001-<?php echo date('Y'); ?> The PHP Group</a><br />
                All rights reserved.
            </div>
        
            <div id="thismirror">
                <br />
                Last updated: <?php echo  date("M d, Y H:i T", filemtime($_SERVER['SCRIPT_FILENAME'])) ?>
            </div>
        </div> 
    </body> 
</html> 
