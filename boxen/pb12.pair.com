# $Id: pb12.pair.com,v 1.4 2007-06-15 05:47:45 mj Exp $

# Service configuration for pb12.pair.com

export CONTINENT="us"

HOURLY="update-systems maintain-pear"
DAILY="update-time backup-pear prune-backups maintain-pear"
WEEKLY="maintain-pear"
MONTHLY="maintain-pear"
