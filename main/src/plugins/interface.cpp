#include "../../include/plugins/interface.h"

#include "../../include/vortex.h"
#include "../../include/vortex_internals.h"

/**
 * @brief Adds a PluginFunction to the PluginInterface.
 *
 * This function creates a shared_ptr to the provided PluginFunction and adds it to the PluginInterface's list of functions.
 *
 * @param function The PluginFunction to add.
 */
void PluginInterface::AddPluginFunction(const PluginFunction &function) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(function);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a PluginOutputEvent to the PluginInterface.
 *
 * This function creates a shared_ptr to the provided PluginOutputEvent and adds it to the PluginInterface's list of output
 * events.
 *
 * @param event The PluginOutputEvent to add.
 */
void PluginInterface::AddPluginOutputEvent(const PluginOutputEvent &event) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(event);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

void PluginInterface::AddPluginInputEvent(const PluginInputEvent &event) {
}

/**
 * @brief Adds a PluginRenderInstance to the PluginInterface.
 *
 * This function adds the provided shared_ptr to a PluginRenderInstance to the PluginInterface's list of render instances.
 *
 * @param renderInstance The shared_ptr to the PluginRenderInstance to add.
 */
void PluginInterface::AddPluginRenderInstance(const std::shared_ptr<PluginRenderInstance> &renderInstance) {
  // Add the shared_ptr to the list of render instances
  this->m_render_instances.push_back(renderInstance);
}

/**
 * @brief Adds an argument to the PluginInterface's argument container.
 *
 * This function adds a key-value pair to the PluginInterface's argument container.
 * If the container does not exist, it creates a new one and adds the argument.
 *
 * @tparam T The type of the argument value.
 * @param key The key for the argument.
 * @param value The value of the argument.
 */
template<typename T>
void PluginInterface::AddArg(const std::string &key, T value) {
  if (this->m_args == nullptr) {
    // Create a new argument container if it doesn't exist
    std::shared_ptr<hArgs> args = std::make_shared<hArgs>();
    this->m_args = args;
  }

  // Add the argument to the container
  this->m_args->add<T>(key.c_str(), value);
}

/**
 * @brief Sets the logo for the PluginInterface.
 *
 * This function sets the logo data and size for the PluginInterface.
 *
 * @param hexa Pointer to the logo data as an array of uint8_t.
 * @param size Size of the logo data in bytes.
 */
void PluginInterface::AddLogo(const uint8_t *hexa, size_t size) {
  // Set the logo data and size
  this->m_logo = hexa;
  this->m_logo_size = size;
}

void PluginInterface::AddLogo(const std::string &relative_path) {
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
void PluginInterface::AddFunction(void (*item)(), const std::string &name) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(item, name);

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds a function to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginFunction and adds it to the PluginInterface's list of functions.
 *
 * @param item Pointer to the function.
 * @param name Name of the function.
 */
void PluginInterface::AddFunction(
    void (*item)(const std::shared_ptr<hArgs> &args),
    const std::string &name,
    const std::string &description,
    const std::vector<std::tuple<std::string, std::string, std::string>> &args_def,
    const bool &can_callback) {
  // Create a shared_ptr to the PluginFunction
  std::shared_ptr<PluginFunction> p_function = std::make_shared<PluginFunction>(item, name);

  p_function->m_description = description;
  p_function->m_params_def = args_def;
  p_function->m_can_callback = can_callback;

  // Add the shared_ptr to the list of functions
  this->m_functions.push_back(p_function);
}

/**
 * @brief Adds an input event to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginInputEvent and adds it to the PluginInterface's list of input events.
 *
 * @param item Pointer to the input event function.
 * @param name Name of the input event.
 */
void PluginInterface::AddInputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name) {
  // Checking if the event already exist
  for (auto existing_events : this->m_input_events) {
    if (name == existing_events->m_name) {
      this->LogError("\"" + name + "\" event already registered ! Abording.");
      return;
    }
  }

  // Create a shared_ptr to the PluginInputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(item, name);

  // Add the shared_ptr to the list of input events
  this->m_input_events.push_back(p_event);
}

void PluginInterface::AddInputEvent(
    void (*item)(const std::shared_ptr<hArgs> &args),
    const std::string &name,
    PlugDevFlag devflag,
    const std::string &description,
    const std::vector<std::tuple<std::string, std::string, std::string>> &args_def,
    const bool &can_callback) {
  // Checking if the event already exist
  for (auto existing_events : this->m_input_events) {
    if (name == existing_events->m_name) {
      this->LogError("\"" + name + "\" event already registered ! Abording.");
      return;
    }
  }

  // Create a shared_ptr to the PluginInputEvent
  std::shared_ptr<PluginInputEvent> p_event = std::make_shared<PluginInputEvent>(item, name);

  p_event->m_can_callback = can_callback;
  p_event->m_devflag = devflag;
  p_event->m_description = description;
  p_event->m_params = args_def;

  // Add the shared_ptr to the list of input events
  this->m_input_events.push_back(p_event);
}

/**
 * @brief Adds an output event to the PluginInterface.
 *
 * This function creates a shared_ptr to a PluginOutputEvent and adds it to the PluginInterface's list of output events.
 *
 * @param item Pointer to the output event function.
 * @param name Name of the output event.
 */
void PluginInterface::AddOutputEvent(void (*item)(const std::shared_ptr<hArgs> &args), const std::string &name) {
  // Create a shared_ptr to the PluginOutputEvent
  std::shared_ptr<PluginOutputEvent> p_event = std::make_shared<PluginOutputEvent>(item, name);

  // Add the shared_ptr to the list of output events
  this->m_output_events.push_back(p_event);
}

/**
 * @brief Gets a shared_ptr to a copy of the PluginInterface.
 *
 * This function creates a shared_ptr to a new PluginInterface object, copying the contents
 * of the current PluginInterface instance.
 *
 * @return A shared_ptr to a copy of the PluginInterface.
 */
std::shared_ptr<PluginInterface> PluginInterface::GetInterface() {
  // Create a shared_ptr to a new PluginInterface object, copying the current instance
  return std::make_shared<PluginInterface>(*this);
}

/**
 * @brief Gets an editor plugin by name.
 *
 * This function searches for an editor plugin with the specified name
 * and returns a shared_ptr to it if found.
 *
 * @param name The name of the editor plugin to search for.
 * @return A shared_ptr to the editor plugin if found, otherwise nullptr.
 */
std::shared_ptr<PluginInterface> PluginInterface::GetEditorPluginByName(const std::string &name) {
  VxContext *ctx = VortexMaker::GetCurrentContext();

  for (auto ep : ctx->IO.ep) {
    if (name == ep->m_name) {
      // Return a shared_ptr to the editor plugin if found
      return ep;
    }
  }

  // Return nullptr if editor plugin with the specified name is not found
  return nullptr;
}

/**
 * @brief Executes a function by name.
 *
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
void PluginInterface::ExecFunction(const std::string &name) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      foo->m_foo();
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes a function by name with arguments.
 *
 * This function searches for a PluginFunction with the specified name
 * and executes its associated function if found.
 *
 * @param name The name of the function to execute.
 */
void PluginInterface::ExecFunction(const std::string &name, std::shared_ptr<hArgs> args) {
  for (auto foo : this->m_functions) {
    if (foo->m_name == name) {
      foo->m_args = args;
      foo->m_args_foo(args);
      return;  // Exit after executing the function
    }
  }
}

/**
 * @brief Executes an input event by name.
 *
 * This function searches for a PluginInputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 * @param name The name of the input event to execute.
 * @param args The arguments to pass to the input event function.
 */
void PluginInterface::ExecInputEvent(const std::string &name, std::shared_ptr<hArgs> args) {
  for (auto event : this->m_input_events) {
    if (event->m_name == name) {
      event->m_foo(args);
      return;  // Exit after executing the input event
    }
  }
}

/**
 * @brief Executes an output event by name.
 *
 * This function searches for a PluginOutputEvent with the specified name
 * and executes its associated function with the provided arguments if found.
 *
 * @param name The name of the output event to execute.
 * @param args The arguments to pass to the output event function.
 */
void PluginInterface::ExecOutputEvent(const std::string &name, std::shared_ptr<hArgs> args) {
  for (auto event : this->m_output_events) {
    if (event->m_name == name) {
      event->m_foo(args);
      return;  // Exit after executing the output event
    }
  }
}

/**
 * @brief Logs an informational message.
 *
 * Logs an informational message associated with this plugin interface.
 *
 * @param message The message to log.
 */
void PluginInterface::LogInfo(const std::string &message) {
  VortexMaker::LogInfo(this->m_name, message);
}

/**
 * @brief Logs a warning message.
 *
 * Logs a warning message associated with this plugin interface.
 *
 * @param message The warning message to log.
 */
void PluginInterface::LogWarning(const std::string &message) {
  VortexMaker::LogWarn(this->m_name, message);
}

/**
 * @brief Logs an error message.
 *
 * Logs an error message associated with this plugin interface.
 *
 * @param message The error message to log.
 */
void PluginInterface::LogError(const std::string &message) {
  VortexMaker::LogError(this->m_name, message);
}

/**
 * @brief Logs a fatal error message.
 *
 * Logs a fatal error message associated with this plugin interface.
 *
 * @param message The fatal error message to log.
 */
void PluginInterface::LogFatal(const std::string &message) {
  VortexMaker::LogFatal(this->m_name, message);
}

/**
 * @brief Checks the dependencies of the plugin interface.
 *
 * This function checks if the dependencies specified for the plugin interface
 * are satisfied based on the current context.
 */
void PluginInterface::CheckDependencies() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  // Iterate through each dependency
  for (auto dependency : this->m_dependencies) {
    // By default, dependency is not satisfied
    dependency->satisfied = false;

    // Check dependency type
    if (dependency->type == "em") {
      // Dependency is an entity plugin
      for (auto ctx_em : ctx->IO.em) {
        // Check if context entity plugin matches the dependency name
        if (ctx_em->m_name == dependency->name) {
          // Check if the context entity plugin version is among the supported versions
          for (auto version : dependency->supported_versions) {
            if (ctx_em->m_version == version) {
              // Dependency is satisfied
              dependency->satisfied = true;
              break;  // No need to check further versions
            }
          }
        }
      }
    }
    // Futures deps types... (plugins, etc...)
  }
}

/**
 * @brief Starts the plugin interface.
 *
 * Checks dependencies and executes the plugin's main function.
 * If any dependency is not satisfied, logs a fatal error, lists the failed dependencies,
 * and sets the state to "failed".
 * If all dependencies are satisfied, executes the main function and sets the state to "running".
 */
void PluginInterface::Start() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  // Update dependencies
  this->CheckDependencies();

  // Get current vortex version
  std::string version = VORTEXLAUNCHER_VERSION;

  // Get major, a medium idendifiers of version
  std::size_t last_point = version.find('.', version.find('.') + 1);
  std::string major = version.substr(0, last_point);

  if (!major.empty() && major.back() == '.') {
    major.pop_back();
  }

  // Check if all dependencies are satisfied
  bool allDependenciesSatisfied = true;
  bool isVersionCompatible = false;

  // Vector of unsatisfied dependencies
  std::vector<std::pair<std::string, std::string>> unsatisfiedDependencies;

  for (auto dependency : this->m_dependencies) {
    if (!dependency->satisfied) {
      allDependenciesSatisfied = false;
      std::string versions;
      for (auto version : dependency->supported_versions) {
        versions += "[" + version + "]";
      }
      unsatisfiedDependencies.push_back({ dependency->name, versions });
    }
  }

  for (auto version : this->m_supported_versions) {
    if (version == major) {
      isVersionCompatible = true;
    }
  }

  // Vector of needed dependencies (to run)
  std::vector<std::pair<std::string, std::string>> needToRunDependencies;
  for (auto dep : this->m_dependencies) {
    for (auto em : ctx->IO.em) {
      if (em->m_name == dep->name) {
        std::string versions;
        for (auto version : dep->supported_versions) {
          versions += "[" + version + "]";
        }

        if (em->m_state != "running") {
          needToRunDependencies.push_back({ dep->name, versions });
        }
      }
    }
  }

  // If any dependency is not satisfied, log and set state to "failed"
  if (!allDependenciesSatisfied) {
    this->LogError("Dependencies not satisfied!");
    for (auto unsatisfiedDep : unsatisfiedDependencies) {
      this->LogError("Failed to retrieve: " + unsatisfiedDep.first + " with version(s) " + unsatisfiedDep.second);
    }
    this->m_state = "failed";
    return;
  }

  if (!isVersionCompatible) {
    this->LogError(
        "The Vortex version (" + version + ", try to find \"" + major + "\") is incompatible with \"" + this->m_name +
        "\" supported version(s).");

    this->m_state = "failed";
    return;
  }

  if (!needToRunDependencies.empty()) {
    this->LogError("Dependencies of this plugins are not running !");

    for (auto notRunningDeps : needToRunDependencies) {
      this->LogError("Please run : " + notRunningDeps.first + " with version(s) " + notRunningDeps.second);
    }
    this->m_state = "failed";
    return;
  }

  // Execute the plugin's main function and set state to "running"
  this->execute();
  this->m_state = "running";
}

/**
 * @brief Stops the plugin interface.
 *
 * Calls the destroy function to clean up resources and sets the state to "stopped".
 */
void PluginInterface::Stop() {
  // Get the current Vortex context
  VxContext *ctx = VortexMaker::GetCurrentContext();

  bool authorized = true;
  std::vector<std::pair<std::string, std::string>> deps;

  for (auto em : ctx->IO.em) {
    if (em->m_state == "running") {
      for (auto dependency : em->m_dependencies) {
        if (dependency->name == this->m_name) {
          for (auto version : dependency->supported_versions) {
            if (this->m_version == version) {
              authorized = false;

              std::string versions;
              for (auto version : dependency->supported_versions) {
                versions += "[" + version + "]";
              }
              deps.push_back({ dependency->name, versions });
            }
          }
        }
      }
    }
  }

  if (!authorized) {
    this->LogError("Cannot stop \"" + this->m_name + "\" ! ");
    for (auto dep : deps) {
      this->LogError("This plugin is used by \"" + dep.first + "\" with needed versions : " + dep.second + " ! ");
    }
  } else {
    this->destroy();
    this->m_state = "stopped";
  }
}

void PluginInterface::CheckVersion() {
}

std::vector<std::shared_ptr<PluginRenderInstance>> PluginInterface::GetPluginRenderInstances() {
  return this->m_render_instances;
};