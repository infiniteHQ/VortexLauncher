#!/bin/sh

# Function to display usage instructions
usage() {
    echo "Usage: $0 --version=\"x.y.z\""
    exit 1
}

# Check if the version argument is provided
if [ "$#" -ne 1 ]; then
    usage
fi

# Extract the new version from the provided argument
NEW_VERSION=""
if echo "$1" | grep -q -- "--version="; then
    NEW_VERSION=$(echo "$1" | cut -d'=' -f2)
else
    usage
fi

# Path to version.conf file
VERSION_FILE="../version.conf"

# Check if version.conf exists
if [ ! -f "$VERSION_FILE" ]; then
    echo "Error: version.conf not found in the expected directory."
    exit 1
fi

# Read the old version from version.conf
OLD_VERSION=$(cat "$VERSION_FILE")

# Directories where we want to search and replace the version
TARGET_DIRS="../docs ../main"

# Process the directories
for dir in $TARGET_DIRS; do
    if [ -d "$dir" ]; then
        echo "Processing directory: $dir"
        # Find all files and replace the old version with the new one, excluding lines that contain VORTEX_VERSION_NUM
        grep -rl "$OLD_VERSION" "$dir" | xargs sed -i "/VORTEX_VERSION_NUM/!s/$OLD_VERSION/$NEW_VERSION/g"
    else
        echo "Warning: Directory $dir does not exist, skipping."
    fi
done

# Now process the CMakeLists.txt file separately
CMAKE_FILE="../CMakeLists.txt"
if [ -f "$CMAKE_FILE" ]; then
    echo "Processing CMakeLists.txt"
    sed -i "/VORTEX_VERSION_NUM/!s/$OLD_VERSION/$NEW_VERSION/g" "$CMAKE_FILE"
else
    echo "Warning: CMakeLists.txt not found, skipping."
fi

MANIFEST_FILE="../manifest.json"
if [ -f "$MANIFEST_FILE" ]; then
    echo "Processing manifest.json"
    sed -i "/VORTEX_VERSION_NUM/!s/$OLD_VERSION/$NEW_VERSION/g" "$MANIFEST_FILE"
else
    echo "Warning: manifest.json not found, skipping."
fi


# After successfully replacing the old version, update the version.conf file
echo "Updating version.conf from $OLD_VERSION to $NEW_VERSION"
echo "$NEW_VERSION" > "$VERSION_FILE"

echo "Version update complete!"
