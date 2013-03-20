#!/usr/bin/env php
<?php

$url = "https://wiki.php.net/systems";
$basedir = dirname(__FILE__);

$doc = new DOMDocument;
@$doc->loadHTMLFile($url);
if (!$doc) {
  echo "Unable to parse $url into XML.\n";
  exit(127);
}

$xml = simplexml_import_dom($doc);
foreach ($xml->xpath("//a[starts-with(@title, 'systems:')]") as $node) {
  if ("del" == current($node->xpath(".."))->getName()) {
    continue;
  }

  $machine = (string)$node;
    
  if (!preg_match("/\.php\.net$/", $machine)) {
    continue;
  }

  if (is_file($basedir . "/" . $machine . ".cfg")) {
    continue;
  }
  
  $fp = fopen($basedir . "/" . $machine . ".cfg", "w");
  if (!$fp) {
    echo "Unable to open file for writing.\n";
    exit(127);
  }
  
  fputs($fp, "define host {\n");
  fputs($fp, "\tuse\t\tgeneric-host\n");
  fputs($fp, "\thost_name\t" . $machine . "\n");
  fputs($fp, "}\n\n");
  fclose($fp);

  echo "I just set up basic configuration for " . $machine . ". Please complete this.\n";      
}