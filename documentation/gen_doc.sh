#!/bin/bash
# Version: 1.0
# Date: 2022-05-31
# This bash script generates CMSIS-Driver Documentation:
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)
# - doxygen 1.9.2
# - git
# - gh cli

set -o pipefail

DIRNAME=$(dirname $(realpath $0))
DOXYGEN=$(which doxygen)
REQ_DXY_VERSION="1.9.5"
REQUIRED_GEN_PACK_LIB="0.7.0"

############ gen-pack library ###########

function install_lib() {
  local URL="https://github.com/Open-CMSIS-Pack/gen-pack/archive/refs/tags/v$1.tar.gz"
  echo "Downloading gen_pack lib to '$2'"
  mkdir -p "$2"
  curl -L "${URL}" -s | tar -xzf - --strip-components 1 -C "$2" || \
   (echo "ERROR: Unable to download gen_pack library!"; exit 1)
}

function load_lib() {
  if [[ -d ${GEN_PACK_LIB} ]]; then
    . "${GEN_PACK_LIB}/gen-pack"
    return 0
  fi
  local GLOBAL_LIB="/usr/local/share/gen-pack/${REQUIRED_GEN_PACK_LIB}"
  local USER_LIB="${HOME}/.local/share/gen-pack/${REQUIRED_GEN_PACK_LIB}"
  if [[ ! -d "${GLOBAL_LIB}" && ! -d "${USER_LIB}" ]]; then
    echo "Required gen_pack lib not found!" >&2
    install_lib "${REQUIRED_GEN_PACK_LIB}" "${USER_LIB}"
  fi

  if [[ -d "${GLOBAL_LIB}" ]]; then
    . "${GLOBAL_LIB}/gen-pack"
  elif [[ -d "${USER_LIB}" ]]; then
    . "${USER_LIB}/gen-pack"
  else
    echo "Required gen-pack lib is not installed!" >&2
    exit 1
  fi
}

load_lib
find_git
find_ghcli
find_doxygen "${REQ_DXY_VERSION}"

#########################################

if [ -z "$VERSION" ]; then
  VERSION_FULL=$(git_describe "v")
  VERSION=${VERSION_FULL%+*}
else
  VERSION_FULL=${VERSION}
fi

echo "Generating documentation ..."

pushd $DIRNAME > /dev/null

rm -rf ${DIRNAME}/html
sed -e "s/{projectNumber}/${VERSION}/" "${DIRNAME}/arm2d.dxy.in" \
  > "${DIRNAME}/arm2d.dxy"

# git_changelog -f html > history.txt

echo "${UTILITY_DOXYGEN} arm2d.dxy"
"${UTILITY_DOXYGEN}" arm2d.dxy
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