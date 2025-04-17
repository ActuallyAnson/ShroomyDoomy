/******************************************************************************/
/*!
\file    DebugMacros.h
\project Shroomy Doomy
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains macro definitions for debugging purposes,
         including logging messages at various levels (info, error, warning)
         and handling unused parameters. These macros are conditionally
         compiled based on whether the DEBUG flag is defined, enabling or
         disabling debugging features during runtime.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once

#define UNUSED_PARAM(x) (void)(x)


#define FL_DEBUG_INFO(message, ...) spdlog::info(message, ##__VA_ARGS__)
#define FL_DEBUG_ERROR(message, ...) spdlog::error(message, ##__VA_ARGS__)
#define FL_DEBUG_WARN(message, ...) spdlog::warn(message, ##__VA_ARGS__)
