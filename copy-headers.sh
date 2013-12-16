#!/bin/sh

echo PWD = ${PWD}

echo SRCROOT = ${SRCROOT}

echo BUILT_PRODUCTS_DIR = ${BUILT_PRODUCTS_DIR}

cd ${SRCROOT}

	# -a, --archive               archive mode; same as -rlptgoD (no -H)
	# not quite doing archive mode for now
rsync -tlR --delete-after --files-from=header-bom.txt philibs ${BUILT_PRODUCTS_DIR}/include/

