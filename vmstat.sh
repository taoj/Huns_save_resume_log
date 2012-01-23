#! /bin/bash

echo " date : $(date)"  >> /var/log/Huns/vmstat.log

vmstat 1 5 >> /var/log/Huns/vmstat.log
