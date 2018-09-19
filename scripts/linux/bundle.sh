#!/bin/bash

FILE="bundle"
TAR=1
GZIP=1
UNKNOWN_ARG=0
BUNDLE_ARGS=()
DEST_DIR=""
SOURCE_DIR=""
UNZIP=0
NEEDS_FIX=0
COMPRESS_LIST=""

#while [ "$#" -gt 0 ];
#do
#  echo $1
#  shift
#done

function join { local IFS="$1"; shift; echo "$*"; }

while [ "$#" -gt 0 ];
do
  case $1 in
    -b|-f|--bundle)
      export FILE=$2;
      shift;;

    -d|--dest-path)
      export DEST_DIR=$2;
      shift;;

    -r|--tar)
      export TAR=1;;

    -rz|-zr|--tar-gz)
      export TAR=1;
      export GZIP=1;;

    -s|--source-path)
      export SOURCE_DIR=$2;
      shift;;

    -u|--unzip)
      export UNZIP=1;;

    -ruz|-rzu|-urz|-uzr|-zru|-zur|--tar-gunzip)
      export TAR=1;
      export UNZIP=1;
      export GZIP=1;;

    -z|--gunzip|--gz)
      export GZIP=1;;

    -h|--help)
      echo "Arguments to $0 are:";
      echo "  -b|-f|--bundle <file>  file to save bundle files into";
      echo "  -d|--dest-path path    directory to generate bundle to";
      echo "  -h|--help              print this help menu";
      echo "  -r|--tar               tar the files into the bundle";
      echo "  -s|--source-path path  directory to read files from";
      echo "  -u|--unzip             unzip the files from the bundle";
      echo "  -z|--gz|--gunzip       gunzip the files into the bundle";
      echo ""
      echo "  All other arguments pass through to mabundle. See its ";
      echo "  help menu for details. '\$MADARA_ROOT/bin/mabundle -h'";
      exit 0;
      ;;

    *)
      BUNDLE_ARGS+=("$1");
      # echo "Passing arg $1 on to mabundle tool";;
  esac

  shift
done

IFS=" "
TAR_ARGS="cvf"

if [ $GZIP -eq 1 ]; then
  if [ "$FILE" = "bundle" ]; then
    FILE="$FILE.tgz"
  fi
else
  if [ "$FILE" = "bundle" ]; then
    FILE="$FILE.tar"
  fi
fi

if [ $UNZIP -eq 1 ]; then

  if [ -z "${DEST_DIR}" ]; then
    DEST_DIR=.
  else
    if [ ! -d "${DEST_DIR}" ]; then
      echo "mkdir ${DEST_DIR}"
      mkdir ${DEST_DIR}
    fi
  fi

  if [ ! -z "${SOURCE_DIR}" ]; then 
    FILE=${SOURCE_DIR}/${FILE}
  fi

  if [ $TAR -eq 1 ] && [ $GZIP -eq 1 ]; then
    echo tar xzvf ${FILE} -C ${DEST_DIR}
    tar xzvf ${FILE} -C ${DEST_DIR}
  else 
    echo "tar xvf ${SOURCE_DIR}/${FILE} -C ${DEST_DIR}"
    tar xvf ${SOURCE_DIR}/${FILE} -C ${DEST_DIR}
  fi
else

  if [ -z "$SOURCE_DIR" ]; then
    SOURCE_DIR="."
  else
    NEEDS_FIX=1
  fi

  if [ -z "${DEST_DIR}" ]; then
    DEST_DIR=.
  else
    NEEDS_FIX=1
    if [ ! -d "${DEST_DIR}" ]; then
        echo "mkdir ${DEST_DIR}"
        mkdir ${DEST_DIR}
    fi
  fi

  COMPRESS_LIST=${DEST_DIR}/datapackage.json

  if [ -d ${SOURCE_DIR}/bin ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/bin"
  fi

  if [ -d ${SOURCE_DIR}/data ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/data"
  fi

  if [ -d ${SOURCE_DIR}/doc ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/doc"
  fi

  if [ -d ${SOURCE_DIR}/lib ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/lib"
  fi

  if [ -d ${SOURCE_DIR}/scripts ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/scripts"
  fi

  if [ -d ${SOURCE_DIR}/src ]; then
    COMPRESS_LIST="${COMPRESS_LIST} ${SOURCE_DIR}/src"
  fi

  IFS=" "

  echo "mabundle -d ${DEST_DIR} -s ${SOURCE_DIR} ${BUNDLE_ARGS[@]}"
  mabundle -d ${DEST_DIR} -s ${SOURCE_DIR} "${BUNDLE_ARGS[@]}"

  if [ $TAR -eq 1 ]; then

    if [ $GZIP -eq 1 ]; then
      TAR_ARGS="-cvzf"
    else
      TAR_ARGS="-cvf"
    fi


    if [ ${NEEDS_FIX} -eq 1 ]; then 
      FILE=${DEST_DIR}/${FILE}
    fi

    echo "tar ${TAR_ARGS} ${FILE} ${COMPRESS_LIST}"
    tar ${TAR_ARGS} ${FILE} ${COMPRESS_LIST} --exclude=1
  else
    if [ "$FILE" = "bundle" ]; then
      FILE="$FILE.gz"
    fi

    if [ ${NEEDS_FIX} -eq 1 ]; then 
      FILE=${DEST_DIR}/${FILE}
    fi

    echo "gzip -cr ${COMPRESS_LIST} > ${FILE}"
    gzip -cr ${COMPRESS_LIST} > ${FILE}
  fi
fi
