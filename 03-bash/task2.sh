#!/bin/bash
# INSTALLER for task1.sh

# check lang
LNG='en'
SYS_LANG=${LANG::5}
if [ "$SYS_LANG" = "uk_UA" ]
then
   LNG='ua'
fi

# check and parse command line arguments
for arg
do
  if [ $arg = '-h' ] || [ $arg = '--help' ]
  then
    if [ $LNG = 'ua' ]
    then
      echo 'Завдання03: учбовий скрип shell: інсталляція скрипта task1.sh інформації про систему'
    else
      echo 'Lesson03: educative shell script: system info task1.sh installator'
    fi
    exit 0
  else
    if [ $LNG = 'ua' ]
    then
      echo 'Некоректні параметри командного рядка' 1>&2
    else
      echo 'Illegal command line arguments' 1>&2
    fi
    exit 1
  fi
done

DIR=/usr/local/bin
PROGNAME=task1.sh

if [ ! -f "$PROGNAME" ]
then
    if [ $LNG = 'ua' ]
    then
      echo "Програмний файл $PROGNAME не знайдено" 1>&2
    else
      echo "Can't find file $PROGAME" 1>&2
    fi
    exit 1
fi

echo "Installing $PROGNAME into $DIR..."
mkdir -p "$DIR"
if [ $? -ne 0 ]
then
  if [ $LNG = 'ua' ]
  then
    echo "Неможливо збудувати повний путь '$DIR'." 1>&2
  else
    echo "Can't create path: '$DIR'." 1>&2
  fi
  exit 2
fi

cp "$PROGNAME" "$DIR/"
#check copy result
if [ $? -ne 0 ]
then
  if [ $LNG = 'ua' ]
  then
    echo "Неможливо встановити файл $PROGNAME у каталог '$DIR'." 1>&2
  else
    echo "Can't install $PROGNAME into '$DIR'." 1>&2
  fi
  exit 2
fi

#set read and execute for all. write only for owner
chmod 755 "$DIR/$RPOGNAME"

#add $DIR to system path
./addsyspath.sh "$DIR" "path_to_task1.sh"

echo "Done.Buy!"
