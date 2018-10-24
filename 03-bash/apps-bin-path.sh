
path_to_add="/usr/local/bin"
if [ -n "${PATH##*${path_to_add}}" ]
then
    export PATH=$PATH:${path_to_add}
fi
