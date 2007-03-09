# $Id: pb12.pair.com,v 1.2 2007-03-09 21:09:43 mj Exp $

# Service configuration for pb12.pair.com

export CONTINENT="us"

HOURLY="update-systems"
DAILY="update-time backup-thisbox backup-pear prune-backups"
WEEKLY=""
MONTHLY=""
