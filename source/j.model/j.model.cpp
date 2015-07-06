/** @file
 *
 * @ingroup implementationPdExternals
 *
 * @brief j.model / j.view - The main control center of Jamoma model and view patcher
 *
 * @details
 *
 * @authors Tim Place, Théo de la Hogue, Trond Lossius, Antoine Villeret
 *
 * @copyright Copyright © 2010 by Tim Place and Théo de la Hogue @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#include "j.model.h"

#if defined (JCOM_VIEW)
extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2eview(void)
#else
extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2emodel(void)
#endif
{
	ModularSpec *spec = new ModularSpec;
	spec->_wrap = &WrapTTContainerClass;
	spec->_new = &WrappedContainerClass_new;
	spec->_free = &WrappedContainerClass_free;
	spec->_any = &WrappedContainerClass_anything;
	spec->_notify = NULL;
    
    TTModelInfo::registerClass();
    
#ifndef JCOM_VIEW
	return (void) wrapTTModularClassAsPdClass(kTTSym_Container, "j.model", NULL, spec);
#else
	return (void) wrapTTModularClassAsPdClass(kTTSym_Container, "j.view", NULL, spec);
#endif
}

void WrapTTContainerClass(WrappedClassPtr c)
{
	eclass_addmethod(c->pdClass, (method)model_assist,                      "assist",				A_CANT, 0L);

	eclass_addmethod(c->pdClass, (method)model_share_patcher_info,          "share_patcher_info",	A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_share_patcher_node,          "share_patcher_node",	A_CANT, 0);
	
	eclass_addmethod(c->pdClass, (method)model_return_address,              "return_address",		A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_return_value,                "return_value",			A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_return_upper_view_model_address,"return_upper_view_model_address", A_CANT, 0);
	
	eclass_addmethod(c->pdClass, (method)model_address,                     "model_address",		A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_reference_write,             "reference_write",      A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_preset_return_names,         "return_names",			A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_filechanged,          "filechanged",			A_CANT, 0);
	
	eclass_addmethod(c->pdClass, (method)model_preset_read,                 "preset_read",			A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_write,                "preset_write",			A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_preset_read_again,           "preset_read_again",	A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_write_again,          "preset_write_again",	A_CANT, 0);
	
	eclass_addmethod(c->pdClass, (method)model_preset_edit,                 "dblclick",				A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_edclose,              "edclose",				A_CANT, 0);
#ifndef JCOM_VIEW
	eclass_addmethod(c->pdClass, (method)model_preset_read,                 "preset:read",			A_GIMME, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_write,                "preset:write",			A_GIMME, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_edit,                 "preset:edit",			A_GIMME, 0);

	eclass_addmethod(c->pdClass, (method)model_preset_read_again,           "preset:read/again",	A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_preset_write_again,          "preset:write/again",	A_CANT, 0);
#endif
	eclass_addmethod(c->pdClass, (method)model_signal_amenities,            "input_created",		A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_signal_amenities,            "output_created",		A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_signal_return_data_active,   "return_data_active",	A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_signal_return_data_bypass,   "return_data_bypass",	A_CANT, 0);
    
	eclass_addmethod(c->pdClass, (method)model_signal_return_audio_mute,    "return_audio_mute",	A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_signal_return_audio_bypass,  "return_audio_bypass",  A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_signal_return_audio_mix,     "return_audio_mix",     A_CANT, 0);
	eclass_addmethod(c->pdClass, (method)model_signal_return_audio_gain,    "return_audio_gain",	A_CANT, 0);

	eclass_addmethod(c->pdClass, (method)model_subscribe,    "loadbang",				A_NULL, 0);
    
	CLASS_ATTR_ATOM(c->pdClass,        "amenities",	0,		WrappedModularInstance,	extra);
	CLASS_ATTR_DEFAULT(c->pdClass,		"amenities",	0,		"all");
	CLASS_ATTR_ACCESSORS(c->pdClass,	"amenities",			model_get_amenities, model_set_amenities);
	CLASS_ATTR_STYLE(c->pdClass,		"amenities",	0,		"text");
    
	CLASS_ATTR_ATOM(c->pdClass,        "presets",	0,          WrappedModularInstance,	extra);
	CLASS_ATTR_DEFAULT(c->pdClass,		"presets",	0,          "none");
	CLASS_ATTR_ACCESSORS(c->pdClass,	"presets",              model_preset_get_presets, model_preset_set_presets);
	CLASS_ATTR_STYLE(c->pdClass,		"presets",	0,          "text");
}

void WrappedContainerClass_new(TTPtr self, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue     none;
	t_object    *aPatcher;
    
    // is there already a j.model or j.view in the patcher ?
	jamoma_patcher_get_model_or_view(x->obj.o_canvas, &aPatcher);
    
    if (aPatcher) {
        pd_error((t_object*)x, "can't have two models or views in the same patcher");
        return;
    }
		
	// create a container
	jamoma_container_create((t_object*)x, x->wrappedObject);
	
#ifndef JCOM_VIEW
	x->patcherContext = kTTSym_model;
	x->wrappedObject.set(kTTSym_service, kTTSym_model);
#else
	x->patcherContext = kTTSym_view;
	x->wrappedObject.set(kTTSym_service, kTTSym_view);
#endif
		
	// Make two outlets
	x->outlets = (TTHandle)sysmem_newptr(sizeof(TTPtr) * 1);
	x->outlets[data_out] = outlet_new((t_object*)x, NULL);						// anything outlet to output data

	// Prepare extra data
	x->extra = (t_extra*)malloc(sizeof(t_extra));
    EXTRA->modelInfo = new TTObject();
    EXTRA->containerAddress = kTTAdrsEmpty;
    EXTRA->argAddress = kTTAdrsEmpty;
    EXTRA->text = NULL;
	EXTRA->textEditor = NULL;
    EXTRA->presetManager = new TTObject();
    EXTRA->attr_presets = kTTSym_none;
	EXTRA->filewatcher = NULL;
    EXTRA->toEdit = new TTObject();
	*EXTRA->toEdit = x->wrappedObject;
	EXTRA->presetName = kTTSymEmpty;
    EXTRA->attr_amenities = new TTHash();
    EXTRA->all_amenities = NO;
    EXTRA->no_amenities = NO;
    
    EXTRA->attr_amenities->append(TTSymbol("all"), none);
    
    // handle attribute args
	attr_args_process(x, argc, argv);
	
	// The following must be deferred because we have to interrogate our box,
	// and our box is not yet valid until we have finished instantiating the object.
	// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
	// defer_low((t_object*)x, (method)model_subscribe, NULL, 0, 0);
    // model_subscribe((t_object*)x);
}

void WrappedContainerClass_free(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTAddress    modelAddress, presetAddress;
    TTValue      v, none;
    
    if (!EXTRA)
        return;
    
	if (EXTRA->modelInfo && EXTRA->modelInfo->valid()) {
        
        modelAddress = EXTRA->containerAddress.appendAddress(TTAddress("model"));
        
        // remove the model node
        JamomaApplication.send("ObjectUnregister", modelAddress, none);
    }
    delete EXTRA->modelInfo;
    
	if (EXTRA->presetManager && EXTRA->presetManager->valid()) {
        
        presetAddress = EXTRA->containerAddress.appendAddress(TTAddress("preset"));
        
        // remove the preset node
         JamomaApplication.send("ObjectUnregister", presetAddress, none);
    }
    delete EXTRA->presetManager;
    
    // delete filewatcher
	/*
	if (EXTRA->filewatcher) {
		filewatcher_stop(EXTRA->filewatcher);
		object_free(EXTRA->filewatcher);
	}
	*/
    
    delete EXTRA->toEdit;
    
    // unbind the container
    x->wrappedObject.set(kTTSym_address, kTTSymEmpty);

	free(EXTRA);
}

void model_subscribe(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue						v, args, none;
    TTNodePtr                   returnedNode = NULL;
    TTNodePtr                   returnedContextNode = NULL;
	TTObject                    aReceiver;
	TTObject                    aTextHandler;
	TTBoolean					isSubModel;
	TTAddress                   returnedAddress, adrs;
	TTSymbol                    description;
	long                        ac;
	t_atom*						av;
    t_atom                      a;
	t_canvas*					aPatcher = ((t_eobj*)x)->o_canvas;
	TTAddress    modelAddress, presetAddress;

	// unsubscribe before subscribing
	if (EXTRA->modelInfo && EXTRA->modelInfo->valid()) {

		modelAddress = EXTRA->containerAddress.appendAddress(TTAddress("model"));

		// remove the model node
		JamomaApplication.send("ObjectUnregister", modelAddress, none);
	}

	if (EXTRA->presetManager && EXTRA->presetManager->valid()) {

		presetAddress = EXTRA->containerAddress.appendAddress(TTAddress("preset"));

		// remove the preset node
		 JamomaApplication.send("ObjectUnregister", presetAddress, none);
	}

	// if the subscription is successful
	if (!jamoma_subscriber_create((t_eobj*)x, x->wrappedObject, kTTAdrsEmpty, x->subscriberObject, returnedAddress, &returnedNode, &returnedContextNode)) {
		
		// get all info relative to our patcher
		jamoma_patcher_get_info(aPatcher, &x->patcherPtr, x->patcherContext, x->patcherClass, x->patcherName);
		/*
		post("j.model info :");
		post("context : %s",x->patcherContext.c_str());
		post("classe : %s",x->patcherClass.c_str());
		post("name : %s",x->patcherName.c_str());
		*/
		// set the address attribute of the Container 
		x->wrappedObject.set(kTTSym_address, returnedAddress);
        
        // keep the address for model_free (because the wrappedObject will be freed before)
        EXTRA->containerAddress = returnedAddress;
        
        // set annotation attribute of the patcher using description attribute
		/* AV TODO for PD ?
		x->wrappedObject.get(kTTSym_description, v);
        description = v[0];
        atom_setsym(&a, gensym(description.c_str()));
        object_attr_setvalueof(jpatcher_get_box(x->patcherPtr), _sym_annotation , 1, &a);
		*/

		// if the j.model|j.view is well subscribed
		if (aPatcher == x->patcherPtr && x->patcherContext != kTTSymEmpty) {
            
            // create a model object (for j.view too !)
            *EXTRA->modelInfo = TTObject("ModelInfo", (TTPtr)x);
            
            // set his class attribute
            EXTRA->modelInfo->set("class", x->patcherClass);
            
            // suscribe it under a model node
            adrs = returnedAddress.appendAddress(TTAddress("model"));
            args = TTValue(adrs, *EXTRA->modelInfo, x->patcherPtr);
            
            if (JamomaApplication.send("ObjectRegister", args, none))
                pd_error((t_object*)x, "can't subscribe model object");
            
            // In model patcher : set model:address with the model address
			if (x->patcherContext == kTTSym_model) {
                
				EXTRA->modelInfo->set(kTTSym_address, returnedAddress);
                
                // then set the address attribute readOnly
                TTModelInfoPtr(EXTRA->modelInfo->instance())->setAddressReadOnly(YES);
            }
            
            // Get patcher arguments
			ac = 0;
			av = NULL;
			
			// If x is in a bpatcher, the patcher is NULL
            // AV : the patcher can't be NULL since it is check in the if statement above
            /*
			if (!aPatcher)
				aPatcher = (t_canvas*)object_attr_getobj(x, _sym_parentpatcher);
            */

			jamoma_patcher_get_args(aPatcher, &ac, &av);
			
			// check if it's a sub model
			if ( ac > 0 && av[0].a_type == A_SYMBOL )
				isSubModel = atom_getsym(av) == _sym_pd;
			
			// in subpatcher :
			if (jamoma_patcher_get_hierarchy(aPatcher) == _sym_subpatcher) {
                
                // remove first 'p' or 'patcher'
                if (ac > 0 && av) {
					if (atom_getsym(av) == _sym_pd) {
                        ac--;
                        av++;
                    }
                }
                
                // remove the next argument because it is the class
				ac--;
				av++;
			}
			
			// j.model case :
			if (x->patcherContext == kTTSym_model) {
                
				// use patcher arguments to setup the model attributes (like @priority and @amenities)
				if (ac && av)
					attr_args_process(x, ac, av);
                
                // Create internal TTTextHandler (for documention and preset management)
                aTextHandler = TTObject(kTTSym_TextHandler);
                x->internals->append(kTTSym_TextHandler, aTextHandler);
                
                if (!EXTRA->attr_amenities->lookup(TTSymbol("all"), v))
                    EXTRA->all_amenities = YES;
                
                if (!EXTRA->attr_amenities->lookup(TTSymbol("none"), v))
                    EXTRA->no_amenities = YES;
                
                // Add amenities relative to preset informations
                if (model_test_amenities(self, TTSymbol("preset")))
                    
                    // add preset managment features
                    model_preset_amenities(self);
                
                // Add amenities relative to signal informations
                if (model_test_amenities(self, TTSymbol("data")) || model_test_amenities(self, TTSymbol("audio"))) {
                    
                    // look at model's content to create signal in/out datas
                    model_signal_amenities(self, _sym_nothing, 0, NULL);
                    
                }
			}
			
			// In view patcher : see in model_subscribe_view
			if (x->patcherContext == kTTSym_view)
                model_subscribe_view(self, _sym_nothing, ac, av);

			// output node address
			t_atom a;
			x->subscriberObject.get("nodeAddress", v);
            
            if (v.size() == 1) {
                returnedAddress = v[0];
                atom_setsym(&a, gensym((char*)returnedAddress.c_str()));
                outlet_anything(x->dumpOut, gensym("address"),1,&a);
//                object_obex_dumpout(self, gensym("address"), 1, &a);
            }
			
			// init the model (but not subModel) or any model registered under the root
			if (!isSubModel || returnedContextNode->getParent()->getContext() == accessApplicationLocalDirectory->getRoot()->getContext())
				model_init(x);
		}
	}
}

void model_subscribe_view(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTObject        aReceiver;
    t_symbol*		hierarchy;
    t_object*		aPatcher;
    TTList          whereToSearch;
    TTBoolean       isThere;
    TTNodePtr       firstTTNode;
    TTAddress       modelAdrs, argAdrs, viewAdrs;
    TTValue         v;
    
    // look at hierarchy
    hierarchy = jamoma_patcher_get_hierarchy(x->patcherPtr);
    
    // if args exists, the first argument of the patcher is the model:address value
    if (argc > 0 && atom_gettype(argv) == A_SYM) {

        argAdrs = TTAddress(atom_getsym(argv)->s_name);
        
        // in poly case : use the same instance as the container address
        if (hierarchy == gensym("poly"))
            if (argAdrs.getInstance() == kTTSymEmpty)
                argAdrs = argAdrs.appendInstance(EXTRA->containerAddress.getInstance());
        
        // if the address is absolute : use it directly
        if (argAdrs.getType() == kAddressAbsolute) {
            
            // set the model:address attribute to notify all observers
            EXTRA->modelInfo->set(kTTSym_address, argAdrs);
            return;
        }
        
        // in case of relative address : try to use the upper view patcher model:address (else use root)
        else {
            
            // if there is a parent view address : get the upper view address
            if (EXTRA->containerAddress.getParent() != kTTAdrsRoot) {
                
                // keep the argument address
                EXTRA->argAddress = argAdrs;
                
                // observe the selected view model:address attribute
                makeInternals_receiver(self, EXTRA->containerAddress.getParent(), TTAddress("model:address"), gensym("return_upper_view_model_address"), aReceiver, YES); // we need to deferlow to avoid lock crash on TTContainer content
            }
            // use the argument address as an absolute address
            else {

                // set the model:address attribute to notify all observers
                EXTRA->modelInfo->set(kTTSym_address, kTTAdrsRoot.appendAddress(argAdrs));
                return;
            }
        }
    }
    // else look around the patcher for model of the same class
    else {
        
        // look for a model of the same class into the patcher of the bpatcher to get his model:address
		jamoma_patcher_get_model_patcher(x->patcherPtr, x->patcherClass, &aPatcher);
        
        // if a model exists
        if (aPatcher) {

            // is there a container (e.g. a j.model) registered with the same context in this model patcher ?
            whereToSearch.append(accessApplicationLocalDirectory->getRoot());
            accessApplicationLocalDirectory->IsThere(&whereToSearch, &testNodeContext, (TTPtr)aPatcher, &isThere, &firstTTNode);
            
            if (isThere) {
                
                firstTTNode->getAddress(modelAdrs);
                
                // set the model:address attribute to notify all observers
                EXTRA->modelInfo->set(kTTSym_address, modelAdrs);
                return;
            }
            
            // deferlow to try another time because the model patcher is maybe not ready
            else {

				// defer_low((t_object*)x, (method)model_subscribe_view, _sym_nothing, argc, argv);
				model_subscribe_view(x, _sym_nothing, argc, argv);
                return;
            }
        }
    }
    
    // check if the model address have been filled or not (see in model_return_upper_view_model_address)
    EXTRA->modelInfo->get(kTTSym_address, v);
    modelAdrs = v[0];
    
    // if the model:address is still empty : the view is not binding a model for instant
    if (modelAdrs == kTTSymEmpty)
        modelAdrs = TTAddress("/no_model_address");
    
    // set the model:address attribute to notify all observers
    EXTRA->modelInfo->set(kTTSym_address, modelAdrs);
}

void model_return_upper_view_model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTAddress       upperViewModelAddress;
    TTValue         v, o, out;
    
    jamoma_ttvalue_from_Atom(v, msg, argc, argv);
    upperViewModelAddress = v[0];
    
    // append argAddress stored in model_view_subscribe
    if (upperViewModelAddress != kTTSymEmpty)
        upperViewModelAddress = upperViewModelAddress.appendAddress(EXTRA->argAddress);
    else
        upperViewModelAddress = kTTAdrsRoot.appendAddress(EXTRA->argAddress);
    
    // set the model:address attribute to notify all observers
    EXTRA->modelInfo->set(kTTSym_address, upperViewModelAddress);
}

void model_init(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTBoolean	initialized;
	TTValue		v;
	
	// Check if the model has not been initialized by a upper model
	x->wrappedObject.get(kTTSym_initialized, v);
	initialized = v[0];
	if (!initialized)
		x->wrappedObject.send(kTTSym_Init);
}

// Method for Assistance Messages
void model_assist(TTPtr self, void *b, long msg, long arg, char *dst)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if (msg==1)			// Inlets
		strcpy(dst, "");		
	else {							// Outlets
		switch(arg) {
			case data_out:
				if (x->patcherContext == kTTSym_model)
					strcpy(dst, "model feeback");
				else if (x->patcherContext == kTTSym_view)
					strcpy(dst, "view feeback");
				else 
					strcpy(dst, "feeback");
				break;
			case dump_out:
				strcpy(dst, "dumpout");
				break;
		}
 	}
}

void model_share_patcher_info(TTPtr self, TTValuePtr patcherInfo)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    patcherInfo->clear();

	if (x->patcherPtr && x->patcherContext != kTTSymEmpty && x->patcherClass != kTTSymEmpty && x->patcherName != kTTSymEmpty) {
		patcherInfo->append((TTPtr)x->patcherPtr);
		patcherInfo->append(x->patcherContext);
		patcherInfo->append(x->patcherClass);
		patcherInfo->append(x->patcherName);
	}
}

void model_share_patcher_node(TTPtr self, TTNodePtr *patcherNode)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue v;
	
	if (x->subscriberObject.valid()) {
		x->subscriberObject.get("contextNode", v);
		*patcherNode = TTNodePtr((TTPtr)v[0]);
	}
}

void WrappedContainerClass_anything(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	// the msg have to contains a relative address
	jamoma_container_send(x->wrappedObject, msg, argc, argv);
}

void model_return_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    
    // store the address temporary into msg member to use it into model_return_value()
	x->msg = msg;
}

void model_return_value(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
    // using the temporay address stored previously into msg member in model_return_address()
	outlet_anything((t_outlet*)x->outlets[data_out], x->msg, argc, argv);
}

void model_reference_write(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
//	defer(self, (method)model_reference_dowrite, msg, argc, argv);
	model_reference_dowrite(self,msg,argc,argv);
}

void model_reference_dowrite(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	char				filename[MAX_FILENAME_CHARS];
	TTSymbol			fullpath;
	TTValue				o, v, none;
	TTObject            aTextHandler;
	TTErr				tterr;
	
	if (x->wrappedObject.valid()) {
		
		// Default HTML file name
		snprintf(filename, MAX_FILENAME_CHARS, DocumentationFormat->data(), x->patcherClass.c_str());
		fullpath = jamoma_file_write((t_object*)x, argc, argv, filename);
		v.append(fullpath);
		
		tterr = x->internals->lookup(kTTSym_TextHandler, o);
		
		if (!tterr) {
			aTextHandler = o[0];

			aTextHandler.set(kTTSym_object, x->wrappedObject);
			
//			critical_enter(0);
			aTextHandler.send(kTTSym_Write, v, none);
//			critical_exit(0);
		}
	}
}

void model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	// In view patcher only
	if (x->patcherContext == kTTSym_view) {
		
		if (atom_gettype(argv) == A_SYM) {
            
			TTAddress modelAdrs = TTAddress(atom_getsym(argv)->s_name);
            
            // set the model:address attribute to notify all observers
            EXTRA->modelInfo->set(kTTSym_address, modelAdrs);
		}
	}
}

t_max_err model_get_amenities(TTPtr self, TTPtr attr, long *ac, t_atom **av)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTValue keys;
    
    EXTRA->attr_amenities->getKeysSorted(keys);
	
	if ((*ac)&&(*av)) {
		//memory passed in, use it
	} else {
        
		//otherwise allocate memory
		*ac = keys.size();
		if (!(*av = (t_atom*)getbytes(sizeof(t_atom)*(*ac)))) {
			*ac = 0;
			return -4; // MAX_ERR_OUT_OF_MEM;
		}
	}
	
	jamoma_ttvalue_to_Atom(keys, ac, av);
	
	return 0; // MAX_ERR_NONE;
}

t_max_err model_set_amenities(TTPtr self, TTPtr attr, long ac, t_atom *av)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTValue     keys, none;
    TTSymbol    key;
    
    EXTRA->attr_amenities->clear();
	
	if (ac&&av) {
        
		jamoma_ttvalue_from_Atom(keys, _sym_nothing, ac, av);
        
        for (TTUInt8 i = 0; i < keys.size(); i++) {
            
            key = keys[i];
            EXTRA->attr_amenities->append(key, none);
        }
    }
	else
		EXTRA->attr_amenities->append(TTSymbol("all"), none); // default all
	
	return 0; // MAX_ERR_NONE;
}

TTBoolean model_test_amenities(TTPtr self, TTSymbol name)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTValue v;
    
    return !EXTRA->no_amenities && (EXTRA->all_amenities || !EXTRA->attr_amenities->lookup(name, v));
}
