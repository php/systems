This repository holds configuration files and documentation about the systems
used for the PHP.net infrastructure.

These systems are maintained collectively by the subscribers to the systems@php.net
mailing list.

This repository is available in `/local/systems` on each physical/virtual host
(aka *boxen*). Most major packages (Apache, MariaDB, Postfix, etc) needed by
services on that boxen are installed using distribution packages.

<!-- TODO: Should specify a file for each boxen that lists installed packages -->

<!--
TODO: the use of $(hostname) on boxen is inconsistent, sometimes it includes
php.net and sometimes it does not. Should unify around $(hostname -s)?
-->

## Boxen configuration

All[^wip] hosts use `cron-box-crontab` as the crontab for the root user,
which is configured to run tasks every five minutes, hourly, daily, weekly,
and monthly. The configuration for each boxen can be found in the file
`boxen/$(hostname)`.

Configuration for the services on all[^wip] hosts is stored in a directory
in this repository named `$(hostname)`, and a script called
`$(hostname)/install-config.sh` will symlink them into place and restart the
appropriate services.

The standard `cron-box` setup will alert systems@php.net when errors are
encountered.

## Tasks

All of the jobs run through cron are stored in this repository, mostly as
shell scripts, and they are currently all located in the top level.

## Secrets

Secrets, tokens, and other configuration may be stored in `/local/this-box` on
the individual boxen, in which case the `backup-thisbox` task should be
included in the daily `cron-box` tasks for that boxen.

[^wip]: Once they've all be configured, which is a work in progress right now.
