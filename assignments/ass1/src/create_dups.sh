# Script used to create a folder with multiple copies of a file

# Usage: ./create_dups.sh <filename> <number of copies>


# Check if the number of arguments is correct
if [ $# -ne 2 ]; then
    echo "Usage: $0 <filename> <number>"
    exit 1
fi

# Check if the first argument is a file
if [ ! -f $1 ]; then
    echo "The first argument must be a file"
    exit 2
fi

# Check if the second argument is a number
if [ ! $2 -eq $2 ] 2>/dev/null; then
    echo "The second argument must be a number"
    exit 3
fi

# Check if the second argument is positive
if [ $2 -lt 0 ]; then
    echo "The second argument must be positive"
    exit 4
fi

# Check if the dups directory exists
if [ -d dups ]; then
    rm -rf dups
    echo "The dups directory already exists. It will be deleted"
fi

# Create the dups directory
mkdir dups

echo "The dups directory has been created"

echo "Starting the duplication process"
for i in $(seq 1 $2); do
    cp $1 dups/$1.$i -v
done

# Exit successfully
exit 0
