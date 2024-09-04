#include "../configmanager/configmanager.h"

std::shared_ptr<ConfigurationManager> ConfigurationManager::m_cfgInstance{nullptr};
std::mutex ConfigurationManager::m_cfgMutex;
