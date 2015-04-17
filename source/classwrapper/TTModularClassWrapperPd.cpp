/*
 *	TTModularClassWrapperMax
 *	An automated class wrapper to make Jamoma object's available as objects for Puredata
 *	Copyright © 2008 by Timothy Place
 *      Pd port by Antoine Villeret 2015
 *
 * License: This code is licensed under the terms of the GNU LGPL
 * http://www.gnu.org/licenses/lgpl.html
 */

#include "TTModularClassWrapperPd.h"
#include "commonsyms.h"
// #include "ext_hashtab.h"

/** A hash of all wrapped clases, keyed on the Pd class name. */
// static t_hashtab*	wrappedPdClasses = NULL;
static std::map<std::string,t_object*> wrappedPdClasses;

t_object *wrappedModularClass_new(t_symbol *name, long argc, t_atom *argv)
{
    WrappedClass*				wrappedPdClass = NULL;
    WrappedModularInstancePtr	x = NULL;
	TTErr						err = kTTErrNone;
#ifdef UI_EXTERNAL
	long					flags;
	t_dictionary*			d = NULL;
	
	if (!(d=object_dictionaryarg(argc,argv)))
		return NULL;
#endif
	
	// Find the WrappedClass
    // hashtab_lookup(wrappedPdClasses, name, (t_object**)&wrappedPdClass);
    wrappedPdClass = (WrappedClass*)wrappedPdClasses[name->s_name];
	
	// If the WrappedClass has a validity check defined, then call the validity check function.
	// If it returns an error, then we won't instantiate the object.
    if (wrappedPdClass) {
        if (wrappedPdClass->validityCheck)
            err = wrappedPdClass->validityCheck(wrappedPdClass->validityCheckArgument);
		else
			err = kTTErrNone;
	}
	else
		err = kTTErrGeneric;
	
	if (!err)
        x = (WrappedModularInstancePtr)eobj_new(wrappedPdClass->pdClass);
	
    if (x) {
		
        x->wrappedClassDefinition = wrappedPdClass;
		
		x->useInternals = NO;
        x->internals = new TTHash();
		x->address = kTTAdrsEmpty;
		x->argv = NULL;
		x->iterateInternals = NO;
		
#ifdef ARRAY_EXTERNAL
		x->arrayFormatInteger = TTString();
		x->arrayFormatString = TTString();
#endif
        
        x->patcherPtr = NULL;
        x->patcherContext = kTTSymEmpty;
        x->patcherClass = kTTSymEmpty;
        x->patcherName = kTTSymEmpty;
        x->patcherAddress = kTTAdrsEmpty;
		
#ifdef UI_EXTERNAL
		flags = 0
		| JBOX_DRAWFIRSTIN		// 0
		| JBOX_NODRAWBOX		// 1
		| JBOX_DRAWINLAST		// 2
		//	| JBOX_TRANSPARENT		// 3
		//	| JBOX_NOGROW			// 4
		//	| JBOX_GROWY			// 5
		| JBOX_GROWBOTH			// 6
		//	| JBOX_IGNORELOCKCLICK	// 7
		//	| JBOX_HILITE			// 8
		//	| JBOX_BACKGROUND		// 9
		//	| JBOX_NOFLOATINSPECTOR	// 10
		//	| JBOX_TEXTFIELD		// 11
		//    | JBOX_MOUSEDRAGDELTA	// 12
		//	| JBOX_COLOR			// 13
		//	| JBOX_BINBUF			// 14
		//	| JBOX_DRAWIOLOCKED		// 15
		//	| JBOX_DRAWBACKGROUND	// 16
		//	| JBOX_NOINSPECTFIRSTIN	// 17
		//	| JBOX_DEFAULTNAMES		// 18
		//	| JBOX_FIXWIDTH			// 19
		;
		
		jbox_new(&x->box, flags, argc, argv);
		x->box.b_firstin = (t_object *)x;
		attr_dictionary_process(x,d);
		jbox_ready((t_jbox *)x);
#endif
		
		// Make specific things
        ModularSpec *spec = (ModularSpec*)wrappedPdClass->specificities;
		if (spec) {
			if (spec->_new)
				spec->_new((TTPtr)x, argc, argv);
		}
		else
			// handle attribute args
			attr_args_process(x, argc, argv);

        x->dumpOut = outlet_new((t_object*)x,NULL);

        // call an object loadbang method if it exists
        method _method = (method)getfn((t_pd*)x,_sym_loadbang);
        if (_method)
            _method(x);
	}

	return (t_object*)x;
}


void wrappedModularClass_unregister(WrappedModularInstancePtr x)
{
	TTValue		keys, v, none;
	TTSymbol	name;
	TTAddress	objectAddress;
	TTErr		err;
    
#ifndef ARRAY_EXTERNAL
    
	x->subscriberObject = TTObject();
    
    // check the wrappedObject is still valid because it could have been released in spec->_free method
	if (x->wrappedObject.valid()) {
        
        // don't release the local application
        if (!(x->wrappedObject.instance() == accessApplicationLocal)) {
            
            if (x->wrappedObject.instance()->getReferenceCount() > 1)
                object_error((t_object*)x, "there are still unreleased reference of the wrappedObject (refcount = %d)", x->wrappedObject.instance()->getReferenceCount() - 1);
            
            // this line should release the last instance of the wrapped object
            // otherwise there is something wrong
            x->wrappedObject = TTObject();
        }
    }

#endif
	
    if (!x->internals->isEmpty()) {
        
        err = x->internals->getKeys(keys);
        
        if (!err) {
            
            x->iterateInternals = YES;
            
            for (int i = 0; i < keys.size(); i++) {
                
                name = keys[i];
                err = x->internals->lookup(name, v);
                
                if (!err) {
                    
                    TTObject o = v[0];
                    
                    if (o.name() == kTTSym_Sender || o.name() == kTTSym_Receiver || o.name() == kTTSym_Viewer)
                        o.set(kTTSym_address, kTTAdrsEmpty);
                    
                    // absolute registration case : remove the address
                    if (v.size() == 2) {
                        objectAddress = v[1];
                        
                        JamomaDebug logpost((t_object*)x, 0, "Remove internal %s object at : %s", name.c_str(), objectAddress.c_str());
                        JamomaApplication.send("ObjectUnregister", objectAddress, none);
                    }
                }
            }
            x->iterateInternals = NO;
        }
        x->internals->clear();
    }
}


void wrappedModularClass_free(WrappedModularInstancePtr x)
{
	ModularSpec* spec = (ModularSpec*)x->wrappedClassDefinition->specificities;
    
    // call specific free method before freeing internal stuff
	if (spec->_free)
		spec->_free(x);
	
	wrappedModularClass_unregister(x);
	
	if (x->argv)
        free(x->argv);
	
#ifdef UI_EXTERNAL
	notify_free((t_object *)x);
	jbox_free((t_jbox *)x);
#endif
	
	x->argv = NULL;
    
    delete x->internals;
    x->internals = NULL;
}


t_max_err wrappedModularClass_notify(TTPtr self, t_symbol *s, t_symbol *msg, void *sender, void *data)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	ModularSpec*				spec = (ModularSpec*)x->wrappedClassDefinition->specificities;
	TTValue						v, none;
	TTAddress                   contextAddress;
    
#ifndef ARRAY_EXTERNAL
	t_object	*				context;
    
	if (x->subscriberObject.valid()) {
        
		x->subscriberObject.get("context", v);
		context = (t_object*)((TTPtr)v[0]);
		
		// if the patcher is deleted
		if (sender == context) {
			if (msg == _sym_free) {
				
				// delete the context node if it exists
				x->subscriberObject.get("contextAddress", v);
				contextAddress = v[0];
				
				JamomaApplication.send("ObjectUnregister", contextAddress, none);
				
				// delete
				x->subscriberObject = TTObject();
				
				// no more notification
                // TODO rewrite this for PD
                // object_detach_byptr((t_object*)x, context);
			}
		}
	}
#endif
	
	if (spec->_notify)
		spec->_notify(self, s, msg, sender, data);
	
#ifdef UI_EXTERNAL
	if (msg == _sym_modified)
		jbox_redraw(&x->box);
	else if ((msg == _sym_attr_modified) && (sender == x))
		jbox_redraw(&x->box);
	
	return jbox_notify((t_jbox*)x, s, msg, sender, data);
#else
    return 0; // MAX_ERR_NONE;
#endif
}


void wrappedModularClass_shareContextNode(TTPtr self, TTNodePtr *contextNode)
{
	TTValue	v;
#ifndef ARRAY_EXTERNAL
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    
	if (x->subscriberObject.valid()) {
		x->subscriberObject.get("contextNode", v);
		*contextNode = TTNodePtr((TTPtr)v[0]);
	}
	else
#endif
		*contextNode = NULL;
}


t_max_err wrappedModularClass_attrGet(TTPtr self, t_object *attr, long* argc, t_atom** argv)
{
	// t_symbol	*attrName = (t_symbol*)object_method(attr, _sym_getname);
	t_symbol	*attrName = ((t_eattr*)attr)->name;
	TTValue		v;
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
	t_max_err	err;
	TTPtr		ptr;
	
    // err = hashtab_lookup(x->wrappedClassDefinition->pdNamesToTTNames, attrName, (t_object**)&ptr);
    ptr = &x->wrappedClassDefinition->pdNamesToTTNames[attrName->s_name];
    if (ptr == NULL)
        return -2;
	
	TTSymbol	ttAttrName(ptr);
	
	if (selectedObject) {
		selectedObject->getAttributeValue(ttAttrName, v);
		jamoma_ttvalue_to_Atom(v, argc, argv);
	}
	
    return 0; // MAX_ERR_NONE;
}


t_max_err wrappedModularClass_attrSet(TTPtr self, t_object *attr, long argc, const t_atom *argv)
{
	WrappedModularInstancePtr x = (WrappedModularInstancePtr)self;
	// t_symbol	*attrName = (t_symbol*)object_method(attr, _sym_getname);
	t_symbol	*attrName = ((t_eattr*)attr)->name;
	TTValue		v;
	long        ac = 0;
	t_atom		*av = NULL;
	t_max_err	m_err;
	TTErr		err;
	TTPtr		ptr;
	
	// for an array of wrapped object
	if (x->useInternals && !x->iterateInternals) {
		
		TTValue		keys;
		
		// temporary set x->iterateInternals to YES
		x->iterateInternals = YES;
		
		// then recall this method for each element of the array
		if (!x->internals->isEmpty()) {
			err = x->internals->getKeys(keys);
			if (!err) {
				for (TTUInt32 i = 0; i < keys.size(); i++) {
					x->cursor = keys[i];
					wrappedModularClass_attrSet(self, attr, argc, argv);
				}
			}
		}
		
		// reset x->iterateInternals to NO
		x->iterateInternals = NO;
		
        return 0; // MAX_ERR_NONE;
	}
	
    // m_err = hashtab_lookup(x->wrappedClassDefinition->pdNamesToTTNames, attrName, (t_object**)&ptr);
    ptr = &x->wrappedClassDefinition->pdNamesToTTNames[attrName->s_name];
    if (ptr == NULL)
        return -2; // INVALID POINTER
	
	TTSymbol	ttAttrName(ptr);
	
	// set attribute's value
	if (argc && argv) {
		
		jamoma_ttvalue_from_Atom(v, _sym_nothing, argc, argv);
		
		if (selectedObject) {
			selectedObject->setAttributeValue(ttAttrName, v);
            return 0;  // MAX_ERR_NONE;
		}
		else
            return -1; // MAX_ERR_GENERIC;
        
	}
	// or get it and dumpout his value
	else {
		
		if (selectedObject) {
			// don't consider array case here (they should have all the same attribute value)
			selectedObject->getAttributeValue(ttAttrName, v);
			
			jamoma_ttvalue_to_Atom(v, &ac, &av);
			// object_obex_dumpout(self, attrName, ac, av);
			outlet_anything(x->dumpOut, attrName, ac, av);
            free(av);
            return 0; // MAX_ERR_NONE;
		}
		else
            return -1; // MAX_ERR_GENERIC;
	}
	
    return -1; // MAX_ERR_GENERIC;
}


void wrappedModularClass_anything(TTPtr self, t_symbol *s, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	ModularSpec*				spec = (ModularSpec*)x->wrappedClassDefinition->specificities;
	TTErr						err;
	
	// for an array of wrapped object
	if (x->useInternals && !x->iterateInternals) {
		
		// temporary set x->iterateInternals to YES
		x->iterateInternals = YES;
		
		// then recall this method for each element of the array
		if (!x->internals->isEmpty()) {
			
			TTUInt32	i = 0;
			TTValue		keys;
			TTSymbol	memoCursor;
			
			err = x->internals->getKeys(keys);
			if (!err) {
				
				memoCursor = x->cursor;
				while (i < keys.size() && !err) {
					
					x->cursor = keys[i];
                    
                    // Is it a message of the wrapped object ?
                    err = wrappedModularClass_sendMessage(self, s, argc, argv);
                    
                    // Is it an attribute of the wrapped object ?
                    if (err)
                        err = wrappedModularClass_setAttribute(self, s, argc, argv);
                    
                    // if error : stop the while because this is an array and all objects are the same
                    if (err)
                        break;
					
					i++;
				}
				x->cursor = memoCursor;
			}
			
			// don't iterate the specific anything method on each object of the array
			if (err && spec->_any)
				spec->_any(self, s, argc, argv);
		}
		
		// reset x->iterateInternals to NO
		x->iterateInternals = NO;
	}
	
	// for single wrapped object
	else {
		
		// Is it a message of the wrapped object ?
		if (!wrappedModularClass_sendMessage(self, s, argc, argv))
			return;
		
        // It could be an extended attribute (not registered in pdNamesToTTNames)
		// Is it an attribute of the wrapped object ?
		if (!wrappedModularClass_setAttribute(self, s, argc, argv))
			return;
		
		if (spec->_any)
			spec->_any(self, s, argc, argv);
	}
}


TTErr wrappedModularClass_sendMessage(TTPtr self, t_symbol *s, long argc, const t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue			inputValue, outputValue;
	TTSymbol		ttName;
	TTMessagePtr	aMessage = NULL;
	long            ac = 0;
	t_atom			*av = NULL;
	t_max_err		m_err;
	TTErr			err;
    TTPtr           ptr;
	
    // m_err = hashtab_lookup(x->wrappedClassDefinition->pdNamesToTTNames, s, (t_object**)&ptr);
    ptr = &x->wrappedClassDefinition->pdNamesToTTNames[s->s_name];
    if (ptr != NULL ) {
		
		// Is it a message of the wrapped object ?
        ttName = TTSymbol(ptr);
		err = selectedObject->findMessage(ttName, &aMessage);
		if (!err) {
			// send message
			if (argc && argv) {
				
				jamoma_ttvalue_from_Atom(inputValue, _sym_nothing, argc, argv);
				selectedObject->sendMessage(ttName, inputValue, outputValue);
				
				jamoma_ttvalue_to_Atom(outputValue, &ac, &av);
//				object_obex_dumpout(self, s, ac, av);
				outlet_anything(x->dumpOut, s, ac, av);
				free(av);
			}
			else
				selectedObject->sendMessage(ttName);
		}
		
		return err;
	}
	else
		return kTTErrGeneric;
}


TTErr wrappedModularClass_setAttribute(TTPtr self, t_symbol *s, long argc, const t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue			inputValue, outputValue;
	TTSymbol		ttName;
	TTAttributePtr	anAttribute= NULL;
	long            ac = 0;
	t_atom			*av = NULL;
	TTErr			err;
	
	err = selectedObject->findAttribute(TTSymbol(s->s_name), &anAttribute);
	if (!err) {
		
		// set attribute's value
		if (argc && argv) {
			jamoma_ttvalue_from_Atom(inputValue, _sym_nothing, argc, argv);
			selectedObject->setAttributeValue(TTSymbol(s->s_name), inputValue);
		}
		// or get it and dumpout his value
		else {
			selectedObject->getAttributeValue(TTSymbol(s->s_name), outputValue);
			
			jamoma_ttvalue_to_Atom(outputValue, &ac, &av);
//			object_obex_dumpout(self, s, ac, av);
			outlet_anything(x->dumpOut, s, ac, av);
			free(av);
        }
	}
	
	return err;
}


void wrappedModularClass_dump(TTPtr self)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTValue			names, v;
    TTUInt32		i;
    TTSymbol		aName, address;
    t_symbol		*s;
    long            ac;
    t_atom			*av;
	
#ifndef ARRAY_EXTERNAL
    t_atom			a;
	
    if (x->subscriberObject.valid())
    {
    	// send out the absolute address of the subscriber
        x->subscriberObject.get("nodeAddress", v);
        address = v[0];
        atom_setsym(&a, gensym((char *) address.c_str()));
//        object_obex_dumpout(self, gensym("address"), 1, &a);
		outlet_anything(x->dumpOut, gensym("address"), 1, &a);
    }
#endif
    
    selectedObject->getAttributeNames(names);
	
    for (i = 0; i < names.size(); i++) {
		
        aName = names[i];
		
		selectedObject->getAttributeValue(aName, v);
		
        s = jamoma_TTName_To_PdName(aName);
		
		ac = 0;
		av = NULL;
		jamoma_ttvalue_to_Atom(v, &ac, &av);
//		object_obex_dumpout(self, s, ac, av);
		outlet_anything(x->dumpOut, s, ac, av);
		free(av);
    }
}


#ifdef UI_EXTERNAL
void wrappedModularClass_paint(WrappedModularInstancePtr x, t_object *view)
{
	t_rect			rect;
	t_rect			r;
	t_jgraphics     *g;
	t_jsurface		*jsurface;
	unsigned char*	data;
	TTValue			v;
	TTErr			err;
	TTInt32			width;
	TTInt32			height;
	TTInt32			stride;
	
	g = (t_jgraphics*)patcherview_get_jgraphics(view);		// obtain graphics context
	jbox_get_rect_for_view((t_object *)x, view, &rect);		// this is the box rectangle -- but we draw relative to 0 0, and thus only care about width & height
	
	v.resize(2);
	v[0] = rect.width;
	v[1] = rect.height;
	err = selectedObject->sendMessage(TTSymbol("resize"), v);
	err = selectedObject->sendMessage(TTSymbol("paint"));
	err = selectedObject->sendMessage(TTSymbol("getData"), v);
	if (!err) {
		data = (unsigned char*)TTPtr(v);
		v[1] width;
		v[2] height;
		v[3] stride;
		
		jsurface = jgraphics_image_surface_create_for_data(data, JGRAPHICS_FORMAT_ARGB32, width, height, stride, NULL, NULL);
		
		r.x = 0;
		r.y = 0;
		r.width = rect.width;
		r.height = rect.height;
		jgraphics_image_surface_draw(g, jsurface, r, r);
		jgraphics_surface_destroy(jsurface);
	}
}


TTPtr wrappedModularClass_oksize(TTPtr self, t_rect *newrect)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	v.resize(4);
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = newrect->width;
	v[3] = newrect->height;
	
	selectedObject->sendMessage(TTSymbol("verifyResize"), v);
	
	v[2] newrect->width;
	v[3] newrect->height;
	return (void *)1;
}


void wrappedModularClass_mousedblclick(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseDoubleClicked"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mousedown(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseDown"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mousedrag(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseDragged"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mouseup(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseUp"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mouseenter(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseEntered"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mousemove(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseMoved"), v);
	jbox_redraw((t_jbox *)x);
}


void wrappedModularClass_mouseleave(TTPtr self, t_object *patcherview, t_pt pt, long modifiers)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue					v;
	
	vresize(3);
	v[0] = pt.x;
	v[1] = pt.y;
    v[2] = convertModifiersFromPdToTTGraphics(modifiers);
	selectedObject->sendMessage(TTSymbol("mouseExited"), v);
	jbox_redraw((t_jbox *)x);
}


int convertModifiersFromPdToTTGraphics(int pdModifiers)
{
	int ttGraphicsModifiers = 0;
	
    if (pdModifiers & eCapsLock)
		ttGraphicsModifiers |= TTModifierCapslock;
    else if (pdModifiers & eShiftKey)
		ttGraphicsModifiers |= TTModifierShiftKey;
    else if (pdModifiers & eControlKey)
		ttGraphicsModifiers |= TTModifierControlKey;
    else if (pdModifiers & eAltKey)
		ttGraphicsModifiers |= TTModifierAltKey;
    else if (pdModifiers & eCommandKey)
		ttGraphicsModifiers |= TTModifierCommandKey;
	
	return ttGraphicsModifiers;
}
#endif


TTErr wrapTTModularClassAsPdClass(TTSymbol& ttblueClassName, const char* pdClassName, WrappedClassPtr* c, ModularSpec* specificities)
{
	TTObject        o;
	TTValue			v, args;
    WrappedClass*	wrappedPdClass = NULL;
	TTSymbol		TTName;
    t_symbol		*PdName = NULL;
    TTUInt16        i;
    t_symbol* s_pdClassName = gensym(pdClassName);
	
	// AV : why do we need to do initialize this each time a new class is instanciated ?
    // jamoma_init();
    // common_symbols_init();
    
#ifdef UI_EXTERNAL
	TTGraphicsInit();
#endif
	
    wrappedPdClass = new WrappedClass;
    wrappedPdClass->pdClassName = gensym(pdClassName);
    wrappedPdClass->pdClass = eclass_new( s_pdClassName->s_name,
										  (method)wrappedModularClass_new,
										  (method)wrappedModularClass_free,
										  sizeof(WrappedModularInstance),
                                          0,
                                          A_GIMME,
										  0);
    wrappedPdClass->ttblueClassName = ttblueClassName;
    wrappedPdClass->validityCheck = NULL;
    wrappedPdClass->validityCheckArgument = NULL;
    wrappedPdClass->options = NULL;
    // wrappedPdClass->pdNamesToTTNames = hashtab_new(0);

    wrappedPdClass->specificities = specificities;
    
#ifdef UI_EXTERNAL
    jbox_initclass(wrappedPdClass->pdClass, flags);
    wrappedPdClass->pdClass->c_flags |= CLASS_FLAG_NEWDICTIONARY; // to specify dictionary constructor
#endif
    
#ifdef AUDIO_EXTERNAL
    // Setup our class to work with MSP
    class_dspinit(wrappedPdClass->pdClass);
#endif
	
	// Create a temporary instance of the class so that we can query it.
    try {
      o = TTObject(ttblueClassName);
    } catch (TTException e){
        error("can't create Jamoma %s object instance. The reason invoked is : %s",ttblueClassName.c_str(),e.getReason());
        return (TTErr) -1; // generic error
    }

	
    // Register Messages as Pd method
	o.messages(v);
	for (i = 0; i < v.size(); i++) {
		TTName = v[i];
        
#ifdef UI_EXTERNAL
		if (TTName == TTSymbol("mouseDown"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mousedown,	"mousedown",	A_CANT, 0);
		else if (TTName == TTSymbol("mouseDragged"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mousedrag,	"mousedrag",	A_CANT, 0);
		else if (TTName == TTSymbol("mouseUp"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mouseup,		"mouseup",		A_CANT, 0);
		else if (TTName == TTSymbol("mouseEntered"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mouseenter,	"mouseenter",	A_CANT, 0);
		else if (TTName == TTSymbol("mouseExited"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mouseleave,	"mouseleave",	A_CANT, 0);
		else if (TTName == TTSymbol("mouseMoved"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mousemove,	"mousemove",	A_CANT, 0);
		else if (TTName == TTSymbol("mouseDoubleClicked"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_mousedblclick,"mousedoubleclick",	A_CANT, 0);
		else if (TTName == TTSymbol("verifyResize"))
            class_addmethod(wrappedPdClass->pdClass, (method)wrappedUIClass_oksize,		"oksize",		A_CANT, 0);
		else
#endif
            if (TTName == TTSymbol("test")                      ||
                TTName == TTSymbol("getProcessingBenchmark")    ||
                TTName == TTSymbol("resetBenchmarking"))
                continue;
            else if ((PdName = jamoma_TTName_To_PdName(TTName))) {
                // hashtab_store(wrappedPdClass->pdNamesToTTNames, PdName, (t_object*)(TTName.rawpointer()));
                wrappedPdClass->pdNamesToTTNames[PdName->s_name]=(t_object*)(TTName.rawpointer());
                eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_anything, PdName->s_name, A_GIMME, 0);
            }
	}
	
    // Register Attributes as Pd attr
	o.attributes(v);
	for (i = 0; i < v.size(); i++) {
		TTAttributePtr	attr = NULL;
		t_symbol		*pdType = _sym_long;
		
		TTName = v[i];
        
#ifdef AUDIO_EXTERNAL
        // the enable word is already used by a message declared in the dsp_init method
        if (TTName == TTSymbol("enable"))
            continue;
#endif
        
        // we want to hide service attribute for Pd external
        if (TTName == TTSymbol("service"))
            continue;
		
        if ((PdName = jamoma_TTName_To_PdName(TTName))) {
            
            if (TTName == kTTSym_bypass && wrappedPdClass->pdClassName != gensym("j.in") && wrappedPdClass->pdClassName != gensym("j.in~"))
                continue;
            
			o.instance()->findAttribute(TTName, &attr);
            wrappedPdClass->pdNamesToTTNames[PdName->s_name] = (t_object*)(TTName.rawpointer());
            // hashtab_store(wrappedPdClass->pdNamesToTTNames, PdName, (t_object*)(TTName.rawpointer()));

			if (attr->type == kTypeFloat32)
				pdType = _sym_float;
			else if (attr->type == kTypeFloat64)
				pdType = _sym_float;
			else if (attr->type == kTypeSymbol || attr->type == kTypeString)
				pdType = _sym_symbol;
			else if (attr->type == kTypeLocalValue)
				pdType = _sym_atom;

			eclass_new_attr_typed(wrappedPdClass->pdClass,PdName->s_name,pdType->s_name,1,0,0,0);
			CLASS_ATTR_ACCESSORS(wrappedPdClass->pdClass,PdName->s_name,wrappedModularClass_attrGet,wrappedModularClass_attrSet);

			// class_addattr(wrappedPdClass->pdClass, attr_offset_new(PdName->s_name, pdType, 0, (method)wrappedModularClass_attrGet, (method)wrappedModularClass_attrSet, 0));
			
			// Add display styles for the Max 5 inspector
			if (attr->type == kTypeBoolean)
                CLASS_ATTR_STYLE(wrappedPdClass->pdClass, (char*)TTName.c_str(), 0, "onoff");
			if (TTName == TTSymbol("fontFace"))
                CLASS_ATTR_STYLE(wrappedPdClass->pdClass,	"fontFace", 0, "font");
		}
	}
	
#ifdef UI_EXTERNAL
    class_addmethod(wrappedPdClass->pdClass, (method)wrappedClass_paint,					"paint",				A_CANT, 0L);
#endif
    // eclass_addmethod(wrappedPdClass->pdClass, (method)stdinletinfo,							"inletinfo",			A_CANT, 0);
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_notify,			"notify",				A_CANT, 0);
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_shareContextNode,	"share_context_node",	A_CANT,	0);
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_anything,			"anything",				A_GIMME, 0);
	
	// Register specific methods and do specific things
	if (specificities) {
		if (specificities->_wrap)
            specificities->_wrap(wrappedPdClass);
	}
	
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_dump,				"dump",					A_GIMME, 0);
    
#ifdef UI_EXTERNAL
    CLASS_ATTR_DEFAULT(wrappedPdClass->pdClass,	"patching_rect",	0,	"0. 0. 160. 160.");
    CLASS_ATTR_MIN(wrappedPdClass->pdClass,		"patching_size",	0,	"1. 1.");
#endif
	
#ifdef ARRAY_EXTERNAL
	
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_ArraySelect,				"array/select",			A_GIMME,0);
    eclass_addmethod(wrappedPdClass->pdClass, (method)wrappedModularClass_ArrayResize,				"array/resize",			A_LONG,0);
	
    CLASS_ATTR_SYMBOL(wrappedPdClass->pdClass,			"format",	0,		WrappedModularInstance,	arrayAttrFormat);
    CLASS_ATTR_ACCESSORS(wrappedPdClass->pdClass,		"format",			wrappedModularClass_FormatGet,	wrappedModularClass_FormatSet);
    // CLASS_ATTR_ENUM(wrappedPdClass->pdClass,			"format",	0,		"single array");
#endif
	
    eclass_register(_sym_box, wrappedPdClass->pdClass);
	if (c)
        *c = wrappedPdClass;
	
    // hashtab_store(wrappedPdClasses, wrappedPdClass->pdClassName, (t_object*)(wrappedPdClass));
    wrappedPdClasses[wrappedPdClass->pdClassName->s_name] = (t_object*)(wrappedPdClass);
	return kTTErrNone;
}


TTErr makeInternals_data(TTPtr self, TTAddress address, TTSymbol name, t_symbol *callbackMethod, TTPtr context, TTSymbol service, TTObject& returnedData, TTBoolean deferlow)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue			baton, v, out;
	TTAddress       dataAddress;
    TTSymbol        dataName;
    
	returnedData = TTObject(kTTSym_Data, service);
    
    baton = TTValue(TTPtr(x), TTPtr(callbackMethod), deferlow);
	
    returnedData.set(kTTSym_baton, baton);
	returnedData.set(kTTSym_function, TTPtr(&jamoma_callback_return_value));
	
	// absolute registration
	dataAddress = address.appendAddress(TTAddress(name));
    v = TTValue(dataAddress, returnedData, context);
	JamomaApplication.send("ObjectRegister", v, out);
	
	dataAddress = out[0];
	dataName = dataAddress.getNameInstance();
    
	// absolute registration case : set the address in second position (see in unregister method)
	v = TTValue(returnedData, dataAddress);
	x->internals->append(dataName, v);
	
    JamomaDebug logpost((t_object*)x, 0, "makes internal \"%s\" %s at : %s", dataName.c_str(), service.c_str(), dataAddress.c_str());
	
	return kTTErrNone;
}


TTErr makeInternals_explorer(TTPtr self, TTSymbol name, t_symbol *callbackMethod, TTObject& returnedExplorer, TTBoolean deferlow)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue		v, args, baton;
	TTObject    returnValueCallback;
    
    // check the internals do not exist yet
    if (!x->internals->lookup(name, v)) {
        returnedExplorer = v[0];
        JamomaDebug logpost((t_object*)x, 0, "makeInternals_explorer : \"%s\" internal already exists", name.c_str());
        return kTTErrNone;
    }
	
	// prepare arguments
	returnValueCallback = TTObject("callback");
    
	baton = TTValue(TTPtr(x), TTPtr(callbackMethod), deferlow);
    
	returnValueCallback.set(kTTSym_baton, baton);
	returnValueCallback.set(kTTSym_function, TTPtr(&jamoma_callback_return_value));
	args.append(returnValueCallback);
	
	args.append((TTPtr)jamoma_explorer_default_filter_bank());
	
	returnedExplorer = TTObject(kTTSym_Explorer, args);
	
	// default registration case : store object only (see in unregister method)
	x->internals->append(name, returnedExplorer);
    
    JamomaDebug logpost((t_object*)x, 0, "makes internal \"%s\" explorer", name.c_str());
    
	return kTTErrNone;
}


TTErr makeInternals_viewer(TTPtr self, TTAddress address, TTSymbol name, t_symbol *callbackMethod, TTObject& returnedViewer, TTBoolean deferlow)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue			v, baton;
	TTAddress       adrs;
    
    // check the internals do not exist yet
    if (!x->internals->lookup(name, v)) {
        returnedViewer = v[0];
        JamomaDebug logpost((t_object*)x, 0, "makeInternals_viewer : \"%s\" internal already exists", name.c_str());
        return kTTErrNone;
    }
	
	returnedViewer = TTObject(kTTSym_Viewer);
    
    baton = TTValue(TTPtr(x), TTPtr(callbackMethod), deferlow);
    returnedViewer.set(kTTSym_baton, baton);
	returnedViewer.set(kTTSym_function, TTPtr(&jamoma_callback_return_value));
	
	// edit address
	adrs = address.appendAddress(TTAddress(name));
	
	// default registration case : store object only (see in unregister method)
	x->internals->append(name, returnedViewer);
    
    // set address attribute (after registration as the value can be updated in the same time)
	returnedViewer.set(kTTSym_address, adrs);
    
    JamomaDebug logpost((t_object*)x, 0, "makes internal \"%s\" viewer to bind on : %s", name.c_str(), adrs.c_str());
    
	return kTTErrNone;
}


TTErr makeInternals_receiver(TTPtr self, TTAddress address, TTSymbol name, t_symbol *callbackMethod, TTObject& returnedReceiver, TTBoolean deferlow, TTBoolean appendNameAsAttribute)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue			v, args, baton;
	TTObject        returnValueCallback, empty;
	TTAddress       adrs;
    
    // check the internals do not exist yet
    if (!x->internals->lookup(name, v)) {
        returnedReceiver = v[0];
        JamomaDebug logpost((t_object*)x, 0, "makeInternals_receiver : \"%s\" internal already exists", name.c_str());
        returnedReceiver.send("Get");
        return kTTErrNone;
    }
	
	// prepare arguments
	
	// we don't want the address back
	args.append(empty);
	
	returnValueCallback = TTObject("callback");
    
	baton = TTValue(TTPtr(x), TTPtr(callbackMethod), deferlow);
    
	returnValueCallback.set(kTTSym_baton, baton);
	returnValueCallback.set(kTTSym_function, TTPtr(&jamoma_callback_return_value));
	args.append(returnValueCallback);
	
	returnedReceiver = TTObject(kTTSym_Receiver, args);
	
	// edit address
	if (appendNameAsAttribute)
        adrs = address.appendAttribute(name);
    else
        adrs = address.appendAddress(TTAddress(name.c_str()));
	
	// default registration case : store object only (see in unregister method)
	x->internals->append(name, returnedReceiver);
    
    // set address attribute (after registration as the value can be updated in the same time)
    returnedReceiver.set(kTTSym_address, adrs);
    
    JamomaDebug logpost((t_object*)x, 0, "makes internal \"%s\" receiver to bind on : %s", name.c_str(), adrs.c_str());
    
	return kTTErrNone;
}

TTErr makeInternals_sender(TTPtr self, TTAddress address, TTSymbol name, TTObject& returnedSender, TTBoolean appendNameAsAttribute)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    TTValue     v;
	TTAddress   adrs;
    
    // check the internals do not exist yet
    if (!x->internals->lookup(name, v)) {
        returnedSender = v[0];
        JamomaDebug logpost((t_object*)x, 0, "makeInternals_sender : \"%s\" internal already exists", name.c_str());
        return kTTErrNone;
    }
	
	returnedSender = TTObject(kTTSym_Sender);
	
	// edit address
	if (appendNameAsAttribute)
        adrs = address.appendAttribute(name);
    else
        adrs = address.appendAddress(TTAddress(name.c_str()));
	
	// default registration case : store object only (see in unregister method)
	x->internals->append(name, returnedSender);
    
    // set address attribute
	returnedSender.set(kTTSym_address, adrs);
    
    JamomaDebug logpost((t_object*)x, 0, "makes internal \"%s\" sender to bind on : %s", name.c_str(), adrs.c_str());
    
	return kTTErrNone;
}

TTErr removeInternals_data(TTPtr self, TTAddress address, TTAddress name)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue		v, none;
	TTAddress   dataAddress;
	TTErr		err;
	
	err = x->internals->lookup(name, v);
	
	if (!err) {
		
		dataAddress = v[1];
		
        JamomaDebug logpost((t_object*)x, 0, "Remove internal %s object at : %s", name.c_str(), dataAddress.c_str());
		JamomaApplication.send("ObjectUnregister", dataAddress, none);

		x->internals->remove(name);
	}
	
	return kTTErrNone;
}


TTObjectBasePtr	getSelectedObject(WrappedModularInstancePtr x)
{
	if (x->useInternals) {
		TTValue     v;
		TTObject    o;
		TTErr       err;
        
		err = x->internals->lookup(x->cursor, v);
		if (!err)
			o = v[0];
		
		return o.instance();
	}
#ifndef ARRAY_EXTERNAL
	else
		return x->wrappedObject.instance();
#else
	else
		return NULL;
#endif
}


void copy_msg_argc_argv(TTPtr self, t_symbol *msg, long argc, const t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTBoolean	copyMsg = false;
    TTUInt32	i;
	
	if (msg != _sym_nothing && msg != _sym_int && msg != _sym_float && msg != _sym_symbol && msg != _sym_list)
		copyMsg = true;
	
	x->msg = msg;
    
    // prepare memory if needed
    if (x->argv == NULL) {
        x->argc = argc + copyMsg;
        x->argv = (t_atom*)sysmem_newptr(sizeof(t_atom) * x->argc);
    }
    // or resize memory if needed
    else if (x->argc != argc + copyMsg) {
        x->argc = argc + copyMsg;
        free(x->argv);
        x->argv = (t_atom*)sysmem_newptr(sizeof(t_atom) * x->argc);
    }
    
    // copy
	if (x->argc && x->argv) {
        
		if (copyMsg) {
			atom_setsym(&x->argv[0], msg);
			for (i = 1; i < x->argc; i++)
				x->argv[i] = argv[i-1];
		}
		else
			for (i = 0; i < x->argc; i++)
				x->argv[i] = argv[i];
	}
}


#ifdef ARRAY_EXTERNAL
t_max_err wrappedModularClass_FormatGet(TTPtr self, TTPtr attr, long *ac, t_atom **av)
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
	
	atom_setsym(*av, x->arrayAttrFormat);
	
    return 0; // MAX_ERR_NONE;
}


t_max_err wrappedModularClass_FormatSet(TTPtr self, TTPtr attr, long ac, t_atom *av)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	if (ac&&av) {
		x->arrayAttrFormat = atom_getsym(av);
	} else {
		// no args, set to single
		x->arrayAttrFormat = gensym("single");
	}
    return 0; // MAX_ERR_NONE;
}


void wrappedModularClass_ArraySelect(TTPtr self, t_symbol *msg, long ac, t_atom *av)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	t_symbol					*instanceAddress;
	TTUInt8						i;
	TTValue						v;
	
	if (!x->internals->isEmpty()) {
		
		if (ac && av) {
			if (atom_gettype(av) == A_LONG) {
				i = atom_getlong(av);
				if (i > 0 && i <= x->arraySize) {
					x->arrayIndex = i;
					jamoma_edit_numeric_instance(x->arrayFormatInteger, &instanceAddress, i);
					x->cursor = TTSymbol(instanceAddress->s_name);
				}
				else
					object_error((t_object*)x, "array/select : %ld is not a valid index", i);
			}
			else if (atom_gettype(av) == A_SYM) {
				
				if (atom_getsym(av) == gensym("*")) {
					x->arrayIndex = 0;
					jamoma_edit_numeric_instance(x->arrayFormatInteger, &instanceAddress, 1);
					x->cursor = TTSymbol(instanceAddress->s_name);
				}
				else
					object_error((t_object*)x, "array/select : %s is not a valid index", atom_getsym(av)->s_name);
			}
		}
		else {
			if (msg == gensym("*")) {
				x->arrayIndex = 0;
				jamoma_edit_numeric_instance(x->arrayFormatInteger, &instanceAddress, 1);
				x->cursor = TTSymbol(instanceAddress->s_name);
			}
			else
				object_error((t_object*)x, "array/select : %s is not a valid index", msg->s_name);
		}
	}
	else
		object_error((t_object*)x, "array/select : the array is empty");
}


void wrappedModularClass_ArrayResize(TTPtr self, long newSize)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    t_symbol	*instanceAddress;
    TTString    s_bracket;
    TTValue     v;
    
    if (newSize >= 0) {
        
        v = TTInt64(newSize);
        v.toString();
        
        s_bracket = "[";
        s_bracket += TTString(v[0]);
        s_bracket += "]";
        
        jamoma_edit_string_instance(x->arrayFormatString, &instanceAddress, s_bracket.c_str());
        
        getfn((t_pd*)x,gensym("address"))((t_object*)x,instanceAddress,0,NULL);
//        object_method((t_object*)x, gensym("address"), instanceAddress, 0, NULL);
        JamomaDebug logpost((t_object*)x, 3,"array/resize : to %s address", instanceAddress->s_name);
    }
    else
        object_error((t_object*)x, "array/resize : %d is not a valid size", newSize);
}
#endif

void object_obex_dumpout(TTPtr self, t_symbol* msg, int argc, t_atom* argv)
{
    outlet_anything(((WrappedModularInstancePtr)self)->dumpOut,msg,argc,argv);
}
