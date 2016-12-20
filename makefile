MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
INSTALL_DIR = /bin/test/

bash_wrapper: bash_wrapper.c
	gcc bash_wrapper.c -o bash_wrapper

.PHONY: install

install: bash_wrapper
	echo "installation directory is" $(INSTALL_DIR)
	# rename "bash" so "bash_wrapper" can redirect commands
	mv $(INSTALL_DIR)bash $(INSTALL_DIR)bash_orig

	# create a symlink to "bash_wrapper" inplace of "bash"
	ln -s $(dir $(MKFILE_PATH))bash_wrapper $(INSTALL_DIR)bash

.PHONY: uninstall

uninstall:
	# test if symlink exist and restore original "bash"
	if [ -L $(INSTALL_DIR)bash ]; then \
		echo $(-L $(INSTALL_DIR)bash); \
		mv $(INSTALL_DIR)bash_orig $(INSTALL_DIR)bash; \
	fi

