/******************************************************************************
/*!
\file  Debug.cs
\Proj name  Shroomy Doomy
\author  Benjamin
\date    Feb 03, 2024
\brief  This file defines the Debug class used for logging informational, warning, 
       and error messages in debug mode. It also includes a method for marking unused
       parameters to avoid compiler warnings.

All content © 2024 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
using System;

namespace FruitLoops
{
    /**
     * @brief Provides static methods for logging debug messages.
     *
     * This class contains methods to log informational, warning, and error messages 
     * in debug mode using conditional compilation. It also provides a method to mark
     * unused parameters.
     */
    public static class Debug
	{
        /**
         * @brief Logs an informational message in debug mode.
         *
         * @param message The message to log.
         * @param args Optional arguments to format the message.
         */
        [System.Diagnostics.Conditional("DEBUG")]
		public static void LogInfo(string message, params object[] args)
		{
			Log("info", message, args);
		}

        /**
         * @brief Logs a warning message in debug mode.
         *
         * @param message The message to log.
         * @param args Optional arguments to format the message.
         */
        [System.Diagnostics.Conditional("DEBUG")]
		public static void LogWarning(string message, params object[] args)
		{
			Log("warning", message, args);
		}

        /**
         * @brief Logs an error message in debug mode.
         *
         * @param message The message to log.
         * @param args Optional arguments to format the message.
         */
        [System.Diagnostics.Conditional("DEBUG")]
		public static void LogError(string message, params object[] args)
		{
			Log("error", message, args);
		}

        /**
         * @brief Marks unused parameters to avoid compiler warnings.
         *
         * This method does nothing and is used solely to suppress warnings about 
         * unused parameters.
         *
         * @tparam T The type of the unused parameter.
         * @param unused The unused parameter.
         */
        [System.Diagnostics.Conditional("DEBUG")]
		public static void MarkUnused<T>(T unused)
		{
			// Intentionally left blank to suppress compiler warnings.
		}

        /**
         * @brief Internal method to format and print log messages.
         *
         * @param level The log level (INFO, WARNING, ERROR).
         * @param message The message to log.
         * @param args Optional arguments to format the message.
         */
        private static void Log(string level, string message, object[] args)
		{
			string formattedMessage = string.Format(message, args);
			Console.WriteLine($"[{level}] {formattedMessage}");
		}
	}
}
