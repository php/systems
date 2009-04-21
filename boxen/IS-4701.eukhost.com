# $Id: IS-4701.eukhost.com,v 1.1 2009-04-21 17:26:05 mj Exp $

# Service configuration for euk1.php.net

export CONTINENT="eu"

HOURLY="update-systems maintain-pear"
DAILY="update-time backup-pear prune-backups maintain-pear"
WEEKLY="maintain-pear"
MONTHLY="maintain-pear"
