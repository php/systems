
Every hour systems/update-everything is executed which does:
    systems/update-mirrors
        Updates the entire SVN checkout

    systems/update-phpweb-backend
        php-main-web/scripts/update-backend
            Creates phpweb/include/mirrors.inc                        (http://php-main-web/fetch/mirrors.php)
            Creates phpweb/include/countries.inc                      (http://php-main-web/fetch/countries.php)
            Creates phpweb/include/events.csv                         (http://php-main-web/fetch/events.php)

            Creates phpweb/include/pregen-events.inc                  (php-main-web/scripts/event_listing)

            Does some ip-country stuff (php-main-web/scripts/ip-to-country)

            Creates phpweb/include/pregen-news.inc & phpweb/feed.atom (php-main-web/scripts/pregen_news)

            Creates phpweb/news.rss && phpweb/conferences/news.rss    (php-main-web/scripts/rss_parser)

            Creates phpweb/include/pregen-confs.inc                   (php-main-web/scripts/conference_teaser)

            Creates:                                                  (php-main-web/scripts/php_version)
                phpweb/include/version.inc
                phpweb/include/releases.inc
                phpweb/releases.atom

            Creates phpweb/reST/cache/*                               (php-main-web/scripts/rest_render)

            Creates phpweb/mirrorkit.tgz

        Copies CHMs from the "manual-chms" rsync module

        php-main-web/scripts/update-user-notes

        Creates phpweb/include/last_updated.inc
