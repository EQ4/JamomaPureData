/** @file
 *
 * @ingroup implementationPdLibrary
 *
 * @brief The global jamoma object.
 *
 * @details Functions and resources used by Max objects.
 *
 * @authors Tim Place, Trond Lossius, Antoine Villeret
 *
 * @copyright Copyright © 2007, Tim Place @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#include "JamomaObject.h"

// statics and globals
static t_eclass		*s_jamoma_class;

/************************************************************************************/

void jamoma_object_initclass(void)
{
    t_eclass *c;
	
	// Define our class
    c = eclass_new(	"jamoma",
                    (method)jamoma_object_new,
                    (method)jamoma_object_free,
                    sizeof(t_jamoma_object),
                    0L, A_NULL, 0);

	// Finalize our class
	s_jamoma_class = c;
}


t_object *jamoma_object_new()
{
    t_jamoma_object		*obj = (t_jamoma_object *)eobj_new(s_jamoma_class);
	if (obj) {
		;
	}
	return (t_object*)obj;
}


void jamoma_object_free(t_jamoma_object *obj)
{
	;
}

