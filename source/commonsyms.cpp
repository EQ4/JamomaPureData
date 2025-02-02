/* 
	symbols.c

	Copyright 2001-2002 - Cycling '74
	Joshua Kit Clayton jkc@cycling74.com
	
*/

#include "commonsyms.h"

//stringulation-station
t_symbol *_sym_nothing;
t_symbol *_sym_new;
t_symbol *_sym_free;
t_symbol *_sym_classname;
t_symbol *_sym_getname;
t_symbol *_sym_getmethod;
t_symbol *_sym_get;
t_symbol *_sym_set;
t_symbol *_sym_unset;
t_symbol *_sym_register;
t_symbol *_sym_char;
t_symbol *_sym_long;
t_symbol *_sym_float32;
t_symbol *_sym_float64;
t_symbol *_sym_symbol;
t_symbol *_sym_pointer;
t_symbol *_sym_object;
t_symbol *_sym_atom;
t_symbol *_sym_list;
t_symbol *_sym_type;
t_symbol *_sym_dim;
t_symbol *_sym_planecount;
t_symbol *_sym_val;
t_symbol *_sym_plane;
t_symbol *_sym_cell;
t_symbol *_sym_jit_matrix;
t_symbol *_sym_class_jit_matrix;
t_symbol *_sym_togworld;
t_symbol *_sym_fromgworld;
t_symbol *_sym_frommatrix;
t_symbol *_sym_class_jit_attribute;
t_symbol *_sym_jit_attribute;
t_symbol *_sym_jit_attr_offset;
t_symbol *_sym_jit_attr_offset_array;
t_symbol *_sym_rebuilding;
t_symbol *_sym_modified;
t_symbol *_sym_lock;
t_symbol *_sym_setinfo;
t_symbol *_sym_setinfo_ex;
t_symbol *_sym_getinfo;
t_symbol *_sym_data;
t_symbol *_sym_getdata;
t_symbol *_sym_outputmatrix;
t_symbol *_sym_clear;
t_symbol *_sym_err_calculate;
t_symbol *_sym_max_jit_classex;
t_symbol *_sym_setall;
t_symbol *_sym_chuck;
t_symbol *_sym_getsize;
t_symbol *_sym_getindex;
t_symbol *_sym_objptr2index;
t_symbol *_sym_append;
t_symbol *_sym_insertindex;
t_symbol *_sym_deleteindex;
t_symbol *_sym_chuckindex;
t_symbol *_sym_makearray;
t_symbol *_sym_reverse;
t_symbol *_sym_rotate;
t_symbol *_sym_shuffle;
t_symbol *_sym_swap;
t_symbol *_sym_findfirst;
t_symbol *_sym_findall;
t_symbol *_sym_methodall;
t_symbol *_sym_methodindex;
t_symbol *_sym_sort;
t_symbol *_sym_matrix_calc;
t_symbol *_sym_genframe;
t_symbol *_sym_filter;
t_symbol *_sym_jit_mop;
t_symbol *_sym_newcopy;
t_symbol *_sym_jit_linklist;
t_symbol *_sym_inputcount;
t_symbol *_sym_outputcount;
t_symbol *_sym_getinput;
t_symbol *_sym_getoutput;
t_symbol *_sym_getinputlist;
t_symbol *_sym_getoutputlist;
t_symbol *_sym_ioname;
t_symbol *_sym_matrixname;
t_symbol *_sym_outputmode;
t_symbol *_sym_matrix;
t_symbol *_sym_getmatrix;
t_symbol *_sym_typelink;
t_symbol *_sym_dimlink;
t_symbol *_sym_planelink;
t_symbol *_sym_restrict_type;
t_symbol *_sym_restrict_planecount;
t_symbol *_sym_restrict_dim;
t_symbol *_sym_special;
t_symbol *_sym_getspecial;
t_symbol *_sym_adapt;
t_symbol *_sym_decorator;
t_symbol *_sym_frommatrix_trunc;
t_symbol *_sym_ioproc;
t_symbol *_sym_getioproc;
t_symbol *_sym_name;
t_symbol *_sym_types;
t_symbol *_sym_minplanecount;
t_symbol *_sym_maxplanecount;
t_symbol *_sym_mindim;
t_symbol *_sym_maxdim;
t_symbol *_sym_gl_points;
t_symbol *_sym_gl_lines;
t_symbol *_sym_gl_line_strip;
t_symbol *_sym_gl_line_loop;
t_symbol *_sym_gl_triangles;
t_symbol *_sym_gl_tri_strip;
t_symbol *_sym_gl_tri_fan;
t_symbol *_sym_gl_quads;
t_symbol *_sym_gl_quad_strip;
t_symbol *_sym_gl_polygon;
t_symbol *_sym_gl_tri_grid;
t_symbol *_sym_gl_quad_grid;
t_symbol *_sym_err_lockout_stack;

t_symbol *_sym_class_jit_namespace;
t_symbol *_sym_jit_namespace;
t_symbol *_sym_findsize;
t_symbol *_sym_attach;
t_symbol *_sym_detach;
t_symbol *_sym_add;
t_symbol *_sym_replace;
t_symbol *_sym_gettype;
t_symbol *_sym_ob_sym;
t_symbol *_sym_resolve_name;
t_symbol *_sym_resolve_raw;
t_symbol *_sym_notifyall;

t_symbol *_sym_anything;
t_symbol *_sym_usercanset;
t_symbol *_sym_usercanget;
t_symbol *_sym_notify;
t_symbol *_sym_getvalueof;
t_symbol *_sym_setvalueof;
t_symbol *_sym_dumpout;
t_symbol *_sym_attributes;
t_symbol *_sym_adornments;
t_symbol *_sym_attr_getnames;
t_symbol *_sym_attr_get;
t_symbol *_sym_float;
t_symbol *_sym_int;
t_symbol *_sym_bang;

t_symbol *_sym_parent;
t_symbol *_sym_global;
t_symbol *_sym_arguments;
t_symbol *_sym_args;
t_symbol *_sym_attr_modified;

t_symbol *_sym_hashtab_entry_new;
t_symbol *_sym_hashtab_entry_free;
t_symbol *_sym_hashtab_free;

// some missing symbols
t_symbol *_sym_topmost;
t_symbol *_sym_patcher;
t_symbol *_sym_bpatcher;
t_symbol *_sym_subpatcher;
t_symbol *_sym_parentpatcher;
t_symbol *_sym_firstobject;
t_symbol *_sym_maxclass;
t_symbol *_sym_jpatcher;
t_symbol *_sym_filename;
t_symbol *_sym_nextobject;
t_symbol *_sym_pd;
t_symbol *_sym_box;
t_symbol *_sym_nobox;
t_symbol *_sym_read;
t_symbol *_sym_write;
t_symbol *_sym_error;
t_symbol *_sym_jed;
t_symbol *_sym_none;
t_symbol *_sym_store;

// jamoma specific symbols
t_symbol *_sym_j_in;
t_symbol *_sym_j_out;
t_symbol *_sym_j_intilda;
t_symbol *_sym_j_outtilda;
t_symbol *_sym_j_ui;
t_symbol *_sym_j_model;
t_symbol *_sym_j_view;
t_symbol *_sym_share_patcher_node;
t_symbol *_sym_share_patcher_info;

// custom symbols
t_symbol *_sym_loadbang;

//comonly used symbols that are exported
void common_symbols_init(void);

void common_symbols_init(void) 
{
	_sym_nothing		= gensym("");
	_sym_new			= gensym("new");
	_sym_free			= gensym("free");
	_sym_classname		= gensym("classname");
	_sym_getname		= gensym("getname");
	_sym_getmethod		= gensym("getmethod");
	_sym_get 			= gensym("get");
	_sym_set 			= gensym("set");
	_sym_unset			= gensym("unset");
	_sym_register		= gensym("register");
	//types
	_sym_char			= gensym("char");
	_sym_long			= gensym("long");
	_sym_float32		= gensym("float32");
	_sym_float64		= gensym("float64");
	_sym_symbol			= gensym("symbol");
	_sym_pointer		= gensym("pointer");
	_sym_object			= gensym("object");
	_sym_atom			= gensym("atom");
	//others
	_sym_list			= gensym("list");
	_sym_type			= gensym("type");
	_sym_dim			= gensym("dim");
	_sym_planecount		= gensym("planecount");
	_sym_val			= gensym("val");
	_sym_plane			= gensym("plane");
	_sym_cell			= gensym("cell");
	_sym_jit_matrix		= gensym("jit_matrix");
	_sym_class_jit_matrix	= gensym("class_jit_matrix");
	_sym_togworld		= gensym("togworld");
	_sym_fromgworld		= gensym("fromgworld");
	_sym_frommatrix		= gensym("frommatrix");
	_sym_class_jit_attribute	= gensym("class_jit_attribute");
	_sym_jit_attribute		= gensym("jit_attribute");
	_sym_jit_attr_offset	= gensym("jit_attr_offset");
	_sym_jit_attr_offset_array	= gensym("jit_attr_offset_array");
	_sym_rebuilding		= gensym("rebuilding");
	_sym_modified		= gensym("modified");
	_sym_lock			= gensym("lock");
	_sym_setinfo		= gensym("setinfo");
	_sym_setinfo_ex		= gensym("setinfo_ex");
	_sym_getinfo		= gensym("getinfo");
	_sym_data			= gensym("data");
	_sym_getdata		= gensym("getdata");
	_sym_outputmatrix	= gensym("outputmatrix");
	_sym_clear			= gensym("clear");
	_sym_err_calculate	= gensym("could not calculate");
	_sym_max_jit_classex = gensym("max_jit_classex");
	_sym_setall			= gensym("setall");
	_sym_chuck			= gensym("chuck");
	_sym_getsize		= gensym("getsize");
	_sym_getindex		= gensym("getindex");
	_sym_objptr2index	= gensym("objptr2index");
	_sym_append			= gensym("append");
	_sym_insertindex	= gensym("insert");
	_sym_deleteindex	= gensym("delete");
	_sym_chuckindex		= gensym("chuckindex");
	_sym_makearray		= gensym("makearray");
	_sym_reverse		= gensym("reverse");
	_sym_rotate			= gensym("rotate");
	_sym_shuffle		= gensym("shuffle");
	_sym_swap			= gensym("swap");
	_sym_findfirst		= gensym("findfirst");
	_sym_findall		= gensym("findall");
	_sym_methodall		= gensym("methodall");
	_sym_methodindex	= gensym("methodindex");
	_sym_sort			= gensym("sort");
	_sym_matrix_calc	= gensym("matrix_calc");
	_sym_genframe		= gensym("genframe");
	_sym_filter			= gensym("filter");
	_sym_jit_mop		= gensym("jit_mop");
	_sym_newcopy		= gensym("newcopy");
	_sym_jit_linklist	= gensym("jit_linklist");
	_sym_inputcount		= gensym("inputcount");
	_sym_outputcount	= gensym("outputcount");
	_sym_getinput		= gensym("getinput");
	_sym_getoutput		= gensym("getoutput");
	_sym_getinputlist	= gensym("getinputlist");
	_sym_getoutputlist	= gensym("getoutputlist");
	_sym_ioname			= gensym("ioname");
	_sym_matrixname		= gensym("matrixname");
	_sym_outputmode		= gensym("outputmode");
	_sym_matrix			= gensym("matrix");
	_sym_getmatrix		= gensym("getmatrix");
	_sym_typelink		= gensym("typelink");
	_sym_dimlink		= gensym("dimlink");
	_sym_planelink		= gensym("planelink");
	_sym_restrict_type	= gensym("restrict_type");
	_sym_restrict_planecount	= gensym("restrict_planecount");
	_sym_restrict_dim	= gensym("restrict_dim");
	_sym_special		= gensym("special");
	_sym_getspecial		= gensym("getspecial");
	_sym_adapt			= gensym("adapt");
	_sym_decorator		= gensym("decorator");
	_sym_frommatrix_trunc	= gensym("frommatrix_trunc");
	_sym_ioproc			= gensym("ioproc");
	_sym_getioproc		= gensym("getioproc");
	_sym_name			= gensym("name");
	_sym_types			= gensym("types");
	_sym_minplanecount	= gensym("minplanecount");
	_sym_maxplanecount	= gensym("maxplanecount");
	_sym_mindim			= gensym("mindim");
	_sym_maxdim			= gensym("maxdim");
	_sym_err_lockout_stack 	= gensym("lockout stack underflow or overflow");
	// gl
	_sym_gl_points 		= gensym("points");
	_sym_gl_lines 		= gensym("lines");
	_sym_gl_line_strip 	= gensym("line_strip");
	_sym_gl_line_loop 	= gensym("line_loop");
	_sym_gl_triangles 	= gensym("triangles");
	_sym_gl_tri_strip 	= gensym("tri_strip");
	_sym_gl_tri_fan 	= gensym("tri_fan");
	_sym_gl_quads 		= gensym("quads");
	_sym_gl_quad_strip 	= gensym("quad_strip");
	_sym_gl_polygon 	= gensym("polygon");
	_sym_gl_tri_grid 	= gensym("tri_grid");
	_sym_gl_quad_grid 	= gensym("quad_grid");
	// namespace
	_sym_class_jit_namespace	= gensym("class_jit_namespace");
	_sym_jit_namespace	= gensym("jit_namespace");
	_sym_findsize		= gensym("findsize");
	_sym_attach			= gensym("attach");
	_sym_detach			= gensym("detach");
	_sym_add			= gensym("add");
	_sym_replace		= gensym("replace");
	_sym_gettype		= gensym("gettype");
	_sym_ob_sym			= gensym("ob_sym");
	_sym_resolve_name	= gensym("resolve_name");
	_sym_resolve_raw	= gensym("resolve_raw");
	_sym_notifyall		= gensym("notifyall");	
	
	_sym_anything		= gensym("anything");
	_sym_usercanset		= gensym("usercanset");
	_sym_usercanget		= gensym("usercanget");
	_sym_notify			= gensym("notify");
	_sym_getvalueof		= gensym("getvalueof");
	_sym_setvalueof		= gensym("setvalueof");
	_sym_dumpout		= gensym("dumpout");
	_sym_attributes		= gensym("attributes");
	_sym_adornments		= gensym("adornments");
	_sym_attr_getnames	= gensym("attr_getnames");
	_sym_attr_get		= gensym("attr_get");
	_sym_float			= gensym("float");
	_sym_int			= gensym("int");
	_sym_bang			= gensym("bang");

	_sym_parent			= gensym("parent");
	_sym_global			= gensym("global");
	_sym_arguments		= gensym("arguments");
	_sym_args			= gensym("args");
	_sym_attr_modified	= gensym("attr_modified");

	_sym_hashtab_entry_new 	= gensym("hashtab_entry_new");
	_sym_hashtab_entry_free = gensym("hashtab_entry_free");
	_sym_hashtab_free 		= gensym("hashtab_free");

    // some missing symbols
    _sym_topmost        = gensym("topmost");
    _sym_patcher        = gensym("patcher");
    _sym_bpatcher       = gensym("bpatcher");
    _sym_subpatcher     = gensym("subpathcer");
    _sym_parentpatcher  = gensym("parentpatcher");
    _sym_firstobject    = gensym("firstobject");
    _sym_maxclass       = gensym("maxclass");
    _sym_jpatcher       = gensym("jpatcher");
    _sym_filename       = gensym("filename");
    _sym_nextobject     = gensym("nextobject");
    _sym_pd             = gensym("pd");
    _sym_box            = gensym("box");
	_sym_nobox          = gensym("nobox");
	_sym_read			= gensym("read");
	_sym_write			= gensym("write");
	_sym_error			= gensym("error");
	_sym_jed			= gensym("jed");
    _sym_none           = gensym("none");
    _sym_store          = gensym("store");


	// jamoma specific symbols
	_sym_j_in = gensym("j.in");
	_sym_j_out = gensym("j.out");
	_sym_j_intilda = gensym("j.in~");
	_sym_j_outtilda = gensym("j.out~");
	_sym_j_ui = gensym("j.ui");
	_sym_j_model = gensym("j.model");
	_sym_j_view = gensym("j.view");
	_sym_share_patcher_node = gensym("share_patcher_node");
	_sym_share_patcher_info = gensym("share_patcher_info");

	// custom symbols
	_sym_loadbang = gensym("loadbang");

}
