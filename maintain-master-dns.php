<?php

# Generate a complete bind zone (formely used for mirrors)
echo "; this file was automatically generated from systems/php.net.zone\n\n";
echo str_replace('@SERIAL@', time(), file_get_contents('/local/systems/php.net.zone'));
