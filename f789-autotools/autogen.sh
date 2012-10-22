#!/bin/bash
touch README NEWS ChangeLog
mkdir -p m4
autoreconf --force --install
