#!/usr/bin/env bash

echo "Autogenerating xtrtest main and header files..."
testcases=$(grep --recursive --include=xtrtest_*.c --exclude-dir=framework --no-filename "xtrtest_.*\(void\)" .. | sort | uniq)
testcases_for_header=$(sed -e 's/^/void /g' -e 's/(void)/(void);/g' <<< $testcases)
testcases_for_main=$(sed -e 's/^/    /g' -e 's/(void)/();/g' <<< $testcases)

cat xtrtest.h.head > xtrtest.h || exit 1
echo "$testcases_for_header" >> xtrtest.h
cat xtrtest.h.tail >> xtrtest.h || exit 1
echo "Generated xtrtest.h"

cat xtrtest_main.c.head > xtrtest_main.c || exit 1
echo "$testcases_for_main" >> xtrtest_main.c
cat xtrtest_main.c.tail >> xtrtest_main.c || exit 1
echo "Generated xtrtest_main.c"
