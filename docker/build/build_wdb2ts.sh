#! /bin/bash

#
#Script to build wdb2ts in a container.
#It serves as the entrypoint in the container.
#
# Option 
#  -b|--build Build wdb2ts and exit. This is the default
#  -d|--devel Configure and return to a bash prompt in the
#       build directory. 
#

REPO=test
CNAME=wdb2ts_build
CURRENTDIR=$(pwd)
BUILDDIR=${CURRENTDIR}
ARTIFACTSDIR=${CURRENTDIR}/artifacts

function usage() {

cat << USAGE

Usage:	$0 [-r [-a </path/to/artifacts>] [-s </path/to/sources>] ([-b]|[-d [-n]])]
	$0 -h
	$0 ([-b]|[-d])

	Start the wdb2ts build docker container and run the build process.
	Also, within the container, run the actual build or drop into Development mode.

	-b|--build	This switch propagates into the container to run in Build mode. 
			This is default behaviour without a switch.

	-d|--devel	This switch propagates into the container to drop into Development mode.
			Without this switch, the application is built.
	-n		Along with -d, do not run the configure step

	-r|--run	Run the $REPO:$CNAME container with appropriate volumes mounted

	-a|--artifacts	Absolute path to the 'artifacts' directory.  This will be populated with 
			the output from the build process.  Default is pwd/artifacts and the 'artifacts'
			directory will be created if not found.
	
	-s|--source	Absolute path to the 'build' directory.  This should be the checked out source.

USAGE
}

function makeArtifactsDir() {
	# Not allowed use currentdir for artifacts
	if [ "ARTIFACTSDIR" == "." ]; then
		ARTIFACTSDIR=${ARTIFACTSDIR}/artifacts
		#echo "ARTIFACTS:  $ARTIFACTSDIR"
	fi
	# Not allowed use a relative path for artifacts
	ARTIFACTSCHAR=$(echo $ARTIFACTSDIR | cut -c 1)
	if [ "$ARTIFACTSCHAR" != "/" ]; then
		ARTIFACTSDIR=${CURRENTDIR}/${ARTIFACTSDIR}
		#echo "ARTIFACTS:  $ARTIFACTSDIR"
	fi
	if [ ! -d ${ARTIFACTSDIR} ]; then
		mkdir -p ${ARTIFACTSDIR}
	fi
	# Not allowed use currentdir for artifacts
	ACDIR=$(cd $ARTIFACTSDIR && pwd)
	echo "Current:   $CURRENTDIR"
	echo "Artifacts: $ACDIR"
	pwd
	if [ ${ACDIR} == ${CURRENTDIR} ]; then
		ARTIFACTSDIR=${ACDIR}/artifacts
		mkdir -p ${ARTIFACTSDIR}
	fi
	echo "ARTIFACTS:  $ARTIFACTSDIR"
}

function makeBuildPath() {
	# Relative path not allowed for builddir
	BUILDCHAR=$(echo $BUILDDIR | cut -c 1)
	if [ "$BUILDCHAR" != "/" ]; then
		BUILDDIR=${CURRENTDIR}/${BUILDDIR}
	fi
	echo "BUILDDIR:  $BUILDDIR"
}

set -e

RUN=false
BUILD=true
CONFIGURE=true

while [[ $# -gt 0 ]]; do
	case "$1" in
		-b|--build)
			BUILD=true
			BUILDARG=$1
		;;
		-d|--devel)
			BUILD=false
			BUILDARG=$1
		;;
		-n)
			if [ $BUILD == false ]; then
				CONFIGURE=false
				BUILDARG="$BUILDARG $1"
			else
				echo "Error, -n switch requires -d"
				usage
				exit 1
			fi
		;;
		-h|--help)
			usage
			exit
		;;
		-r|--run)
			RUN=true
		;;
		-a|--artifacts)
			if [ $RUN == true ]; then
				ARTIFACTSDIR=$2
				shift
			else
				echo "Error, -a switch requires -r"
				usage
				exit 1
			fi
		;;
		-s|--source)
			if [ $RUN == true ]; then
				BUILDDIR=$2
				shift
			else
				echo "Error, -s switch requires -r"
				usage
				exit 1
			fi
		;;
		*)
		    # unknown option
		;;
	esac
	shift # past argument or value
done

if [ $RUN == true ]; then
	echo "RUN:       $RUN"
	echo "ARTIFACTS: $ARTIFACTSDIR"
	makeArtifactsDir
	echo "BUILDDIR:  $BUILDDIR"
	makeBuildPath
	echo "BUILD:     $BUILD"
	DOCKERCMD="docker run --rm -it -v ${ARTIFACTSDIR}:/artifacts -v ${BUILDDIR}:/build ${REPO}:${CNAME} ${BUILDARG}"
	echo ${DOCKERCMD}
	exec ${DOCKERCMD}
else
	echo "BUILD:     $BUILD"

	#Dependencies is compiled into /usr/local/lib and /usr/local/lib64.
	#COPY dependencies from /usr/local/lib to /artifacts/usr/local/lib 
	#mkdir -p /artifacts/usr/local/lib
	#cp -R /usr/local/lib/*.so* /artifacts/usr/local/lib
	mkdir -p /artifacts/usr/local/lib64
	cp -R /usr/local/lib64/*.so* /artifacts/usr/local/lib64

	#Always configure the build
	#/src/configure \
	#	--with-wdb2ts-configdir=/etc/metno-wdb2ts \
	#	--with-wdb2ts-logdir=/var/log/metno-wdb2ts \
	#	--with-wdb2ts-tmpdir=/var/lib/metno-wdb2ts/tmp \
	#	--prefix=/usr \
	#	--with-apxs=/usr/bin/apxs2 \
	#    --with-libcurl \
	#    --localstatedir=/var 

	if [ $CONFIGURE == true ]; then
		./autogen.sh \
		&& ./configure \
			--with-wdb2ts-configdir=/etc/httpd/conf \
			--with-wdb2ts-logdir=/var/log/wdb2ts \
			--with-wdb2ts-tmpdir=/usr/local/var/lib/metno-wdb2ts/tmp \
			--prefix=/usr/local \
			--libdir=/usr/local/lib64 \
		    --with-pic


		echo "Configure complete.  Now what?"
	fi

	echo "BUILD: $BUILD"
	if [ "$BUILD" == "false" ]; then
		# Development mode
		echo "Development mode, start fiddling..."
		# Replace this script with bash
		exec /bin/bash
	fi

	#Build wdb2ts and exit 

	make 
	make install DESTDIR=/artifacts

	# The install defaults to /usr/lib64/httpd/modules 
	# but we need /usr/local/lib64/httpd/modules
	if [ -d /artifacts/usr/local/lib64/httpd ]; then
		mv /artifacts/usr/lib64/httpd/modules/*so /artifacts/usr/local/lib64/httpd/modules/
	else
		mv /artifacts/usr/lib64/httpd /artifacts/usr/local/lib64/
	fi
	rm -rf /artifacts/usr/lib64
	chmod -R a+r /artifacts
fi
