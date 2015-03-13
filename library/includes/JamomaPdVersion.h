/** @file
 *
 * @ingroup implementationMaxLibrary
 *
 * @brief Jamoma For Max Shared Library
 *
 * @details Functions and resources used by Max objects.
 *
 * @authors Tim Place
 *
 * @copyright Copyright (c) 2009, Tim Place @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#ifndef __JAMOMA_PD_VERSION_H__
#define __JAMOMA_PD_VERSION_H__

#define GIT_SHA_SHORT "-128-NOTFOUND"
#define GIT_VERSION_MAJ "-128-NOTFOUND"
#define GIT_VERSION_MIN ""
#define GIT_VERSION_SUB ""
#define GIT_VERSION_MOD ""
#define GIT_VERSION GIT_VERSION_MAJ.GIT_VERSION_MIN.GIT_VERSION_SUB GIT_VERSION_MOD



#define JAMOMA_PD_VERSION "GIT_VERSION"
#define JAMOMA_PD_REV "GIT_SHA_SHORT"

#endif //__JAMOMA_PD_VERSION_H__
