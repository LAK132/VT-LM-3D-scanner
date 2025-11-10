#ifndef SCANNER_MAIN_HPP
#define SCANNER_MAIN_HPP

#include <lak/architecture.hpp>

#include "scanner_git.hpp"

#define APP_VERSION GIT_TAG "-" GIT_HASH
#define APP_NAME    "3D Scanner " STRINGIFY(LAK_ARCH) " " APP_VERSION

#endif
