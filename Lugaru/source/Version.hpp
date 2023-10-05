/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

/** Version number is either MAJOR.MINOR, or MAJOR.MINOR.PATCH if PATCH != 0 */
const unsigned int VERSION_MAJOR = 1;
const unsigned int VERSION_MINOR = 0;
const unsigned int VERSION_PATCH = 0;
const std::string VERSION_NUMBER = "1.0.0";

/** Set to "" for stable (tagged) builds, "-dev" for dev builds */
const std::string VERSION_SUFFIX = "-dev";

/** Final std::string built from the above constants, following the scheme
 *  defined in CMakeLists.txt, typically:
 *      MAJOR.MINOR[.PATCH][-dev] [(git HASH)] [[RELEASE]]
 *
 *  For example:
 *      "1.2-dev (git ab12c34)"
 *      "1.2-dev (git ab12c34) [OSS Lugaru official]"
 *      "1.3.1 [Mageia 1.3.1-2.mga6]"
 */
const std::string VERSION_STRING = "1.0.0-dev";

/** Build type (Release, Debug, RelWithDebInfo) to output to the terminal */
#ifndef NDEBUG
const std::string VERSION_BUILD_TYPE = "Debug";
#else
const std::string VERSION_BUILD_TYPE = "Release";
#endif