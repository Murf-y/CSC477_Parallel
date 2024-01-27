# Script used to delete all files in a given folder if that file is a zipped file (ends with gz)

# Usage: ./clean_dups_from_zipped.sh <folder>


# Check if folder exists
if [ ! -d $1 ]; then
    echo "The first argument must be a folder"
    exit 1
fi

# For each file in the folder, if its name ends with gz, delete it
for file in $(ls $1); do
    if [ ${file: -3} == ".gz" ]; then
        rm $1/$file -v
    fi
done

