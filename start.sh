#!/bin/bash
# Script starting all services in terminal
FRIEND_SERVICES="./friend_services"
FEED_SERVICES="./newsfeed_services"
PROFILE_SERVICES="./profile_services"

# Running friend services
echo "Starting friend services"
xterm -hold -e cd friend_services || $FRIEND_SERVICES --config-file=config.properties
echo "Starting friend services done."

# Running newsfeed services
echo "Starting newsfeed services"
xterm -hold -e cd feed_services || $FEED_SERVICES
echo "Starting newsfeed services done."

# Running friend services
echo "Starting profile services"
xterm -hold -e cd profile_services||$PROFILE_SERVICES
echo "Starting profile services done."
