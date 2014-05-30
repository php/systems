# $Id: pb12.pair.com,v 1.4 2007/06/15 05:47:45 mj Exp $

# Service configuration for pb12.pair.com

export CONTINENT="us"

#HOURLY="update-systems"
HOURLY="update-win-pkg-cache"
DAILY="update-time backup-pecl prune-backups"
WEEKLY=""
MONTHLY=""
