/** @file
 *
 * @ingroup implementationMaxExternals
 *
 * @brief j.send / j.send~ : Send messages and audio through remote communication
 *
 * @details
 *
 * @authors Théo de la Hogue, Trond Lossius
 *
 * @copyright © 2010 by Théo de la Hogue @n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#include "TTModularClassWrapperPd.h"

#ifdef JCOM_SEND_TILDE

#define info_numChannels 0
#define info_vectorSize 1

#endif

// Definitions

/** Wrap the j.send class as a Max object.
 @param c			The class to be wrapped
 @see				WrappedSenderClass_new, WrappedSenderClass_free
 */
void		WrapTTSenderClass(WrappedClassPtr c);

/** Wrapper for the j.send constructor class, called when an instance is created. 
 @param self		Pointer to this object.
 @param argc		The number of arguments passed to the object.
 @param argv		Pointer to an array of atoms passed to the object.
 @see				WrappedSenderClass_free, send_subscribe
 */
void		WrappedSenderClass_new(TTPtr self, long argc, t_atom *argv);

/** Wrapper for the j.send deconstructor class, called when an instance is destroyed. 
 @param self		Pointer to this object.
 @see				WrappedSenderClass_new
 */
void		WrappedSenderClass_free(TTPtr self);

/** Assistance Method. 
 @param self		Pointer to this object.
 @param b			Pointer to (exactly what?)
 @param msg			The message passed to the object.
 @param arg			
 @param dst			Pointer to the destination that assistance strings are passed to for display.
 */
void		send_assist(TTPtr self, void *b, long msg, long arg, char *dst);

/** Associate j.send(~) with NodeLib. This is a prerequisit for communication with other Jamoma object in the module and beyond.  */
void		send_subscribe(TTPtr self);

/** Internal method called when the model:address parameter changed. It allows relative address binding.
 @param self		Pointer to this object.
 @param msg			The message sent to this object.
 @param argc		The number of arguments passed to the object.
 @param argv		Pointer to an array of atoms passed to the object.
 @see				send_subscribe
  */
void		send_return_model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

#ifdef JCOM_SEND_TILDE

/** j.send~ 32-bit MSP perform method (for Max 5). Only defineed for j.send~. */
t_int*		send_perform(t_int *w);

/** j.send~ 32-bit DSP method (for Max 5).Only defineed for j.send~. */
void		send_dsp(TTPtr self, t_signal **sp, short *count);

/** j.send~ 64-bit MSP perform method (for Max 6). Only defineed for j.send~. */
void		send_perform64(TTPtr self, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam);

/** j.send~ 64-bit DSP method (for Max 6). Only defineed for j.send~. */
void		send_dsp64(TTPtr self, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);

#else

/** bang handler for j.send 
 @param self		Pointer to this object.
 @see				send_int, send_float, send_list, WrappedSenderClass_anything
 */
void		send_bang(TTPtr self);

/** int handler for j.send 
 @param self		Pointer to this object.
 @param value		The value sent to this object.
 @see				send_bang, send_float, send_list, WrappedSenderClass_anything
 */
void		send_int(TTPtr self, long value);

/** float handler for j.send 
 @param self		Pointer to this object.
 @param value		The value sent to this object.
 @see				send_bang, send_int, send_list, WrappedSenderClass_anything
 */
void		send_float(TTPtr self, double value);

/** list handler for j.send 
 @param self		Pointer to this object.
 @param msg			The message sent to this object.
 @param argc		The number of arguments passed to the object.
 @param argv		Pointer to an array of atoms passed to the object.
 @see				send_bang, send_int, send_float, WrappedSenderClass_anything
 */
void		send_list(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

/** anything else handler for j.send 
 @param self		Pointer to this object.
 @param msg			The message sent to this object.
 @param argc		The number of arguments passed to the object.
 @param argv		Pointer to an array of atoms passed to the object.
 @see				send_bang, send_int, send_float, send_list
 */
void		WrappedSenderClass_anything(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

/** Internal method used to send data to a j.in. 
 @param self		Pointer to this object.
 @param msg			The message sent to this object.
 @param argc		The number of arguments passed to the object.
 @param argv		Pointer to an array of atoms passed to the object.
 @see				send_list
 */
void		send_input(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

#endif

/** address message handler for j.send. To change the address to bind.
 @param self		Pointer to this object.
 @param address		The address to bind
 @see				send_subscribe
 */
void		send_address(TTPtr self, t_symbol *address);

#pragma mark -
#pragma mark main

extern "C" void JAMOMA_EXPORT_MAXOBJ setup_j0x2esend(void)
{
	ModularSpec *spec = new ModularSpec;
	spec->_wrap = &WrapTTSenderClass;
	spec->_new = &WrappedSenderClass_new;
	spec->_free = &WrappedSenderClass_free;
#ifdef JCOM_SEND_TILDE
	spec->_any = NULL;
#else
	spec->_any = &WrappedSenderClass_anything;
#endif
	
#ifdef JCOM_SEND_TILDE
	return (void)wrapTTModularClassAsPdClass(kTTSym_Sender, "j.send~", NULL, spec);
#else
	return (void)wrapTTModularClassAsPdClass(kTTSym_Sender, "j.send", NULL, spec);
#endif
}

void WrapTTSenderClass(WrappedClassPtr c)
{
	eclass_addmethod(c->pdClass, (method)send_assist,					"assist",					A_CANT, 0L);
	
	eclass_addmethod(c->pdClass, (method)send_return_model_address,		"return_model_address",		A_CANT, 0);
	
#ifdef JCOM_SEND_TILDE	
	eclass_addmethod(c->pdClass, (method)send_dsp,						"dsp",						A_CANT, 0L);
	eclass_addmethod(c->pdClass, (method)send_dsp64,					"dsp64",					A_CANT, 0);
#else
	eclass_addmethod(c->pdClass, (method)send_bang,						"bang",						A_NULL, 0L);
	eclass_addmethod(c->pdClass, (method)send_int,						"int",						A_LONG, 0L);
	eclass_addmethod(c->pdClass, (method)send_float,					"float",					A_FLOAT, 0L);
	eclass_addmethod(c->pdClass, (method)send_list,						"list",						A_GIMME, 0L);
	eclass_addmethod(c->pdClass, (method)WrappedSenderClass_anything,	"symbol",					A_SYM, 0L);
#endif
	
	eclass_addmethod(c->pdClass, (method)send_address,					"address",					A_SYM, 0L);

	eclass_addmethod(c->pdClass, (method)send_subscribe, "loadbang", A_NULL, 0);
    // no class_dspinit : it is done in wrapTTModularClassAsMaxClass for AUDIO_EXTERNAL
}

#pragma mark -
#pragma mark Object life

void WrappedSenderClass_new(TTPtr self, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	t_symbol					*address;
 	long						attrstart = attr_args_offset(argc, argv);			// support normal arguments
    t_atom						a[1];
	
	// read first argument
	if (attrstart && argv) 
		address = atom_getsym(argv);
	else
		address = _sym_nothing;
	
//	x->address = TTAddress(jamoma_parse_dieze((t_object*)x, address)->s_name);
	x->address = TTAddress(address->s_name);
    
    // if the j.send tries to bind an Input object : bind the signal attribute
    if (x->address.getName() == TTSymbol("in"))
        x->address = x->address.appendAttribute(kTTSym_signal);
    
	x->argc = 0; // the argc member is usefull to count how many time the external tries to bind

#ifdef JCOM_SEND_TILDE
	// create a sender to handle an audio signal
	jamoma_sender_create_audio((t_object*)x, x->wrappedObject);
	
	// create an inlet to handle audio signal
	dsp_setup((t_pxobject *)x, 1);
	x->obj.z_misc = Z_NO_INPLACE | Z_PUT_FIRST;
#else
	// create a sender to handle any data signal
	jamoma_sender_create((t_object*)x, x->wrappedObject);
#endif
	
	x->dumpOut = outlet_new((t_object*)x,NULL);
	// handle attribute args
	attr_args_process(x, argc, argv);
    
	// for absolute address
	if (x->address.getType() == kAddressAbsolute) {
		
		x->wrappedObject.set(kTTSym_address, x->address);
		
		atom_setsym(a, gensym((char*)x->address.c_str()));
		outlet_anything((t_outlet*)x->dumpOut, gensym("address"), 1, a);
		return;
	}
    
	// The following must be deferred because we have to interrogate our box,
	// and our box is not yet valid until we have finished instantiating the object.
	// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
//	defer_low((t_object*)x, (method)send_subscribe, NULL, 0, 0);
	send_subscribe(x);
}

void WrappedSenderClass_free(TTPtr self)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    
    x->wrappedObject.set(kTTSym_address, kTTAdrsEmpty);
    
#ifdef JCOM_SEND_TILDE

	// Always call dsp_free first in this routine
    dsp_free((t_pxobject *)x);
#endif
}

#pragma mark -
#pragma mark NodeLib association

void send_subscribe(TTPtr self)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTValue						v;
	t_atom						a[1];
	TTAddress                   contextAddress = kTTAdrsEmpty;
	TTAddress                   absoluteAddress, returnedAddress;
    TTNodePtr                   returnedNode = NULL;
    TTNodePtr                   returnedContextNode = NULL;
	TTObject                    anObject, empty;
	
	if (x->address == kTTAdrsEmpty)
		return;
	
	// for relative address
	jamoma_patcher_get_info(((t_eobj*)x)->o_canvas, &x->patcherPtr, x->patcherContext, x->patcherClass, x->patcherName);
	
	if (!jamoma_subscriber_create((t_eobj*)x, empty, TTAddress("model"), x->subscriberObject, returnedAddress, &returnedNode, &returnedContextNode)) {
		
		// get the context address to make
		// a viewer on the contextAddress/model:address attribute
		x->subscriberObject.get("contextAddress", v);
		contextAddress = v[0];
        
        // release the subscriber
        x->subscriberObject = TTObject();
		
		if (x->patcherContext != kTTSymEmpty) {
            
            // observe model:address attribute (in view patcher : deferlow return_model_address)
			makeInternals_receiver(x, contextAddress, TTSymbol("/model:address"), gensym("return_model_address"), anObject, x->patcherContext == kTTSym_view);

			return;
		}
	}
	
	// else, if no context, set address directly
	else if (x->patcherContext == kTTSymEmpty) {
        
        // release the subscriber
        x->subscriberObject = TTObject();
        
		contextAddress = kTTAdrsRoot;
		absoluteAddress = contextAddress.appendAddress(x->address);
		x->wrappedObject.set(kTTSym_address, absoluteAddress);
		
		atom_setsym(a, gensym((char*)absoluteAddress.c_str()));
		outlet_anything((t_outlet*)x->dumpOut, gensym("address"), 1, a);
		return;
	}
	
	// otherwise while the context node is not registered : try to binds again :(
	// (to -- this is not a good way todo. For binding we should make a subscription 
	// to a notification mechanism and each time an TTObjet subscribes to the namespace
	// using jamoma_subscriber_create we notify all the externals which have used 
	// jamoma_subscriber_create with NULL object to bind)
		
	// release the subscriber
	x->subscriberObject = TTObject();
	
	x->argc++; // the index member is usefull to count how many time the external tries to bind
	if (x->argc > 100) {
		object_error((t_object*)x, "j.send : tries to bind too many times on %s", x->address.c_str());
		outlet_anything((t_outlet*)x->dumpOut, gensym("error"), 0, NULL);
		return;
	}
	
	// The following must be deferred because we have to interrogate our box,
	// and our box is not yet valid until we have finished instantiating the object.
	// Trying to use a loadbang method instead is also not fully successful (as of Max 5.0.6)
//	defer_low((t_object*)x, (method)send_subscribe, NULL, 0, 0);
	send_subscribe(x);
}

void send_return_model_address(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTAddress                   absoluteAddress;
	t_atom						a[1];
	
	if (argc && argv && x->wrappedObject.valid() && x->address.getType() == kAddressRelative) {
		
		// set address attribute of the wrapped Receiver object
		absoluteAddress = TTAddress(atom_getsym(argv)->s_name).appendAddress(x->address);
		x->wrappedObject.set(kTTSym_address, absoluteAddress);
		x->argc = 0; // the index member is usefull to count how many time the external tries to bind
		
		atom_setsym(a, gensym((char*)absoluteAddress.c_str()));
		outlet_anything((t_outlet*)x->dumpOut, gensym("address"), 1, a);
		
		JamomaDebug logpost((t_object*)x, 3, "binds on %s", absoluteAddress.c_str());
	}
}

#pragma mark -
#pragma mark Methods bound to input/inlets

// Method for Assistance Messages
void send_assist(TTPtr self, void *b, long msg, long arg, char *dst)
{
	if (msg==1)			// Inlets
		strcpy(dst, "");		
	else {							// Outlets
		switch(arg) {
				strcpy(dst, "dumpout");
				break;
		}
 	}
}

#ifndef JCOM_SEND_TILDE

void send_bang(TTPtr self)
{
	send_list(self, _sym_bang, 0, NULL);
}

void send_int(TTPtr self, long value)
{
	t_atom a;
	
	atom_setlong(&a, value);
	send_list(self, _sym_int, 1, &a);
}

void send_float(TTPtr self, double value)
{
	t_atom a;
	
	atom_setfloat(&a, value);
	send_list(self, _sym_float, 1, &a);
}

void send_list(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	
	// dynamic address setting for j.send without address
	if (x->address == kTTAdrsEmpty) {
			
			TTAddress anAddress = TTAddress(msg->s_name);
			t_symbol *newMsg;
			
			// send only to absolute address
			if (anAddress.getType() == kAddressAbsolute) {
				
				x->wrappedObject.set(kTTSym_address, anAddress);
				
				// edit message type
				if (argc == 0)
					newMsg = _sym_nothing;
				else if (argc > 1)
					newMsg = _sym_list;
				else if (atom_gettype(argv) == A_LONG)
					newMsg = _sym_int;
				else if (atom_gettype(argv) == A_FLOAT)
					newMsg = _sym_float;
				else if (atom_gettype(argv) == A_SYM)
					newMsg = _sym_symbol;
                else
                    return;
				
				jamoma_sender_send(x->wrappedObject, newMsg, argc, argv);
			}
	}
	else
		jamoma_sender_send(x->wrappedObject, msg, argc, argv);
}

void WrappedSenderClass_anything(TTPtr self, t_symbol *msg, long argc, t_atom *argv)
{	
	send_list(self, msg, argc, argv);
}

#endif

void send_address(TTPtr self, t_symbol *address)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
    t_atom						a[1];
	TTAddress                   newAddress = TTAddress(address->s_name); // = TTAddress(jamoma_parse_dieze((t_object*)x, address)->s_name);
    
    // if the former address was relative and the new one is absolute :
    // we don't need model:address receiver anymore
    if (x->address.getType() == kAddressRelative &&
        newAddress.getType() == kAddressAbsolute) {
        
        TTValue v;
        TTErr   err = x->internals->lookup(TTSymbol("/model:address"), v);
        
        if (!err) {
            
            TTObject aReceiver = v[0];
            aReceiver.set(kTTSym_address, kTTAdrsEmpty);
            
            x->internals->remove(TTSymbol("/model:address"));
        }
    }
    
    // assign the new address
	x->address = newAddress;
    
    // for absolute address
	if (x->address.getType() == kAddressAbsolute) {
		
		x->wrappedObject.set(kTTSym_address, x->address);
		
		atom_setsym(a, gensym((char*)x->address.c_str()));
		outlet_anything((t_outlet*)x->dumpOut, gensym("address"), 1, a);
        
		JamomaDebug logpost((t_object*)x, 3, "binds on %s", x->address.c_str());
        
		return;
	}
    
	send_subscribe(self);
}

#pragma mark -
#pragma mark Methods relating to audio processing

#ifdef JCOM_SEND_TILDE
// Perform Method - just pass the whole vector straight through
// (the work is all done in the dsp method)
t_int *send_perform(t_int *w)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)(w[1]);
	TTSenderPtr					aSender = (TTSenderPtr)x->wrappedObject.instance();
	TTListPtr					objectCache = NULL;
	TTObject                    anObject;
	TTUInt16					vectorSize = 0;
    TTUInt16					n;
	t_float*					envelope;
	TTFloat32					sum = 0, mean;
	TTValue						v, none;
    
    if (x->obj.z_disabled)
        return w + 4;
	
	if (aSender) {
		
		// get the object cache of the Sender object
		if (!aSender->getAttributeValue(kTTSym_objectCache, v)) {
			
			objectCache = TTListPtr((TTPtr)v[0]);
			
			if (objectCache) {
				
				// get signal vectorSize
				aSender->mSignal.get(kTTSym_vectorSize, vectorSize);
				
				// store the input from the inlet
				TTAudioSignalPtr(aSender->mSignal.instance())->setVector(0, vectorSize, (TTFloat32*)w[2]);
				
				// process the mean value
				envelope = (t_float *)(w[3]);
				n = vectorSize;
				while (n--) {
					sum += *envelope;
					envelope++;
				}
				mean = sum / vectorSize;
				v = TTValue(mean);
				
				// send signal or mean to each object
				for (objectCache->begin(); objectCache->end(); objectCache->next()) {
					
					anObject = objectCache->current()[0];
					
					if (anObject.valid()) {
						
						// INPUT AUDIO case : cache the signal into the input
						if (anObject.name() == kTTSym_InputAudio)
							TTInputPtr(anObject.instance())->mSignalCache.appendUnique(aSender->mSignal);
						
						// DATA case : send the mean value of the sample
						else if (anObject.name() == kTTSym_Data)
							anObject.send(kTTSym_Command, v, none);
						
					}
				}
			}
		}
	}
	
	return w + 4;
}

// Perform Method 64 bit - just pass the whole vector straight through
// (the work is all done in the dsp 64 bit method)
void send_perform64(TTPtr self, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long sampleframes, long flags, void *userparam)
{
    WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTSenderPtr					aSender = (TTSenderPtr)x->wrappedObject.instance();
	TTListPtr					objectCache = NULL;
	TTObject                    anObject;
	TTUInt16					vectorSize = 0;
    TTUInt16					n;
	TTSampleValue*              envelope;
	TTFloat32					sum = 0, mean;
	TTValue						v, none;
    
    if (x->obj.z_disabled)
        return;
	
	if (aSender) {
		
		// get the object cache of the Sender object
		if (!aSender->getAttributeValue(kTTSym_objectCache, v)) {
			
			objectCache = TTListPtr((TTPtr)v[0]);
			
			if (objectCache) {
				
				// get signal vectorSize
				aSender->mSignal.get(kTTSym_vectorSize, vectorSize);
				
				// store the input from the inlet
				TTAudioSignalPtr(aSender->mSignal.instance())->setVector64Copy(0, vectorSize, ins[0]);
				
				// process the mean value
				envelope = ins[0];
				n = vectorSize;
				while (n--) {
					sum += *envelope;
					envelope++;
				}
				mean = sum / vectorSize;
				v = TTValue(mean);
				
				// send signal or mean to each object
				for (objectCache->begin(); objectCache->end(); objectCache->next()) {
					
					anObject = objectCache->current()[0];
					
					if (anObject.valid()) {
						
						// INPUT case : cache the signal into the input
						if (anObject.name() == kTTSym_InputAudio)
							TTInputPtr(anObject.instance())->mSignalCache.appendUnique(aSender->mSignal);
						
						// DATA case : send the mean value of the sample
						else if (anObject.name() == kTTSym_Data)
							anObject.send(kTTSym_Command, v, none);
					}
				}
			}
		}
	}
}

// DSP Method
void send_dsp(TTPtr self, t_signal **sp, short *count)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTSenderPtr					aSender = (TTSenderPtr)x->wrappedObject.instance();
	void**						audioVectors = NULL;
	TTUInt16					vectorSize = 0;
	
	if (aSender) {
		
		audioVectors = (void**)sysmem_newptr(sizeof(void*) * 3);
		audioVectors[0] = x;
		
		if (count[0] || count[1]) {
			if (sp[0]->s_n > vectorSize)
				vectorSize = sp[0]->s_n;
			
			audioVectors[1] = sp[0]->s_vec;
			audioVectors[2] = sp[1]->s_vec;
		}
		
		// set signal numChannels and vectorSize
		aSender->mSignal.set(kTTSym_numChannels, 1);
		aSender->mSignal.set(kTTSym_vectorSize, vectorSize);
		
		dsp_addv(send_perform, 3, audioVectors);
		sysmem_freeptr(audioVectors);
	}
}

// DSP64 method
void send_dsp64(TTPtr self, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
	WrappedModularInstancePtr	x = (WrappedModularInstancePtr)self;
	TTSenderPtr					aSender = (TTSenderPtr)x->wrappedObject.instance();
	
	if (aSender) {
		
        // set signal numChannels and vectorSize
        aSender->mSignal.set(kTTSym_numChannels, 1);
        aSender->mSignal.set(kTTSym_vectorSize, (TTUInt16)maxvectorsize);
   
        // mSignal will be set in the perform method
        object_method(dsp64, gensym("dsp_add64"), x, send_perform64, 0, NULL);
	}
}
#endif
