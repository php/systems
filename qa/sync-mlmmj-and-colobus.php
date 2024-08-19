#!/usr/bin/env php
<?php

$config_file = $argv[1] ?? "config";

[ $config, $groups ] = read_config($config_file);

function read_config($config_file)
{
    $fh = fopen($config_file, "r")
        or die("unable to open file '{$config_file}'\n");

    $config = [];
    $groups = [];
    $group = null;

    while ($line = fgets($fh)) {
        rtrim($line);
        /* skip comments */
        if (preg_match('/^\s*#/', $line)) {
            continue;
        }

        if (preg_match('/\}/', $line)) {
            $group = null;
            continue;
        }

        if (preg_match('/^(.+?)=>(.+)$/', $line, $match)) {
            $k = trim($match[1]);
            $v = trim($match[2]);

            if (strtolower($k) == 'include') {
            /*
             * We don't actually handle include, we don't need it here.
                [ $more_config, $more_groups ] = read_config($v);
                $config += $more_config;
                $groups += $more_groups;
            */
                continue;
            }

            if (isset($group)) {
                $groups[$group][$k] = $v;
            } else {
                $config[$k] = $v;
            }
        }

        if (preg_match('/group\s*(\S+)\s*\{/i', $line, $match)) {
            $group = $match[1];
        }
    }

    return [ $config, $groups ];
}

$should_be_active = [];

foreach (array_keys($groups) as $name) {
  $path = $groups[$name]['path'];
  if (array_key_exists('mail', $groups[$name]) || array_key_exists('followup', $groups[$name])) {
      $should_be_active[$path] = 1;
  }
}

if (!count($should_be_active)) {
    die("No active groups, aborting\n");
}

$spool = '/var/spool/mlmmj';

$dh = opendir($spool)
    or die("Can't open spool directory '{$spool}'\n");

while ($file = readdir($dh)) {
    if (str_starts_with($file, '.')) {
        continue;
    }

    $path = $spool . "/" . $file;

    if (array_key_exists($path, $should_be_active)) {
        if (file_exists($path . "/control/access")) {
            unlink($path . "/control/access");
            unlink($path . "/control/closedlistsub");
            echo "{$path} opened\n";
        }
    } else {
        if (!file_exists($path . "/control/access")) {
            touch($path . "/control/access");
            touch($path . "/control/closedlistsub");
            echo "{$path} closed\n";
        }
    }
}
