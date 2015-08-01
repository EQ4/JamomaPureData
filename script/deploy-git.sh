#!/bin/sh

set -e

if [ "x${TRAVIS_BRANCH}" != "xmaster" ]; then
  echo "We are not on master branch, don't upload build."
  exit 0
fi

GITDEPLOYTARGET=git@github.com:jamoma/nightly-builds.git

if [ "x${GITDEPLOYTARGET}" = "x" ]; then
 echo "no git-deploytarget defined; skipping deployment"
 exit 0
fi
GITDEPLOYHOST="${GITDEPLOYTARGET##*@}"
GITDEPLOYHOST="${GITDEPLOYHOST%%/*}"
GITDEPLOYHOST="${GITDEPLOYHOST%%:*}"

echo "GITDEPLOYTARGET ${GITDEPLOYTARGET}"
echo "GITDEPLOYHOST ${GITDEPLOYHOST}"

KEYFILE=${HOME}/.ssh/id_rsa

# check if there is an ssh keyfile
# if not, try to decrypt one; if that fails stop
if [ ! -e "${KEYFILE}" ]; then
 mkdir -p ${HOME}/.ssh
 openssl aes-256-cbc -K $encrypted_key -iv $encrypted_iv -in ${0%/*}/id_rsa.enc -out "${KEYFILE}" -d
fi
if [ ! -e "${KEYFILE}" ]; then
 echo "couldn't find ${KEYFILE}; skipping deployment"
 exit 0
fi

## config done

## password-less authentication to deploy host

# make sure our remote host is known
# (so we are not asked to accept it)
mkdir -p ~/.ssh
chmod 700 ~/.ssh
ssh-keyscan -H ${GITDEPLOYHOST} >> ~/.ssh/known_hosts || error "ssh-keyscan failed, go ahead"
echo "ssh-keyscanned ${GITDEPLOYHOST}"

# and use the (encrypted) auth key
if [ -e "${KEYFILE}" ]; then
 chmod 600 "${KEYFILE}"
 eval `ssh-agent -s`
 ssh-add "${KEYFILE}"
 echo "ssh-added ${KEYFILE}"
else
 echo "missing ${KEYFILE}"
fi

ARCHIVE_NAME="JamomaPd-${TRAVIS_OS_NAME}_${ARCH}-${TRAVIS_TAG}"

if [ "x$ARCH" = "xmingw-w64" ]; then
  ARCHIVE_NAME="JamomaPd-Windows-win32-${TRAVIS_TAG}"
elif [ "x$ARCH" = "x" -a "x$TRAVIS_OS_NAME" = "xlinux" ]; then
  ARCHIVE_NAME="JamomaPd-Linux-x86_64"
elif [ "x$ARCH" = "xrpi" ]; then
  ARCHIVE_NAME="JamomaPd-Linux-RaspberryPi"
elif [ "x$TRAVIS_OS_NAME" = "xosx" ]; then
  ARCHIVE_NAME="JamomaPd-OSX-fat_binary"
fi

if [ "x$TRAVIS_TAG" != "x" ]; then
  ARCHIVE_NAME=${ARCHIVE_NAME}-${TRAVIS_TAG}
fi

cd ${HOME}/JamomaPd-install

echo "Compress installation folder."
if [ "x$ARCH" = "xmingw-w64" ]; then
 zip -r "${TRAVIS_BUILD_DIR}/${ARCHIVE_NAME}.zip" Jamoma/
else
 tar cvzf "${TRAVIS_BUILD_DIR}/${ARCHIVE_NAME}.tar.gz" Jamoma/
fi

cd ${TRAVIS_BUILD_DIR}

echo "Clone nightly-builds"
git clone ${GITDEPLOYTARGET} --depth=1 JamomaNighltyBuilds
mkdir -p JamomaNighltyBuilds/JamomaPuredata
mv ${ARCHIVE_NAME}* JamomaNighltyBuilds/JamomaPuredata/
cd JamomaNighltyBuilds

git config user.email "travis-ci@jamoma.org"
git config user.name "Travis CI"
git config push.default matching

git add -f JamomaPuredata/${ARCHIVE_NAME}*
git commit -m "JamomaPuredata ${TRAVIS_COMMIT} ${TRAVIS_OS_NAME}/${ARCH}"
git push
