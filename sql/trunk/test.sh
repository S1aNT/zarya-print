#!/bin/sh
sqlite3 -init dbbackup main.sqlite

#echo '.dump' | sqlite3 mail.sqlite >dump.sql