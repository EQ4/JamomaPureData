/** @file
 *
 * @ingroup implementationPdExternals
 *
 * @brief j.ui : Provide standard user interface component for modules
 *
 * @details
 *
 * @authors Tim Place, Trond Lossius, Antoine Villeret
 *
 * @copyright © 2007 by Tim Place @n
 * Copyright © 2015, Antoine Villeret@n
 * This code is licensed under the terms of the "New BSD License" @n
 * http://creativecommons.org/licenses/BSD/
 */


#ifndef __J_UI__
#define __J_UI__

/*
#include "ext.h"
#include "ext_obex.h"
#include "ext_user.h"
#include "ext_common.h"
#include "jpatcher_api.h"			// jpatcher_api.h must come before z_dsp.h (in Jamoma.h)
#include "jgraphics.h"
*/

#include "TTModular.h"				// Jamoma Modular API
#include "JamomaForPd.h"			// Jamoma for Max
//#include "ext_symobject.h"

#define NO_MODEL_STRING "waiting for a model:address"

#define panel_out 0

// those stuffes are needed for handling patchers without using the pcontrol object
// #include "jpatcher_api.h"
/*
typedef struct dll {
	t_object d_ob;
	struct dll *d_next;
	struct dll *d_prev;
	void *d_x1;
} t_dll;

typedef struct outlet {
	struct tinyobject o_ob;
	struct dll *o_dll;
} t_outlet;

typedef struct inlet {
	struct tinyobject i_ob;
	void *i_who;
	struct object *i_owner;
} t_inlet;
*/

// members
typedef struct _ui{
    t_ebox				box;
	TTHandle			outlets;
    TTObject            uiInfo;
    TTObject            uiSubscriber;			///< internal TTSubscriber object to bind on the ui node
	TTHashPtr			hash_datas;				///< hash table of TTData
	TTHashPtr			hash_viewers;			///< hash table of TTViewer
	TTHashPtr			hash_receivers;			///< hash table of TTReceiver
	TTObject            modelMessExplorer;		///< internal TTExplorer object to observe messages
	TTObject            modelParamExplorer;		///< internal TTExplorer object to observe parameters
	TTObject            modelRetExplorer;		///< internal TTExplorer object to observe returns
	
	TTAddress           viewAddress;
	TTAddress           modelAddress;
	t_object            *patcherPtr;			///< the patcher in which the external is (ignoring subpatcher)
	TTSymbol			patcherContext;			///< the patcher context in which the external is (model, view)
	TTSymbol			patcherClass;			///< the patcher class in which the external is
	TTSymbol			patcherName;
	
	TTBoolean			hover;					// is the mouse hover the j.ui panel ?
	TTBoolean			highlight;				// is the user selecting things ?
	TTBoolean			highlightAll;			// to highlight or not all j.remotes

    /*
	t_jrgba				bgcolor;
	t_jrgba				bordercolor;
	t_jrgba				headercolor;
	t_jrgba				textcolor;
    t_jrgba             highlightcolor;
    */

    TTString            *text;                  // the text of the editor to read after edclose
	t_object            *textEditor;             // the text editor window
    TTObject            textHandler;            ///< internal TTTextHandler to fill the max text editor
    TTObject            state;                  ///< internal TTPreset to get the current state of the binded model
	
	long				ui_freeze;				// freeze all viewers of the view
	
	t_jpopupmenu		*menu;					// model menu
	void				*menu_qelem;			// ...
	long				menu_selection;			// ...
	t_linklist			*menu_items;			// ...
	t_atom				*preset_names;
	long                preset_num;

	t_jpopupmenu		*refmenu;				// reference menu
	void				*refmenu_qelem;			// ...
	long				refmenu_selection;		// ...
	t_linklist			*refmenu_items;			// ...

	long				has_preset;				// is the binded model have preset features ?
	long				has_model;				// is the binded model have model features ?
	
	long				has_panel;				// is the binded model have a panel ?
	t_rect				rect_panel;
	t_object			*patcher_panel;

	long				has_meters;				// is the binded model have meters ? (set number of meters, not just a toggle)
	long				is_metersdefeated;
	t_rect				rect_meters;

	long				has_mute;				// is the binded model have a mute ?
	long				is_muted;
	bool				highlight_mute;			// selection state of mute
	t_rect				rect_mute;

	long				has_bypass;				// is the binded model have a bypass ?
	long				is_bypassed;
	bool				highlight_bypass;		// selection state of bypass
	t_rect				rect_bypass;

	long				has_freeze;				// is the binded model have a freeze ?
	long				is_frozen;
	bool				highlight_freeze;		// selection state of freeze
	t_rect				rect_freeze;

	long				has_active;			// is the binded model have a active ?
	long				is_active;
	bool				highlight_active;		// selection state of active
	t_rect				rect_active;

	long				has_gain;				// is the binded model have a gain ?
	float				gain;
	bool				highlight_gain;			// selection state of gain
	t_rect				rect_gain;
	bool				gainDragging;
	
	long				has_mix;				// is the binded model have a mix ?
	float				mix;
	bool				highlight_mix;			// selection state of mix
	t_rect				rect_mix;
	bool				mixDragging;
	
	t_pt				anchor;				// used for dragging the dials
	float				anchorValue;		//	...
} t_ui;

// prototypes: general
t_ui		*ui_new(t_symbol *s, long argc, t_atom *argv);
void 		ui_free(t_ui *x);
t_pd_err	ui_notify(t_ui *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
void		ui_subscribe(t_ui *x, t_symbol *address);
void		ui_build(t_ui *x);
t_object    *ui_get_model_object(t_ui *x);
void 		ui_bang(t_ui *x);

// prototypes: drawing/ui
void 		ui_paint(t_ui *x, t_object *view);
void 		ui_mousedown(t_ui *x, t_object *patcherview, t_pt pt, long modifiers);
void		ui_mousedragdelta(t_ui *x, t_object *patcherview, t_pt pt, long modifiers);
void		ui_mouseup(t_ui *x, t_object *patcherview);
void 		ui_mousemove(t_ui *x, t_object *patcherview, t_pt pt, long modifiers);
void 		ui_mouseleave(t_ui *x, t_object *patcherview, t_pt pt, long modifiers);
void		*ui_oksize(t_ui *x, t_rect *rect);
void		ui_preset_interface(t_ui *x);
void 		ui_paint_address(t_ui *x, t_object *textfield);

// prototypes: menus
void		ui_menu_do(t_ui *x, t_object *patcherview, t_pt px, long modifiers);
void 		ui_menu_qfn(t_ui *x);
void 		ui_menu_build(t_ui *x);
void		ui_refmenu_do(t_ui *x, t_object *patcherview, t_pt px, long modifiers);
void 		ui_refmenu_qfn(t_ui *x);
void 		ui_refmenu_build(t_ui *x);

// prototypes: TTUiInfo registration
void		ui_register_info(t_ui* obj);
void		ui_unregister_info(t_ui* obj);

// prototypes: internal TTViewer
void		ui_data_interface(t_ui *x, TTSymbol name);

void		ui_receiver_create(t_ui *obj, TTObject& returnedReceiver, t_symbol *aCallbackMethod, TTSymbol name, TTAddress address, TTBoolean deferlow = NO, TTBoolean appendNameAsAttribute = NO);
void		ui_receiver_destroy_all(t_ui *obj);

void		ui_viewer_create(t_ui *obj, TTObject& returnedViewer, t_symbol *aCallbackMethod, TTSymbol name, TTAddress address, TTBoolean subscribe, TTBoolean deferlow = NO);
void		ui_viewer_destroy(t_ui *obj, TTSymbol name);
void		ui_viewer_destroy_all(t_ui *obj);
void		ui_viewer_send(t_ui *obj, TTSymbol name, TTValue v);
void		ui_viewer_highlight(t_ui *obj, TTSymbol name, TTBoolean s);
void		ui_viewer_freeze(t_ui *obj, TTSymbol name, TTBoolean f);

void		ui_explorer_create(t_object *x, TTObject& returnedExplorer, t_symbol *method);
void		ui_modelMessExplorer_callback(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_modelParamExplorer_callback(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_modelRetExplorer_callback(TTPtr self, t_symbol *msg, long argc, t_atom* argv);

void		ui_view_panel_attach(TTPtr self, t_symbol *msg, long argc, t_atom *argv);

void		ui_return_model_address(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_model_init(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_model_content(TTPtr self, t_symbol *msg, long argc, t_atom* argv);

void		ui_return_mute(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_bypass(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_mix(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_gain(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_freeze(TTPtr self, t_symbol *msg, long argc, t_atom* argv);
void		ui_return_active(TTPtr self, t_symbol *msg, long argc, t_atom* argv);

// prototype : text editor
void		ui_edit_state(t_ui *x);
void		ui_edclose(t_ui *x, char **text, long size);
void		ui_doedit(t_ui *x);

// prototype: ui handling for preset features
void		ui_return_preset_names(TTPtr self, t_symbol *msg, long argc, t_atom* argv);

#endif // __J_UI__
