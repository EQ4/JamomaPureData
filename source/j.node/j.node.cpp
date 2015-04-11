/** @file
 *
 * @ingroup implementationPdExternals
 *
 * @brief j.node : edit priority, tag and description for a node that doesn't refer to any parameter, message or return
 *
 * @details
 *
 * @authors Théo de la Hogue, Antoine Villeret
 *
 * @copyright © 2014 by Théo de la Hogue @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */

#include "TTModularClassWrapperPd.h"

#define	dump_out 0

// Definitions
void		WrapTTNodeInfoClass(WrappedClassPtr c);
void		WrappedNodeInfoClass_new(TTPtr self, long argc, t_atom* argv);
void        WrappedNodeInfoClass_free(TTPtr self);


/** Provide assistance on input and output while patching.
 @param self	The node instance.
 @param b
 @param	msg		Determines if assistance is requested for an input or output.
 @param arg		Determines what input/output assistance is requested for.
 @param dst		Destination address that assistance string is copied to.
 */
void		node_assist(TTPtr self, TTPtr b, long msg, long arg, char *dst);

/** Subscribe the j.node into the directory.
 @param self	The node instance.
 @param relativeAddres where to subscribe the node relatively into the tree structure.
 @param argc    useless
 @param argv    useless
 */
void        node_subscribe(TTPtr self, t_symbol* relativeAddress, long argc, t_atom* argv);

extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2enode(void)
{
	ModularSpec *spec = new ModularSpec;
	spec->_wrap = &WrapTTNodeInfoClass;
	spec->_new = &WrappedNodeInfoClass_new;
	spec->_free = NULL;
	spec->_any = NULL;
	
    return (void)wrapTTModularClassAsPdClass(kTTSym_NodeInfo, "j.node", NULL, spec);
}


void WrapTTNodeInfoClass(WrappedClassPtr c)
{
    eclass_addmethod(c->pdClass, (method)node_assist,						"assist",				A_CANT, 0L);
}


void WrappedNodeInfoClass_new(TTPtr self, long argc, t_atom* argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	t_symbol *relativeAddress;
	long      attrstart = attr_args_offset(argc, argv);			// support normal arguments
	
	// check address argument
	relativeAddress = _sym_nothing;
	if (attrstart && argv)
		if (atom_gettype(argv) == A_SYM)
			relativeAddress = atom_getsym(argv);
	
	if (relativeAddress == _sym_nothing) {
		object_error((t_object*)x, "needs a name as first argument");
		return;
	}
    
    // check for reserved address
    if (relativeAddress == gensym("data/mute")      ||
        relativeAddress == gensym("data/bypass")    ||
        relativeAddress == gensym("data/freeze")    ||
        relativeAddress == gensym("data/preview")   ||
        relativeAddress == gensym("audio/mute")     ||
        relativeAddress == gensym("audio/bypass")   ||
        relativeAddress == gensym("audio/mix")      ||
        relativeAddress == gensym("audio/gain")     ||
        relativeAddress == gensym("model")          ||
        relativeAddress == gensym("preset")         ) {
        
        object_error((t_object*)x, "%s address is reserved by j.model", relativeAddress->s_name);
		return;
    }
    
    jamoma_node_info_create((t_object*)x, x->wrappedObject);
	
	if (argc && argv)
        attr_args_process(x, argc, argv);
	
	// The following must be deferred because we have to interrogate our box,
	// and our box is not yet valid until we have finished instantiating the object.
	// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
    node_subscribe(x,relativeAddress, argc, argv);
//	defer_low((t_object*)x, (method)node_subscribe, relativeAddress, argc, argv);
}

void node_subscribe(TTPtr self, t_symbol* relativeAddress, long argc, t_atom* argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTAddress   returnedAddress;
    TTNodePtr   returnedNode = NULL;
    TTNodePtr   returnedContextNode = NULL;

	// for relative address
	if (TTAddress(relativeAddress->s_name).getType() == kAddressRelative) {
        
        jamoma_subscriber_create((t_eobj*)x, x->wrappedObject, TTAddress(relativeAddress->s_name), x->subscriberObject, returnedAddress, &returnedNode, &returnedContextNode);
	}
	else
		object_error((t_object*)x, "can't register because %s is not a relative address", relativeAddress->s_name);
}

// Method for Assistance Messages
void node_assist(TTPtr self, TTPtr b, long msg, long arg, char *dst)
{
	if (msg==1) 						// Inlet
		strcpy(dst, "input");
	else {							// Outlets
		switch(arg) {
			case dump_out:
				strcpy(dst, "dumpout");
				break;
		}
 	}
}
