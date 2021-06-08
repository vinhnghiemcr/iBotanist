#!bin/bash


variable=`awk -F "\"*,\"*" 'END{print $2}' /Users/nhinguyen/'OneDrive - Texas Tech University'/'CS 4366 Senior Capstone'/'New CSV (Created by Sheetgo on Feb 07, 2021, 02;04 PM).csv'`
sed -i '' 's/.*author-about.*/author-about\:\ '"${variable}"'/g' _config.yml

bundle exec jekyll serve
