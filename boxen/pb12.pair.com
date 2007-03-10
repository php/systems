# $Id: pb12.pair.com,v 1.3 2007-03-10 12:56:10 mj Exp $

# Service configuration for pb12.pair.com

export CONTINENT="us"

HOURLY="update-systems maintain-pear"
DAILY="update-time backup-thisbox backup-pear prune-backups maintain-pear"
WEEKLY="maintain-pear"
MONTHLY="maintain-pear"
