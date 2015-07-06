/** @file
 *
 * @ingroup implementationPdExternals
 *
 * @brief j.namespace - deal with the Jamoma Namespace
 *
 * @details
 *
 * @authors Théo de la Hogue, Trond Lossius, Antoine Villeret
 *
 * @copyright Copyright © 2010 Théo de la Hogue @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#include "TTModularClassWrapperPd.h"

#define data_out 0
#define size_out 1
#define dump_out 2

// Definitions
void		WrapTTExplorerClass(WrappedClassPtr c);
void		WrappedExplorerClass_new(TTPtr self, long argc, t_atom *argv);
void		WrappedExplorerClass_free(TTPtr self);

void		nmspc_assist(TTPtr self, void *b, long m, long a, char *s);

void		nmspc_return_value(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void		nmspc_return_selection(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void		nmspc_bang(TTPtr self);
void		nmspc_symbol(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void		nmspc_subscribe(TTPtr self);

void		nmspc_return_model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

t_max_err	nmspc_get_format(TTPtr self, TTPtr attr, long *ac, t_atom **av);
t_max_err	nmspc_set_format(TTPtr self, TTPtr attr, long ac, t_atom *av);

t_max_err	nmspc_get_relative(TTPtr self, TTPtr attr, long *ac, t_atom **av);
t_max_err	nmspc_set_relative(TTPtr self, TTPtr attr, long ac, t_atom *av);

/*
void		nmspc_add_max_namespace(TTPtr self);
long		nmspc_myobject_iterator(TTPtr self, (t_object*) b);
*/

//t_symbol *nmspc_filter_underscore_instance(t_symbol *a);


extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2enamespace(void)
{
	ModularSpec *spec = new ModularSpec;
	spec->_wrap = &WrapTTExplorerClass;
	spec->_new = &WrappedExplorerClass_new;
	spec->_free = &WrappedExplorerClass_free;
	spec->_any = NULL;
	
    return (void)wrapTTModularClassAsPdClass(kTTSym_Explorer, "j.namespace", NULL, spec);
}

void WrapTTExplorerClass(WrappedClassPtr c)
{
	// add methods
    eclass_addmethod(c->pdClass, (method)nmspc_assist,				"assist",					A_CANT, 0);
	
    eclass_addmethod(c->pdClass, (method)nmspc_return_value,		"return_value",				A_CANT, 0);
	
    eclass_addmethod(c->pdClass, (method)nmspc_return_model_address,"return_model_address",		A_CANT, 0);

    eclass_addmethod(c->pdClass, (method)nmspc_return_selection,	"return_selection",			A_CANT, 0);
	
    eclass_addmethod(c->pdClass, (method)nmspc_bang,				"bang",						A_NULL, 0);
	
    eclass_addmethod(c->pdClass, (method)nmspc_symbol,				"anything",					A_GIMME, 0);
	
    eclass_addmethod(c->pdClass, (method)nmspc_bang,				"explore",					A_NULL, 0); // overwrite explore message to use the bang method

    eclass_addmethod(c->pdClass, (method)NULL,				"loadbang",					A_NULL, 0); // overwrite explore message to use the bang method

    //eclass_addmethod(c->pdClass, (method)nmspc_add_max_namespace,	"add_max_namespace",		0);
	
    // TODO restore those attributes
    // CLASS_ATTR_ENUM(c->pdClass,		"output",	0,		"descendants children brothers attributes");
	
    // CLASS_ATTR_SYM(c->pdClass,			"format",	0,		WrappedModularInstance,	msg);	// use msg member to store format
    CLASS_ATTR_ACCESSORS(c->pdClass,	"format",	nmspc_get_format,	nmspc_set_format);
    // CLASS_ATTR_ENUM(c->pdClass,		"format",	0,		"none umenu umenu_prefix jit.cellblock coll");

    CLASS_ATTR_LONG(c->pdClass,		"relative",	0,		WrappedModularInstance,	index);	// use index member to store relative
    CLASS_ATTR_ACCESSORS(c->pdClass,	"relative",	nmspc_get_relative,	nmspc_set_relative);
    CLASS_ATTR_STYLE(c->pdClass,		"relative",	0,		"onoff");
}

void WrappedExplorerClass_new(TTPtr self, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
 	long						attrstart = attr_args_offset(argc, argv);			// support normal arguments
	t_symbol*					name;
	TTAddress                   address;
	TTValue						v, args;
	
	// create the explorer
	jamoma_explorer_create((t_object*)x, x->wrappedObject);
	
	// read first argument to know if the explorer handles a namespace
	if (attrstart && argv) {
		
		if (atom_gettype(argv) == A_SYM) {
			
			name = atom_getsym(argv);
			x->wrappedObject.set(kTTSym_namespace, TTSymbol(name->s_name));
		}
		else
			pd_error((t_object*)x, "argument not expected");
	}
	
	// Make two outlets
	x->outlets = (TTHandle)sysmem_newptr(sizeof(TTPtr) * 2);
    x->outlets[size_out] = outlet_new((t_object*)x, NULL);
    x->outlets[data_out] = outlet_new((t_object*)x, NULL);
	
	x->msg = _sym_none;
	
	// handle attribute args
	attr_args_process(x, argc, argv);
}

void WrappedExplorerClass_free(TTPtr self)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    x->wrappedObject.set(kTTSym_namespace, kTTSymEmpty);
}

void nmspc_assist(TTPtr self, void *b, long msg, long arg, char *dst)
{
	if (msg==1) 						// Inlet
		strcpy(dst, "input");
	else {							// Outlets
		switch(arg) {
			case data_out:
				strcpy(dst, "result of exploration");
				break;
			case size_out:
				strcpy(dst, "size of the result (after the result)");
				break;
			case dump_out:
				strcpy(dst, "dumpout");
				break;
		}
 	}
}

void nmspc_subscribe(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue						v;
	TTAddress                   contextAddress = kTTAdrsEmpty;
	TTAddress                   absoluteAddress, returnedAddress;
    TTNodePtr                   returnedNode;
    TTNodePtr                   returnedContextNode;
	TTObject                    anObject, empty;
	
    jamoma_patcher_get_info(((t_eobj*)x)->o_canvas, &x->patcherPtr, x->patcherContext, x->patcherClass, x->patcherName);
	
    if (!jamoma_subscriber_create((t_eobj*)x, empty, kTTAdrsEmpty, x->subscriberObject, returnedAddress, &returnedNode, &returnedContextNode)) {
        
		// get the context address to make
		// a receiver on the contextAddress/model:address attribute
		x->subscriberObject.get("contextAddress", v);
		contextAddress = v[0];
	}
	
	// bind on the model:address attribute
	if (contextAddress != kTTAdrsEmpty) {
		
		absoluteAddress = contextAddress.appendAddress(x->address);
		x->wrappedObject.set(kTTSym_address, x->address);
		
		makeInternals_receiver(x, contextAddress, TTSymbol("/model:address"), gensym("return_model_address"), anObject, YES); // YES : we want to deferlow this method
	}
	
	// while the context node is not registered : try to binds again :(
	// (to -- this is not a good way todo. For binding we should make a subscription 
	// to a notification mechanism and each time an TTObjet subscribes to the namespace
	// using jamoma_subscriber_create we notify all the externals which have used 
	// jamoma_subscriber_create with NULL object to bind)
	else {
		
		// release the subscriber
		x->subscriberObject = TTObject();
		
		// The following must be deferred because we have to interrogate our box,
		// and our box is not yet valid until we have finished instantiating the object.
		// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
        nmspc_subscribe(x);
//		defer_low((t_object*)x, (method)nmspc_subscribe, NULL, 0, 0);
	}
}

void nmspc_return_model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTAddress absoluteAddress;
    
    // the wrapped object is maybe destroyed
    if (!x->wrappedObject.valid())
        return;
	
	if (argc && argv) {
		
		// set address attribute of the wrapped Receiver object
		absoluteAddress = TTAddress(atom_getsym(argv)->s_name).appendAddress(x->address);
		x->wrappedObject.set(kTTSym_address, absoluteAddress);
		x->wrappedObject.send("Explore");
	}
}

void nmspc_return_value(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue		v;
	TTSymbol	output;
	TTAddress   address;
	t_symbol	*s;
	TTInt32     i;
	t_atom		a[1], c[2], j[3];
    
    // the wrapped object is maybe destroyed
    if (!x->wrappedObject.valid())
        return;
	
	// Ask Explorer object
	x->wrappedObject.get("output", v);
	output = v[0];
	
	x->wrappedObject.get(kTTSym_address, v);
	address = v[0];
	
	// UMENU OR UMENU_PREFIX FORMAT
	if (x->msg == gensym("umenu") || x->msg == gensym("umenu_prefix")) {
		
		// clear menu
        outlet_anything((t_outlet*)x->outlets[data_out], _sym_clear, 0, NULL);

		// prepare umenu prefix to be concatenated
		if (x->msg == gensym("umenu_prefix")) {
			
			atom_setlong(a, 0);
            outlet_anything((t_outlet*)x->outlets[data_out], gensym("prefix_mode"), 1, a);
			
			// prepare umenu prefix 
			if (address.getName() == S_SEPARATOR) {
                
                TTString prefix = address.c_str();
				
				if (output == kTTSym_attributes)
					prefix += ":";
                
                atom_setsym(a, gensym((char*)prefix.data()));
			}
			else {
				TTString prefix = address.c_str();
				
				if (output == kTTSym_children)
					prefix += "/";
				else if (output == kTTSym_brothers)
					prefix += ".";
				else if (output == kTTSym_attributes)
					prefix += ":";
				else
					prefix += "/";
				
				atom_setsym(a, gensym((char*)prefix.data()));
			}
			
            outlet_anything((t_outlet*)x->outlets[data_out], gensym("prefix"), 1, a);
		}
		
		// fill umenu
		// output argv
		for (i = 0; i < argc; i++) {
			s = atom_getsym(argv+i);
			
			if (output == kTTSym_descendants && address.getName() == S_SEPARATOR)
				if (s->s_name[0] == C_SEPARATOR)
					s = gensym(s->s_name+1); // remove the / in this case
			
			if (output == kTTSym_attributes)
                s = jamoma_TTName_To_PdName(TTSymbol(s->s_name));
			
			if (output == kTTSym_brothers && s == _sym_bang)
				s = gensym("0");
			
			if (s) {
				atom_setsym(a, s);
                outlet_anything((t_outlet*)x->outlets[data_out], _sym_append, 1, a);
			}
		}
	}
	
	// JIT CELLBLOCK FORMAT
	else if (x->msg == gensym("jit.cellblock")) {
		
		// clear jit.cellblock
		atom_setsym(a, gensym("all"));
        outlet_anything((t_outlet*)x->outlets[data_out], _sym_clear, 1, a);
		
		// prepare jit.cellblock structure
		atom_setlong(a, argc);
        outlet_anything((t_outlet*)x->outlets[data_out], gensym("rows"), 1, a);
		
		// fill jit.cellblock
		// output argv
		for (i = 0; i < argc; i++) {
			s = atom_getsym(argv+i);
			
			if (output == kTTSym_attributes)
                s = jamoma_TTName_To_PdName(TTSymbol(s->s_name));
			
			if (output == kTTSym_brothers && s == _sym_bang)
				s = gensym("0");
			
			if (s) {
				atom_setlong(j, 0);
				atom_setlong(j+1, i);
				atom_setsym(j+2, s);
                outlet_anything((t_outlet*)x->outlets[data_out], _sym_set, 3, j);
			}
		}
	}
	
	// COLL FORMAT
	else if (x->msg == gensym("coll")) {
		
		// clear coll
        outlet_anything((t_outlet*)x->outlets[data_out], _sym_clear, 0, NULL);
		
		// fill coll
		// output argv
		for (i=0; i<argc; i++) {
			s = atom_getsym(argv+i);
			
			if (output == kTTSym_attributes)
                s = jamoma_TTName_To_PdName(TTSymbol(s->s_name));
			
			if (output == kTTSym_brothers && s == _sym_bang)
				s = gensym("0");
			
			if (s) {
				atom_setsym(c, s);
				atom_setlong(c+1, 0); // any data to avoid storage error
                outlet_anything((t_outlet*)x->outlets[data_out], _sym_store, 2, c);
			}
		}
	}
	
	// NO FORMAT
	else if (x->msg == gensym("none") || x->msg == _sym_nothing) {
		if (argc)
            outlet_anything((t_outlet*)x->outlets[data_out], _sym_nothing, argc, argv);
		else if (msg != _sym_nothing)
            outlet_anything((t_outlet*)x->outlets[data_out], msg, argc, argv);
	}
    
	// output the size of the result after the result
    // to perform auto selection in case the result contains only 1 item
	atom_setlong(a, argc);
    outlet_anything((t_outlet*)x->outlets[size_out], _sym_int, 1, a);
}

void nmspc_return_selection(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue		v;
	TTSymbol    output;
	TTUInt32	i, state;
	t_atom		u[2], j[6];
    
    // the wrapped object is maybe destroyed
    if (!x->wrappedObject.valid())
        return;
	
	// Ask Explorer object
	x->wrappedObject.get("output", v);
	output = v[0];
	
	// UMENU OR UMENU_PREFIX FORMAT
	if (x->msg == gensym("umenu") || x->msg == gensym("umenu_prefix")) {
		
		// clear check item
        outlet_anything((t_outlet*)x->outlets[data_out], gensym("clearchecks"), 0, NULL);
		
		// update check item
		for (i = 0; i < (TTUInt32) argc; i++) {
			state = atom_getlong(argv+i);
			atom_setlong(u, i);
			atom_setlong(u+1, state);
            outlet_anything((t_outlet*)x->outlets[data_out], gensym("checkitem"), 2, u);
		}
	}
	
	// JIT CELLBLOCK FORMAT
	else if (x->msg == gensym("jit.cellblock")) {
		
		// update background color
		for (i = 0; i < (TTUInt32) argc; i++) {
			
			atom_setlong(j, 0);
			atom_setlong(j+1, i);
			atom_setsym(j+2, gensym("brgb"));
			
			state = atom_getlong(argv+i);
			
			if (state) {
				atom_setlong(j+3, 158);
				atom_setlong(j+4, 0);
				atom_setlong(j+5, 92);
			}
			else {
				atom_setlong(j+3, 0);
				atom_setlong(j+4, 0);
				atom_setlong(j+5, 0);
			}
			
            outlet_anything((t_outlet*)x->outlets[data_out], gensym("cell"), 6, j);
		}
	}
	
	// COLL FORMAT
	else if (x->msg == gensym("coll")) {
		;
	}
	
	// NO FORMAT
	else if (x->msg == gensym("none") || x->msg == _sym_nothing)
        outlet_anything((t_outlet*)x->outlets[data_out], gensym("selection"), argc, argv);
}

void nmspc_bang(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	// UMENU OR UMENU_PREFIX FORMAT : clear umenu
	if (x->msg == gensym("umenu") || x->msg == gensym("umenu_prefix"))
        outlet_anything((t_outlet*)x->outlets[data_out], _sym_clear, 0, NULL);

	x->wrappedObject.send("Explore");
}

void nmspc_symbol(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue		v;
	TTAddress   adrs, absoluteAddress;
	
	// for absolute address
    adrs = TTAddress(msg->s_name);
	if (adrs.getType() == kAddressAbsolute) {
		v.append(adrs);
	}
	else {
		
		// if the relative attribute is on
		if (x->index) {
			absoluteAddress = x->patcherAddress.appendAddress(adrs);
			v.append(absoluteAddress);
		}
		else {
			pd_error((t_object*)x, "set relative attribute on before to send relative address");
			return;
		}
	}
	
	x->wrappedObject.set(kTTSym_address, v);
	
	// UMENU OR UMENU_PREFIX FORMAT : clear umenu
	if (x->msg == gensym("umenu") || x->msg == gensym("umenu_prefix"))
        outlet_anything((t_outlet*)x->outlets[data_out], _sym_clear, 0, NULL);
	
	x->wrappedObject.send("Explore");
}

/*
void nmspc_add_max_namespace(t_nmspc *x)
{
	t_object *patcher;
	t_max_err err;
	long result = 0;

	err = object_obex_lookup(x, gensym("#P"), &patcher);

	object_method(patcher, gensym("iterate"), nmspc_myobject_iterator, (void *)x, PI_WANTBOX | PI_DEEP, &result);
}

long nmspc_myobject_iterator(t_nmspc *x, t_object *b)
{
	TTNodePtr newTTNode;
	bool newInstanceCreated;
	char temp[256];
    t_symbol *varname = object_attr_getsym(b, gensym("varname"));
	uint i;

	// Make sure we are dealing with valid OSC input by looking for a leading slash
	if (varname) {
		if (varname->s_name[0] == C_SEPARATOR) {
			newInstanceCreated = false;
			
			// put all scripting name in a /max node
			snprintf(temp,256,"/max%s", varname->s_name);
			
			// search for [ and ] cause this is how max declare instance. 
			for (i=0; i<strlen(temp); i++) {
				if (temp[i] == '[')
					temp[i] = '.';
				else if (temp[i] == ']')
					temp[i] = 0;
			}

			jamoma_directory_register(gensym(temp), gensym("maxobject"), (t_object *)b, &newTTNode, &newInstanceCreated);

			// add varname and maxclass as properties of the node
			//TODO : max_node_attribute_add(newTTNode,gensym("varname"), b);
			//TODO : max_node_attribute_add(newTTNode,gensym("maxclass"), b);

			//if (newInstanceCreated)
			//	object_warn((t_object *)x,"%s : this scripting name is already registered in the tree", varname->s_name);
		}
	}

    return 0;
}
 */

t_max_err nmspc_get_format(TTPtr self, TTPtr attr, long *ac, t_atom **av)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if ((*ac)&&(*av)) {
		//memory passed in, use it
	} else {
		//otherwise allocate memory
		*ac = 1;
		if (!(*av = (t_atom*)getbytes(sizeof(t_atom)*(*ac)))) {
			*ac = 0;
            return -4; // MAX_ERR_OUT_OF_MEM;
		}
	}
	
	atom_setsym(*av, x->msg);
	
    return 0; // MAX_ERR_NONE;
}

t_max_err nmspc_set_format(TTPtr self, TTPtr attr, long ac, t_atom *av) 
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if (ac&&av) {
		x->msg = atom_getsym(av);
	} else {
		// no args, set to none
		x->msg = _sym_none;
	}
    return 0; // MAX_ERR_NONE;
}

t_max_err nmspc_get_relative(TTPtr self, TTPtr attr, long *ac, t_atom **av)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if ((*ac)&&(*av)) {
		//memory passed in, use it
	} else {
		//otherwise allocate memory
		*ac = 1;
		if (!(*av = (t_atom*)getbytes(sizeof(t_atom)*(*ac)))) {
			*ac = 0;
            return -4; // MAX_ERR_OUT_OF_MEM;
		}
	}
	
	atom_setlong(*av, x->index);
	
    return 0; // MAX_ERR_NONE;
}

t_max_err nmspc_set_relative(TTPtr self, TTPtr attr, long ac, t_atom *av)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if (ac&&av) {
		x->index = atom_getlong(av);
		
		if (x->index) {
			// The following must be deferred because we have to interrogate our box,
			// and our box is not yet valid until we have finished instantiating the object.
			// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
            nmspc_subscribe(x);
//			defer_low((t_object*)x, (method)nmspc_subscribe, NULL, 0, 0);
		}
		
	} else {
		// no args
		x->index = 0;
	}
    return 0; // MAX_ERR_NONE;
}
