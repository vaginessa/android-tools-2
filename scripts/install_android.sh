#!/bin/bash
# Author+=amit.pundir@linaro.org

set -e

UBUNTU=`cat /etc/issue.net | cut -d' ' -f2`
HOST_ARCH=`uname -m`
if [ ${HOST_ARCH} == "x86_64" ] ; then
	PKGS='gnupg flex bison gperf build-essential zip curl zlib1g-dev libc6-dev lib32ncurses5-dev x11proto-core-dev libx11-dev lib32z1-dev libgl1-mesa-dev g++-multilib mingw32 tofrodos python-markdown libxml2-utils xsltproc vim-common python-parted python-yaml wget uuid-dev python-mako'
else
	echo "ERROR: Only 64bit Host(Build) machines are supported at the moment."
	exit 1
fi
if [[ ${UBUNTU} =~ "14.04" || ${UBUNTU} =~ "13." || ${UBUNTU} =~ "12.10" ]]; then
	#Install basic dev package missing in chrooted environments
	sudo apt-get install software-properties-common
	sudo dpkg --add-architecture i386
	PKGS+=' libstdc++6:i386 git-core'
	if [[ ${UBUNTU} =~ "14.04" ]]; then
		PKGS+=' u-boot-tools bc acpica-tools'
	elif [[ ${UBUNTU} =~ "13.10" ]]; then
		PKGS+=' u-boot-tools bc iasl'
	else
		PKGS+=' uboot-mkimage acpica-tools'
	fi
elif [[ ${UBUNTU} =~ "12.04" || ${UBUNTU} =~ "10.04" ]] ; then
	#Install basic dev package missing in chrooted environments
	sudo apt-get install python-software-properties
	if [[ ${UBUNTU} =~ "12.04" ]]; then
		PKGS+=' libstdc++6:i386 git-core'
	else
		PKGS+=' ia32-libs libssl-dev libcurl4-gnutls-dev libexpat1-dev gettext'
	fi
else
	echo "ERROR: Only Ubuntu 10.04, 12.*, 13.* and 14.04 versions are supported."
	exit 1
fi


PKGS+=' openjdk-7-jdk openjdk-7-jre'
PKGS+=' samba minicom vim vim-gnome subversion htop openssh-server astyle ctags emacs'
PKGS+=' android-tools-adb android-tools-fastboot android-tools-fsutils'
echo "${PKGS}"
#sudo apt-get update
sudo apt-get -y install ${PKGS}

