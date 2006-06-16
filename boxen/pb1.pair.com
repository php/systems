# $Id: pb1.pair.com,v 1.2 2006-06-16 18:25:18 wez Exp $

# Service configuration for pb1.pair.com

BOOT="nntp-spambots"
HOURLY="kick-qmail"
DAILY="backup-ecelerity maintain-ecelerity prune-backups"


