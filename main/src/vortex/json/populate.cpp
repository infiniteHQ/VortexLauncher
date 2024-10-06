#include "../../../include/vortex.h"
#include "../../../include/vortex_internals.h"

/**
 * @brief PopulateJSON writes JSON data to a file.
 * 
 * This function writes the given JSON data to the specified file.
 * 
 * @param json_data The JSON object containing the data to write.
 * @param file The path to the JSON file to write to.
 * @throws std::runtime_error if the file cannot be opened.
 */
VORTEX_API void VortexMaker::PopulateJSON(const nlohmann::json& json_data, const std::string &file)
{
    // Check if the directory of the file exists before opening it
    std::filesystem::path filePath(file);
    std::filesystem::path directory = filePath.parent_path();

    if (!directory.empty() && !std::filesystem::exists(directory))
    {
        // Create the directory if it does not exist
        std::filesystem::create_directories(directory);
    }

    // Open the file for writing
    std::ofstream fichier(file);

    // Check if the file is opened successfully
    if (!fichier.is_open())
    {
        // Throw an exception if the file cannot be opened
        VortexMaker::LogError("Core", "Error while opening file " + file);
        return;
    }

    // Write the JSON data to the file
    fichier << json_data.dump(4); // Pretty-print with 4 spaces indent

    // Close the file
    fichier.close();
}
