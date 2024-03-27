#! /bin/bash
siege --delay=0.5 --file=siege_urls --internet --verbose --reps=200 --concurrent=15