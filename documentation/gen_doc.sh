#!/bin/bash
# Version: 1.0
# Date: 2022-05-31
# This bash script generates CMSIS-NN Documentation:
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)
# - doxygen 1.8.6

set -o pipefail

DIRNAME=$(dirname $(readlink -f $0))
DOXYGEN=$(which doxygen)
REQ_DXY_VERSION="1.9.2"
DESCRIBE=$(readlink -f ${DIRNAME}/../Scripts/git/git_describe.sh)
CHANGELOG=$(readlink -f ${DIRNAME}/../Scripts/git/gen_changelog.sh)


if [[ ! -f "${DOXYGEN}" ]]; then
  echo "Doxygen not found!" >&2
  echo "Did you miss to add it to PATH?"
  exit 1
else
  version=$("${DOXYGEN}" --version | sed -E 's/.*([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
  echo "DOXYGEN is ${DOXYGEN} at version ${version}"
  if [[ "${version}" != "${REQ_DXY_VERSION}" ]]; then
    echo " >> Version is different from ${REQ_DXY_VERSION} !" >&2
  fi
fi

if [ -z "$VERSION" ]; then
  VERSION_FULL=$(/bin/bash ${DESCRIBE} "v")
  VERSION=${VERSION_FULL%+*}
else
  VERSION_FULL=${VERSION}
fi

echo "Generating documentation ..."

pushd $DIRNAME > /dev/null

rm -rf ${DIRNAME}/html
sed -e "s/{projectNumber}/${VERSION}/" "${DIRNAME}/arm2d.dxy.in" \
  > "${DIRNAME}/arm2d.dxy"

# echo "${CHANGELOG} -f html > history.txt"
# /bin/bash "${CHANGELOG}" -f html 1> history.txt 2>/dev/null

echo "${DOXYGEN} arm2d.dxy"
"${DOXYGEN}" arm2d.dxy
popd > /dev/null

if [[ $2 != 0 ]]; then
  cp -f "${DIRNAME}/Doxygen_Templates/search.css" "${DIRNAME}/html/search/"
fi

projectName=$(grep -E "PROJECT_NAME\s+=" "${DIRNAME}/arm2d.dxy" | sed -r -e 's/[^"]*"([^"]+)"/\1/')
datetime=$(date -u +'%a %b %e %Y %H:%M:%S')
year=$(date -u +'%Y')
if [[ "${year}" != "2022" ]]; then 
  year="2022-${year}"
fi
sed -e "s/{datetime}/${datetime}/" "${DIRNAME}/Doxygen_Templates/footer.js.in" \
  | sed -e "s/{year}/${year}/" \
  | sed -e "s/{projectName}/${projectName}/" \
  | sed -e "s/{projectNumber}/${VERSION}/" \
  | sed -e "s/{projectNumberFull}/${VERSION_FULL}/" \
  > "${DIRNAME}/html/footer.js"

exit 0