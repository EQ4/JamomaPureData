/** @file
 *
 * @ingroup implementationPdExternals
 *
 * @brief j.map - map input to output: y=f(x)
 *
 * @details
 *
 * @authors Théo de la Hogue, Trond Lossius, Antoine Villeret
 *
 * @copyright Copyright © 2010 by Théo de la Hogue @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#include "TTModularClassWrapperPd.h"

#define data_out 0
#define dump_out 1

// This is used to store extra data
typedef struct extra {
	TTValuePtr     arguments;      // keep creation arguments to reset the mapper to the initial state
} t_extra;
#define EXTRA ((t_extra*)x->extra)

// Definitions
void WrapTTMapperClass(WrappedClassPtr c);
void WrappedMapperClass_new(TTPtr self, long argc, t_atom *argv);
void WrappedMapperClass_free(TTPtr self);

void map_return_value(TTPtr self, t_symbol *msg, long argc, t_atom *argv);
void map_return_input_going_down(TTPtr self, t_symbol *msg, long argc, t_atom *argv);
void map_return_input_going_up(TTPtr self, t_symbol *msg, long argc, t_atom *argv);
void map_return_output_going_down(TTPtr self, t_symbol *msg, long argc, t_atom *argv);
void map_return_output_going_up(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void map_float(TTPtr self, t_float value);
void map_list(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void map_reset(TTPtr self);

void map_subscribe(TTPtr self);

extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2emap(void)
{
	ModularSpec *spec = new ModularSpec;
	spec->_wrap = &WrapTTMapperClass;
	spec->_new = &WrappedMapperClass_new;
	spec->_free = &WrappedMapperClass_free;
	spec->_any = NULL;
    spec->_notify = NULL;
	
    return (void)wrapTTModularClassAsPdClass(kTTSym_Mapper, "j.map", NULL, spec);
}

void WrapTTMapperClass(WrappedClassPtr c)
{	
    eclass_addmethod(c->pdClass, (method)map_return_value, "return_value", A_CANT, 0);
    eclass_addmethod(c->pdClass, (method)map_return_input_going_down, "return_input_going_down", A_CANT, 0);
    eclass_addmethod(c->pdClass, (method)map_return_input_going_up, "return_input_going_up", A_CANT, 0);
    eclass_addmethod(c->pdClass, (method)map_return_output_going_down, "return_output_going_down", A_CANT, 0);
    eclass_addmethod(c->pdClass, (method)map_return_output_going_up, "return_output_going_up", A_CANT, 0);
	
    eclass_addmethod(c->pdClass, (method)map_float, "float", A_FLOAT, 0L);
    eclass_addmethod(c->pdClass, (method)map_list, "list", A_GIMME, 0L);
    
    eclass_addmethod(c->pdClass, (method)map_reset, "reset", A_NULL, 0L);
    eclass_addmethod(c->pdClass, (method)map_subscribe, "loadbang", A_NULL, 0L);
}

void WrappedMapperClass_new(TTPtr self, long argc, t_atom *argv)
{
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
	t_symbol *relativeAddress;
	long attrstart = attr_args_offset(argc, argv); // support normal arguments
	
	// possible relativeAddress
	if (attrstart && argv)
		relativeAddress = atom_getsym(argv);
	else
		relativeAddress = _sym_nothing;
	
    if (relativeAddress) x->address = TTAddress(relativeAddress->s_name);
	
	jamoma_mapper_create((t_object*)x, x->wrappedObject);
	
	// Make two outlets
	x->outlets = (TTHandle)sysmem_newptr(sizeof(TTPtr) * 1);
    x->outlets[data_out] = outlet_new((t_object*)x, NULL); // anything outlet to output data
    x->dumpOut = outlet_new((t_object*)x, NULL);

	// handle attribute args
	attr_args_process(x, argc, argv);
    
    // Prepare extra data
	x->extra = (t_extra*)malloc(sizeof(t_extra));
    EXTRA->arguments = new TTValue();
    jamoma_ttvalue_from_Atom(*EXTRA->arguments, _sym_nothing, argc, argv);
	
	// The following must be deferred because we have to interrogate our box,
	// and our box is not yet valid until we have finished instantiating the object.
	// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
//    map_subscribe(x);
//	defer_low((t_object*)x, (method)map_subscribe, NULL, 0, 0);
}

void WrappedMapperClass_free(TTPtr self)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    delete EXTRA->arguments;
    
    x->wrappedObject.set("input", kTTAdrsEmpty);
    x->wrappedObject.set("output", kTTAdrsEmpty);
    
	free(EXTRA);
}

void map_subscribe(TTPtr self)
{
    /* TODO : fix exposeAttribute as parameter because for now
	 it crash due to a locked mutex during the notification
	 of observers when the attribute is updated from a Max message.
     
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
	TTValue v, n, args;
	TTDataPtr aData;
    
    // add 'cue' after the address
    if (x->address == kTTAdrsEmpty)
        x->address = TADRS("mapper");
    // if the subscription is successful
    if (!jamoma_subscriber_create((ObjectPtr)x, x->wrappedObject, x->address, &x->subscriberObject, returnedAddress, &returnedNode, &returnedContextNode)) {
        // expose attributes of TTMapper as TTData in the tree structure
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("input"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_string);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The input address to map"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("output"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_string);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The output address to map"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("inputMin"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_decimal);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The low bound input value"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("inputMax"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_decimal);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The high bound input value"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("outputMin"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_decimal);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The low bound output value"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("outputMax"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_decimal);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("The high bound output value"));
        x->subscriberObject->exposeAttribute(x->wrappedObject, TTSymbol("enable"), kTTSym_parameter, &aData);
        aData->setAttributeValue(kTTSym_type, kTTSym_boolean);
        aData->setAttributeValue(kTTSym_tags, kTTSym_generic);
        aData->setAttributeValue(kTTSym_description, TTSymbol("Turn on and off the mapping"));
    }
	 */
}

void map_return_value(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    outlet_anything((t_outlet*)x->outlets[data_out], NULL, argc, argv);
}

void map_return_input_going_down(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    outlet_anything((t_outlet*)x->dumpOut, gensym("input/going/down"), argc, argv);
}

void map_return_input_going_up(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    outlet_anything((t_outlet*)x->dumpOut, gensym("input/going/up"), argc, argv);
}

void map_return_output_going_down(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    outlet_anything((t_outlet*)x->dumpOut, gensym("output/going/down"), argc, argv);
}

void map_return_output_going_up(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    outlet_anything((t_outlet*)x->dumpOut, gensym("output/going/up"), argc, argv);
}

void map_bang(TTPtr self)
{
	map_list(self, _sym_bang, 0, NULL);
}

void map_float(TTPtr self, t_float value)
{
	t_atom a;
	
	atom_setfloat(&a, value);
	map_list(self, _sym_float, 1, &a);
}

void map_list(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
	TTValue inputValue, outputValue;
	
	jamoma_ttvalue_from_Atom(inputValue, msg, argc, argv);
	
	x->wrappedObject.send(kTTSym_Map, inputValue, outputValue);
	
	// we don't send the output value here because there is callback for this
}

void map_reset(TTPtr self)
{
    WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
    long   argc = 0;
    t_atom *argv = NULL;
    
    jamoma_ttvalue_to_Atom(*EXTRA->arguments, &argc, &argv);
    
    // handle attribute args to reset to creation state
	attr_args_process(x, argc, argv);
}
