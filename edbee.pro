
TEMPLATE = subdirs

src_lib.subdir = edbee-lib

src_lib_test.subdir = edbee-test
src_lib_test.depends = src_lib

src_app.subdir = edbee-app
src_app.depends = src_lib

src_app_test.subdir = edbee-app-test
src_app_test.depends = src_lib src_app

SUBDIRS = \
	src_lib \
	src_lib_test \
	src_app \
	src_app_test

