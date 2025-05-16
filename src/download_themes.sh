#!/bin/bash

# Create themes directory if it doesn't exist
mkdir -p themes

# Download from Wayback Machine
wget -r -l1 -nd -P themes https://web.archive.org/web/*/dev-www.snipeoffice.org/themes* 