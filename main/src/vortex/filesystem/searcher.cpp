#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief SearchFilesRecursive searches for files recursively in a directory.
 * 
 * This function recursively searches for files in the specified directory and its subdirectories.
 * If a file with the specified filename is found, its path is added to the provided vector of filenames.
 * 
 * @param path The path of the directory to search in.
 * @param filename The name of the file to search for.
 * @param file A vector to store the paths of the found files.
 * @return The path of the first found file matching the filename, or "null" if no file is found.
 */
VORTEX_API std::string VortexMaker::SearchFilesRecursive(const fs::path &path, const std::string &filename, std::vector<std::string> &file)
{
  // Iterate through each entry in the directory
  for (const auto &entry : fs::directory_iterator(path))
  {
    // If the entry is a regular file and its filename contains the specified filename
    if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos)
    {
      // Add the path of the found file to the vector and return its path
      file.push_back(entry.path().string());
      return entry.path().string();
    }
    // If the entry is a directory, recursively search in it
    else if (entry.is_directory())
    {
      VortexMaker::SearchFilesRecursive(entry.path(), filename, file);
    }
  }
  // Return "null" if no file is found
  return "null";
}


/**
 * @brief SearchFilesRecursive searches for files recursively in a directory.
 * 
 * This function recursively searches for files in the specified directory and its subdirectories.
 * If a file with the specified filename is found, its path is added to the provided vector of filenames.
 * 
 * @param path The path of the directory to search in.
 * @param filename The name of the file to search for.
 * @param file A vector to store the paths of the found files.
 * @return The path of the first found file matching the filename, or "null" if no file is found.
 */
VORTEX_API std::string VortexMaker::SearchFilesRecursive(const fs::path &path, const std::string &filename, std::vector<std::string> &file, int recursions, int counter)
{
  if(counter >= recursions)
  return "null";

  counter++;

  // Iterate through each entry in the directory
  for (const auto &entry : fs::directory_iterator(path))
  {
    // If the entry is a regular file and its filename contains the specified filename
    if (entry.is_regular_file() && entry.path().filename().string().find(filename) != std::string::npos)
    {
      // Add the path of the found file to the vector and return its path
      file.push_back(entry.path().string());
      return entry.path().string();
    }
    // If the entry is a directory, recursively search in it
    else if (entry.is_directory())
    {
      VortexMaker::SearchFilesRecursive(entry.path(), filename, file, recursions, counter);
    }
  }
  // Return "null" if no file is found
  return "null";
}


/**
 * @brief SearchFiles searches for files in a directory.
 * 
 * This function searches for files in the specified directory and its subdirectories.
 * It returns a vector containing the paths of all files found matching the specified filename.
 * 
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the filename.
 */
VORTEX_API std::vector<std::string> VortexMaker::SearchFiles(const std::string &path, const std::string &filename)
{
  // Initialize a vector to store the paths of found files
  std::vector<std::string> fichiersTest;
  
  // Call the recursive function to search for files
  VortexMaker::SearchFilesRecursive(fs::current_path() / path, filename, fichiersTest);
  
  // Return the vector containing the paths of found files
  return fichiersTest;
}


/**
 * @brief SearchFiles searches for files in a directory.
 * 
 * This function searches for files in the specified directory and its subdirectories.
 * It returns a vector containing the paths of all files found matching the specified filename.
 * 
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the filename.
 */
VORTEX_API std::vector<std::string> VortexMaker::SearchFiles(const std::string &path, const std::string &filename, int recursions)
{
  // Initialize a vector to store the paths of found files
  std::vector<std::string> fichiersTest;
  
  // Call the recursive function to search for files
  VortexMaker::SearchFilesRecursive(fs::current_path() / path, filename, fichiersTest, recursions, 0);
  
  // Return the vector containing the paths of found files
  return fichiersTest;
}

/**
 * @brief SearchFiles searches for files in a directory.
 * 
 * This function searches for files in the specified directory and its subdirectories (in system variant).
 * It returns a vector containing the paths of all files found matching the specified filename.
 * 
 * @param path The relative path of the directory to search in.
 * @param filename The name of the file to search for.
 * @return A vector containing the paths of all files found matching the filename.
 */
VORTEX_API std::vector<std::string> VortexMaker::SearchSystemFiles(const std::string &path, const std::string &filename)
{
  // Initialize a vector to store the paths of found files
  std::vector<std::string> fichiersTest;
  
  // Call the recursive function to search for files
  VortexMaker::SearchFilesRecursive(path, filename, fichiersTest);
  
  // Return the vector containing the paths of found files
  return fichiersTest;
}
