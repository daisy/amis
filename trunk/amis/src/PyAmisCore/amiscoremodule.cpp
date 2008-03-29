
/* ======================== Module amiscore ========================= */

#include "Python.h"



#include "Error.h"
#include "Media.h"
#include "ModuleDescData.h"
#include "RecentBooks.h"
#include "dtb/Bookmarks.h"
#include "dtb/CustomTest.h"
#include "dtb/Dtb.h"
#include "dtb/DtbFileSet.h"
#include "dtb/Metadata.h"
#include "dtb/Spine.h"
#include "dtb/nav/NavContainer.h"
#include "dtb/nav/NavList.h"
#include "dtb/nav/NavMap.h"
#include "dtb/nav/NavModel.h"
#include "dtb/nav/NavNode.h"
#include "dtb/nav/NavPoint.h"
#include "dtb/nav/NavTarget.h"
#include "dtb/nav/PageList.h"
#include "dtb/nav/PageTarget.h"
#include "dtb/smil/SmilMediaGroup.h"
#include "dtb/smil/SmilTree.h"
#include "io/ModuleDescReader.h"

/* Workaround for "const" added in Python 2.5. But removed before 2.5a1? */
#if PY_VERSION_HEX >= 0x02050000 && PY_VERSION_HEX < 0x020500a1
# define Py_KEYWORDS_STRING_TYPE const char
#else
# define Py_KEYWORDS_STRING_TYPE char
#endif

#include "amiscoremodule.h"

static PyObject *
bool_New(bool itself)
{
    if (itself) {
        Py_RETURN_TRUE;
    } 
    Py_RETURN_FALSE;
}

static int
bool_Convert(PyObject *v, bool *p_itself)
{
    int istrue = PyObject_IsTrue(v);
    if (istrue < 0) return 0;
    *p_itself = (istrue > 0);
    return 1;
}

PyObject *
ambulant_url_New(const ambulant::net::url& itself)
{
    return PyString_FromString(itself.get_url().c_str());
}

int
ambulant_url_Convert(PyObject *v, ambulant::net::url *p_itself)
{
    char *cstr = PyString_AsString(v);
    if (cstr == NULL) return 0;
    std::string cxxstr = cstr;
    ambulant::net::url url = ambulant::net::url::from_url(cxxstr);
    *p_itself = url;
    return 1;
}

static PyObject *PyAm_Error;

/* ----------------------- Object type Error ------------------------ */

extern PyTypeObject Error_Type;

inline bool ErrorObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Error_Type);
}

typedef struct ErrorObject {
	PyObject_HEAD
	amis::Error* ob_itself;
} ErrorObject;

PyObject *ErrorObj_New(amis::Error* itself)
{
	ErrorObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Error
	Error *encaps_itself = dynamic_cast<Error *>(itself);
	if (encaps_itself && encaps_itself->py_Error)
	{
		Py_INCREF(encaps_itself->py_Error);
		return encaps_itself->py_Error;
	}
#endif
	it = PyObject_NEW(ErrorObject, &Error_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int ErrorObj_Convert(PyObject *v, amis::Error* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Error
	if (!ErrorObj_Check(v))
	{
		*p_itself = Py_WrapAs_Error(v);
		if (*p_itself) return 1;
	}
#endif
	if (!ErrorObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Error required");
		return 0;
	}
	*p_itself = ((ErrorObject *)v)->ob_itself;
	return 1;
}

static void ErrorObj_dealloc(ErrorObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *ErrorObj_setCode(ErrorObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::ErrorCode _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setCode(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ErrorObj_setMessage(ErrorObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setMessage(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ErrorObj_getCode(ErrorObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::ErrorCode _rv = _self->ob_itself->getCode();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *ErrorObj_getMessage(ErrorObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getMessage();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *ErrorObj_clear(ErrorObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->clear();
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef ErrorObj_methods[] = {
	{"setCode", (PyCFunction)ErrorObj_setCode, 1,
	 PyDoc_STR("(amis::ErrorCode _arg1) -> None")},
	{"setMessage", (PyCFunction)ErrorObj_setMessage, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getCode", (PyCFunction)ErrorObj_getCode, 1,
	 PyDoc_STR("() -> (amis::ErrorCode _rv)")},
	{"getMessage", (PyCFunction)ErrorObj_getMessage, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"clear", (PyCFunction)ErrorObj_clear, 1,
	 PyDoc_STR("() -> None")},
	{NULL, NULL, 0}
};

#define ErrorObj_getsetlist NULL


static int ErrorObj_compare(ErrorObject *self, ErrorObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define ErrorObj_repr NULL

static int ErrorObj_hash(ErrorObject *self)
{
	return (int)self->ob_itself;
}
static int ErrorObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::Error* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((ErrorObject *)_self)->ob_itself = new amis::Error();
			return 0;
		}
	}

	{
		amis::ErrorCode _arg1;
		std::string _arg2;
		char *_arg2_cstr;
		if (PyArg_ParseTuple(_args, "ls",
		                     &_arg1,
		                     &_arg2_cstr))
		{
			_arg2 = _arg2_cstr;
			((ErrorObject *)_self)->ob_itself = new amis::Error(_arg1,
			                                                    _arg2);
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, ErrorObj_Convert, &itself))
	{
		((ErrorObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define ErrorObj_tp_alloc PyType_GenericAlloc

static PyObject *ErrorObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((ErrorObject *)_self)->ob_itself = NULL;
	return _self;
}

#define ErrorObj_tp_free PyObject_Del


PyTypeObject Error_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Error", /*tp_name*/
	sizeof(ErrorObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) ErrorObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) ErrorObj_compare, /*tp_compare*/
	(reprfunc) ErrorObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) ErrorObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	ErrorObj_methods, /* tp_methods */
	0, /*tp_members*/
	ErrorObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	ErrorObj_tp_init, /* tp_init */
	ErrorObj_tp_alloc, /* tp_alloc */
	ErrorObj_tp_new, /* tp_new */
	ErrorObj_tp_free, /* tp_free */
};

/* --------------------- End object type Error ---------------------- */


/* --------------------- Object type MediaNode ---------------------- */

extern PyTypeObject MediaNode_Type;

inline bool MediaNodeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &MediaNode_Type);
}

typedef struct MediaNodeObject {
	PyObject_HEAD
	amis::MediaNode* ob_itself;
} MediaNodeObject;

PyObject *MediaNodeObj_New(amis::MediaNode* itself)
{
	MediaNodeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_MediaNode
	MediaNode *encaps_itself = dynamic_cast<MediaNode *>(itself);
	if (encaps_itself && encaps_itself->py_MediaNode)
	{
		Py_INCREF(encaps_itself->py_MediaNode);
		return encaps_itself->py_MediaNode;
	}
#endif
	it = PyObject_NEW(MediaNodeObject, &MediaNode_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int MediaNodeObj_Convert(PyObject *v, amis::MediaNode* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_MediaNode
	if (!MediaNodeObj_Check(v))
	{
		*p_itself = Py_WrapAs_MediaNode(v);
		if (*p_itself) return 1;
	}
#endif
	if (!MediaNodeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "MediaNode required");
		return 0;
	}
	*p_itself = ((MediaNodeObject *)v)->ob_itself;
	return 1;
}

static void MediaNodeObj_dealloc(MediaNodeObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *MediaNodeObj_clone(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaNode* _rv = _self->ob_itself->clone();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaNodeObj_New, _rv);
	return _res;
}

static PyObject *MediaNodeObj_getId(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *MediaNodeObj_setId(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaNodeObj_getSrc(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getSrc();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *MediaNodeObj_setSrc(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setSrc(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaNodeObj_getHref(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getHref();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *MediaNodeObj_setHref(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setHref(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaNodeObj_getLangCode(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getLangCode();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *MediaNodeObj_setLangCode(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLangCode(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaNodeObj_getMediaNodeType(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaNodeType _rv = _self->ob_itself->getMediaNodeType();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *MediaNodeObj_setMediaNodeType(MediaNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaNodeType _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setMediaNodeType(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef MediaNodeObj_methods[] = {
	{"clone", (PyCFunction)MediaNodeObj_clone, 1,
	 PyDoc_STR("() -> (amis::MediaNode* _rv)")},
	{"getId", (PyCFunction)MediaNodeObj_getId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setId", (PyCFunction)MediaNodeObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getSrc", (PyCFunction)MediaNodeObj_getSrc, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setSrc", (PyCFunction)MediaNodeObj_setSrc, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getHref", (PyCFunction)MediaNodeObj_getHref, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setHref", (PyCFunction)MediaNodeObj_setHref, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getLangCode", (PyCFunction)MediaNodeObj_getLangCode, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setLangCode", (PyCFunction)MediaNodeObj_setLangCode, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getMediaNodeType", (PyCFunction)MediaNodeObj_getMediaNodeType, 1,
	 PyDoc_STR("() -> (amis::MediaNodeType _rv)")},
	{"setMediaNodeType", (PyCFunction)MediaNodeObj_setMediaNodeType, 1,
	 PyDoc_STR("(amis::MediaNodeType _arg1) -> None")},
	{NULL, NULL, 0}
};

#define MediaNodeObj_getsetlist NULL


static int MediaNodeObj_compare(MediaNodeObject *self, MediaNodeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define MediaNodeObj_repr NULL

static int MediaNodeObj_hash(MediaNodeObject *self)
{
	return (int)self->ob_itself;
}
static int MediaNodeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::MediaNode* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, MediaNodeObj_Convert, &itself))
	{
		((MediaNodeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define MediaNodeObj_tp_alloc PyType_GenericAlloc

static PyObject *MediaNodeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((MediaNodeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define MediaNodeObj_tp_free PyObject_Del


PyTypeObject MediaNode_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.MediaNode", /*tp_name*/
	sizeof(MediaNodeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) MediaNodeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) MediaNodeObj_compare, /*tp_compare*/
	(reprfunc) MediaNodeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) MediaNodeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	MediaNodeObj_methods, /* tp_methods */
	0, /*tp_members*/
	MediaNodeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	MediaNodeObj_tp_init, /* tp_init */
	MediaNodeObj_tp_alloc, /* tp_alloc */
	MediaNodeObj_tp_new, /* tp_new */
	MediaNodeObj_tp_free, /* tp_free */
};

/* ------------------- End object type MediaNode -------------------- */


/* ---------------------- Object type TextNode ---------------------- */

extern PyTypeObject TextNode_Type;

inline bool TextNodeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &TextNode_Type);
}

typedef struct TextNodeObject {
	PyObject_HEAD
	amis::TextNode* ob_itself;
} TextNodeObject;

PyObject *TextNodeObj_New(amis::TextNode* itself)
{
	TextNodeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_TextNode
	TextNode *encaps_itself = dynamic_cast<TextNode *>(itself);
	if (encaps_itself && encaps_itself->py_TextNode)
	{
		Py_INCREF(encaps_itself->py_TextNode);
		return encaps_itself->py_TextNode;
	}
#endif
	it = PyObject_NEW(TextNodeObject, &TextNode_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int TextNodeObj_Convert(PyObject *v, amis::TextNode* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_TextNode
	if (!TextNodeObj_Check(v))
	{
		*p_itself = Py_WrapAs_TextNode(v);
		if (*p_itself) return 1;
	}
#endif
	if (!TextNodeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "TextNode required");
		return 0;
	}
	*p_itself = ((TextNodeObject *)v)->ob_itself;
	return 1;
}

static void TextNodeObj_dealloc(TextNodeObject *self)
{
	MediaNode_Type.tp_dealloc((PyObject *)self);
}

static PyObject *TextNodeObj_getTextString(TextNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::wstring _rv = _self->ob_itself->getTextString();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("u",
	                     _rv.c_str());
	return _res;
}

static PyObject *TextNodeObj_setTextString(TextNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::wstring _arg1;
	wchar_t *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "u",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setTextString(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *TextNodeObj_getLangDir(TextNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::TextDirection _rv = _self->ob_itself->getLangDir();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *TextNodeObj_setLangDir(TextNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::TextDirection _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLangDir(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *TextNodeObj_clone(TextNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaNode* _rv = _self->ob_itself->clone();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaNodeObj_New, _rv);
	return _res;
}

static PyMethodDef TextNodeObj_methods[] = {
	{"getTextString", (PyCFunction)TextNodeObj_getTextString, 1,
	 PyDoc_STR("() -> (std::wstring _rv)")},
	{"setTextString", (PyCFunction)TextNodeObj_setTextString, 1,
	 PyDoc_STR("(std::wstring _arg1) -> None")},
	{"getLangDir", (PyCFunction)TextNodeObj_getLangDir, 1,
	 PyDoc_STR("() -> (amis::TextDirection _rv)")},
	{"setLangDir", (PyCFunction)TextNodeObj_setLangDir, 1,
	 PyDoc_STR("(amis::TextDirection _arg1) -> None")},
	{"clone", (PyCFunction)TextNodeObj_clone, 1,
	 PyDoc_STR("() -> (amis::MediaNode* _rv)")},
	{NULL, NULL, 0}
};

#define TextNodeObj_getsetlist NULL


static int TextNodeObj_compare(TextNodeObject *self, TextNodeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define TextNodeObj_repr NULL

static int TextNodeObj_hash(TextNodeObject *self)
{
	return (int)self->ob_itself;
}
static int TextNodeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::TextNode* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((TextNodeObject *)_self)->ob_itself = new amis::TextNode();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, TextNodeObj_Convert, &itself))
	{
		((TextNodeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define TextNodeObj_tp_alloc PyType_GenericAlloc

static PyObject *TextNodeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((TextNodeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define TextNodeObj_tp_free PyObject_Del


PyTypeObject TextNode_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.TextNode", /*tp_name*/
	sizeof(TextNodeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) TextNodeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) TextNodeObj_compare, /*tp_compare*/
	(reprfunc) TextNodeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) TextNodeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	TextNodeObj_methods, /* tp_methods */
	0, /*tp_members*/
	TextNodeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	TextNodeObj_tp_init, /* tp_init */
	TextNodeObj_tp_alloc, /* tp_alloc */
	TextNodeObj_tp_new, /* tp_new */
	TextNodeObj_tp_free, /* tp_free */
};

/* -------------------- End object type TextNode -------------------- */


/* --------------------- Object type AudioNode ---------------------- */

extern PyTypeObject AudioNode_Type;

inline bool AudioNodeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &AudioNode_Type);
}

typedef struct AudioNodeObject {
	PyObject_HEAD
	amis::AudioNode* ob_itself;
} AudioNodeObject;

PyObject *AudioNodeObj_New(amis::AudioNode* itself)
{
	AudioNodeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_AudioNode
	AudioNode *encaps_itself = dynamic_cast<AudioNode *>(itself);
	if (encaps_itself && encaps_itself->py_AudioNode)
	{
		Py_INCREF(encaps_itself->py_AudioNode);
		return encaps_itself->py_AudioNode;
	}
#endif
	it = PyObject_NEW(AudioNodeObject, &AudioNode_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int AudioNodeObj_Convert(PyObject *v, amis::AudioNode* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_AudioNode
	if (!AudioNodeObj_Check(v))
	{
		*p_itself = Py_WrapAs_AudioNode(v);
		if (*p_itself) return 1;
	}
#endif
	if (!AudioNodeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "AudioNode required");
		return 0;
	}
	*p_itself = ((AudioNodeObject *)v)->ob_itself;
	return 1;
}

static void AudioNodeObj_dealloc(AudioNodeObject *self)
{
	MediaNode_Type.tp_dealloc((PyObject *)self);
}

static PyObject *AudioNodeObj_clone(AudioNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaNode* _rv = _self->ob_itself->clone();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaNodeObj_New, _rv);
	return _res;
}

static PyObject *AudioNodeObj_getClipBegin(AudioNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getClipBegin();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *AudioNodeObj_setClipBegin(AudioNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setClipBegin(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *AudioNodeObj_getClipEnd(AudioNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getClipEnd();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *AudioNodeObj_setClipEnd(AudioNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setClipEnd(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef AudioNodeObj_methods[] = {
	{"clone", (PyCFunction)AudioNodeObj_clone, 1,
	 PyDoc_STR("() -> (amis::MediaNode* _rv)")},
	{"getClipBegin", (PyCFunction)AudioNodeObj_getClipBegin, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setClipBegin", (PyCFunction)AudioNodeObj_setClipBegin, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getClipEnd", (PyCFunction)AudioNodeObj_getClipEnd, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setClipEnd", (PyCFunction)AudioNodeObj_setClipEnd, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{NULL, NULL, 0}
};

#define AudioNodeObj_getsetlist NULL


static int AudioNodeObj_compare(AudioNodeObject *self, AudioNodeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define AudioNodeObj_repr NULL

static int AudioNodeObj_hash(AudioNodeObject *self)
{
	return (int)self->ob_itself;
}
static int AudioNodeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::AudioNode* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((AudioNodeObject *)_self)->ob_itself = new amis::AudioNode();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, AudioNodeObj_Convert, &itself))
	{
		((AudioNodeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define AudioNodeObj_tp_alloc PyType_GenericAlloc

static PyObject *AudioNodeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((AudioNodeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define AudioNodeObj_tp_free PyObject_Del


PyTypeObject AudioNode_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.AudioNode", /*tp_name*/
	sizeof(AudioNodeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) AudioNodeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) AudioNodeObj_compare, /*tp_compare*/
	(reprfunc) AudioNodeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) AudioNodeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	AudioNodeObj_methods, /* tp_methods */
	0, /*tp_members*/
	AudioNodeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	AudioNodeObj_tp_init, /* tp_init */
	AudioNodeObj_tp_alloc, /* tp_alloc */
	AudioNodeObj_tp_new, /* tp_new */
	AudioNodeObj_tp_free, /* tp_free */
};

/* ------------------- End object type AudioNode -------------------- */


/* --------------------- Object type ImageNode ---------------------- */

extern PyTypeObject ImageNode_Type;

inline bool ImageNodeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &ImageNode_Type);
}

typedef struct ImageNodeObject {
	PyObject_HEAD
	amis::ImageNode* ob_itself;
} ImageNodeObject;

PyObject *ImageNodeObj_New(amis::ImageNode* itself)
{
	ImageNodeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_ImageNode
	ImageNode *encaps_itself = dynamic_cast<ImageNode *>(itself);
	if (encaps_itself && encaps_itself->py_ImageNode)
	{
		Py_INCREF(encaps_itself->py_ImageNode);
		return encaps_itself->py_ImageNode;
	}
#endif
	it = PyObject_NEW(ImageNodeObject, &ImageNode_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int ImageNodeObj_Convert(PyObject *v, amis::ImageNode* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_ImageNode
	if (!ImageNodeObj_Check(v))
	{
		*p_itself = Py_WrapAs_ImageNode(v);
		if (*p_itself) return 1;
	}
#endif
	if (!ImageNodeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "ImageNode required");
		return 0;
	}
	*p_itself = ((ImageNodeObject *)v)->ob_itself;
	return 1;
}

static void ImageNodeObj_dealloc(ImageNodeObject *self)
{
	MediaNode_Type.tp_dealloc((PyObject *)self);
}

static PyObject *ImageNodeObj_clone(ImageNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaNode* _rv = _self->ob_itself->clone();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaNodeObj_New, _rv);
	return _res;
}

static PyMethodDef ImageNodeObj_methods[] = {
	{"clone", (PyCFunction)ImageNodeObj_clone, 1,
	 PyDoc_STR("() -> (amis::MediaNode* _rv)")},
	{NULL, NULL, 0}
};

#define ImageNodeObj_getsetlist NULL


static int ImageNodeObj_compare(ImageNodeObject *self, ImageNodeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define ImageNodeObj_repr NULL

static int ImageNodeObj_hash(ImageNodeObject *self)
{
	return (int)self->ob_itself;
}
static int ImageNodeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::ImageNode* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((ImageNodeObject *)_self)->ob_itself = new amis::ImageNode();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, ImageNodeObj_Convert, &itself))
	{
		((ImageNodeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define ImageNodeObj_tp_alloc PyType_GenericAlloc

static PyObject *ImageNodeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((ImageNodeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define ImageNodeObj_tp_free PyObject_Del


PyTypeObject ImageNode_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.ImageNode", /*tp_name*/
	sizeof(ImageNodeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) ImageNodeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) ImageNodeObj_compare, /*tp_compare*/
	(reprfunc) ImageNodeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) ImageNodeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	ImageNodeObj_methods, /* tp_methods */
	0, /*tp_members*/
	ImageNodeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	ImageNodeObj_tp_init, /* tp_init */
	ImageNodeObj_tp_alloc, /* tp_alloc */
	ImageNodeObj_tp_new, /* tp_new */
	ImageNodeObj_tp_free, /* tp_free */
};

/* ------------------- End object type ImageNode -------------------- */


/* --------------------- Object type MediaGroup --------------------- */

extern PyTypeObject MediaGroup_Type;

inline bool MediaGroupObj_Check(PyObject *x)
{
	return ((x)->ob_type == &MediaGroup_Type);
}

typedef struct MediaGroupObject {
	PyObject_HEAD
	amis::MediaGroup* ob_itself;
} MediaGroupObject;

PyObject *MediaGroupObj_New(amis::MediaGroup* itself)
{
	MediaGroupObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_MediaGroup
	MediaGroup *encaps_itself = dynamic_cast<MediaGroup *>(itself);
	if (encaps_itself && encaps_itself->py_MediaGroup)
	{
		Py_INCREF(encaps_itself->py_MediaGroup);
		return encaps_itself->py_MediaGroup;
	}
#endif
	it = PyObject_NEW(MediaGroupObject, &MediaGroup_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int MediaGroupObj_Convert(PyObject *v, amis::MediaGroup* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_MediaGroup
	if (!MediaGroupObj_Check(v))
	{
		*p_itself = Py_WrapAs_MediaGroup(v);
		if (*p_itself) return 1;
	}
#endif
	if (!MediaGroupObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "MediaGroup required");
		return 0;
	}
	*p_itself = ((MediaGroupObject *)v)->ob_itself;
	return 1;
}

static void MediaGroupObj_dealloc(MediaGroupObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *MediaGroupObj_destroyContents(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->destroyContents();
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaGroupObj_setText(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::TextNode* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      TextNodeObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setText(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaGroupObj_setImage(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::ImageNode* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ImageNodeObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setImage(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaGroupObj_addAudioClip(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::AudioNode* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      AudioNodeObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addAudioClip(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MediaGroupObj_hasText(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->hasText();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_hasAudio(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->hasAudio();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_hasImage(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->hasImage();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_getText(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::TextNode* _rv = _self->ob_itself->getText();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     TextNodeObj_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_getImage(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::ImageNode* _rv = _self->ob_itself->getImage();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ImageNodeObj_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_getAudio(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::AudioNode* _rv = _self->ob_itself->getAudio(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     AudioNodeObj_New, _rv);
	return _res;
}

static PyObject *MediaGroupObj_getNumberOfAudioClips(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	unsigned int _rv = _self->ob_itself->getNumberOfAudioClips();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *MediaGroupObj_getId(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *MediaGroupObj_setId(MediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef MediaGroupObj_methods[] = {
	{"destroyContents", (PyCFunction)MediaGroupObj_destroyContents, 1,
	 PyDoc_STR("() -> None")},
	{"setText", (PyCFunction)MediaGroupObj_setText, 1,
	 PyDoc_STR("(amis::TextNode* _arg1) -> None")},
	{"setImage", (PyCFunction)MediaGroupObj_setImage, 1,
	 PyDoc_STR("(amis::ImageNode* _arg1) -> None")},
	{"addAudioClip", (PyCFunction)MediaGroupObj_addAudioClip, 1,
	 PyDoc_STR("(amis::AudioNode* _arg1) -> None")},
	{"hasText", (PyCFunction)MediaGroupObj_hasText, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"hasAudio", (PyCFunction)MediaGroupObj_hasAudio, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"hasImage", (PyCFunction)MediaGroupObj_hasImage, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"getText", (PyCFunction)MediaGroupObj_getText, 1,
	 PyDoc_STR("() -> (amis::TextNode* _rv)")},
	{"getImage", (PyCFunction)MediaGroupObj_getImage, 1,
	 PyDoc_STR("() -> (amis::ImageNode* _rv)")},
	{"getAudio", (PyCFunction)MediaGroupObj_getAudio, 1,
	 PyDoc_STR("(unsigned int _arg1) -> (amis::AudioNode* _rv)")},
	{"getNumberOfAudioClips", (PyCFunction)MediaGroupObj_getNumberOfAudioClips, 1,
	 PyDoc_STR("() -> (unsigned int _rv)")},
	{"getId", (PyCFunction)MediaGroupObj_getId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setId", (PyCFunction)MediaGroupObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{NULL, NULL, 0}
};

#define MediaGroupObj_getsetlist NULL


static int MediaGroupObj_compare(MediaGroupObject *self, MediaGroupObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define MediaGroupObj_repr NULL

static int MediaGroupObj_hash(MediaGroupObject *self)
{
	return (int)self->ob_itself;
}
static int MediaGroupObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::MediaGroup* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((MediaGroupObject *)_self)->ob_itself = new amis::MediaGroup();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, MediaGroupObj_Convert, &itself))
	{
		((MediaGroupObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define MediaGroupObj_tp_alloc PyType_GenericAlloc

static PyObject *MediaGroupObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((MediaGroupObject *)_self)->ob_itself = NULL;
	return _self;
}

#define MediaGroupObj_tp_free PyObject_Del


PyTypeObject MediaGroup_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.MediaGroup", /*tp_name*/
	sizeof(MediaGroupObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) MediaGroupObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) MediaGroupObj_compare, /*tp_compare*/
	(reprfunc) MediaGroupObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) MediaGroupObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	MediaGroupObj_methods, /* tp_methods */
	0, /*tp_members*/
	MediaGroupObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	MediaGroupObj_tp_init, /* tp_init */
	MediaGroupObj_tp_alloc, /* tp_alloc */
	MediaGroupObj_tp_new, /* tp_new */
	MediaGroupObj_tp_free, /* tp_free */
};

/* ------------------- End object type MediaGroup ------------------- */


/* ------------------- Object type ModuleDescData ------------------- */

extern PyTypeObject ModuleDescData_Type;

inline bool ModuleDescDataObj_Check(PyObject *x)
{
	return ((x)->ob_type == &ModuleDescData_Type);
}

typedef struct ModuleDescDataObject {
	PyObject_HEAD
	amis::ModuleDescData* ob_itself;
} ModuleDescDataObject;

PyObject *ModuleDescDataObj_New(amis::ModuleDescData* itself)
{
	ModuleDescDataObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_ModuleDescData
	ModuleDescData *encaps_itself = dynamic_cast<ModuleDescData *>(itself);
	if (encaps_itself && encaps_itself->py_ModuleDescData)
	{
		Py_INCREF(encaps_itself->py_ModuleDescData);
		return encaps_itself->py_ModuleDescData;
	}
#endif
	it = PyObject_NEW(ModuleDescDataObject, &ModuleDescData_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int ModuleDescDataObj_Convert(PyObject *v, amis::ModuleDescData* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_ModuleDescData
	if (!ModuleDescDataObj_Check(v))
	{
		*p_itself = Py_WrapAs_ModuleDescData(v);
		if (*p_itself) return 1;
	}
#endif
	if (!ModuleDescDataObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "ModuleDescData required");
		return 0;
	}
	*p_itself = ((ModuleDescDataObject *)v)->ob_itself;
	return 1;
}

static void ModuleDescDataObj_dealloc(ModuleDescDataObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *ModuleDescDataObj_getLabel(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaGroup* _rv = _self->ob_itself->getLabel();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaGroupObj_New, _rv);
	return _res;
}

static PyObject *ModuleDescDataObj_getId(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *ModuleDescDataObj_getModuleType(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::ModuleDescData::ModuleType _rv = _self->ob_itself->getModuleType();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *ModuleDescDataObj_getDllFileName(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getDllFileName();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *ModuleDescDataObj_getXmlFileName(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getXmlFileName();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *ModuleDescDataObj_isEnabled(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->isEnabled();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *ModuleDescDataObj_setLabel(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLabel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ModuleDescDataObj_setId(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ModuleDescDataObj_setModuleType(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::ModuleDescData::ModuleType _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setModuleType(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ModuleDescDataObj_setDllFileName(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setDllFileName(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ModuleDescDataObj_setXmlFileName(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setXmlFileName(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *ModuleDescDataObj_setIsEnabled(ModuleDescDataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setIsEnabled(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef ModuleDescDataObj_methods[] = {
	{"getLabel", (PyCFunction)ModuleDescDataObj_getLabel, 1,
	 PyDoc_STR("() -> (amis::MediaGroup* _rv)")},
	{"getId", (PyCFunction)ModuleDescDataObj_getId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getModuleType", (PyCFunction)ModuleDescDataObj_getModuleType, 1,
	 PyDoc_STR("() -> (amis::ModuleDescData::ModuleType _rv)")},
	{"getDllFileName", (PyCFunction)ModuleDescDataObj_getDllFileName, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getXmlFileName", (PyCFunction)ModuleDescDataObj_getXmlFileName, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"isEnabled", (PyCFunction)ModuleDescDataObj_isEnabled, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"setLabel", (PyCFunction)ModuleDescDataObj_setLabel, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"setId", (PyCFunction)ModuleDescDataObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setModuleType", (PyCFunction)ModuleDescDataObj_setModuleType, 1,
	 PyDoc_STR("(amis::ModuleDescData::ModuleType _arg1) -> None")},
	{"setDllFileName", (PyCFunction)ModuleDescDataObj_setDllFileName, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setXmlFileName", (PyCFunction)ModuleDescDataObj_setXmlFileName, 1,
	 PyDoc_STR("(ambulant::net::url _arg1) -> None")},
	{"setIsEnabled", (PyCFunction)ModuleDescDataObj_setIsEnabled, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{NULL, NULL, 0}
};

#define ModuleDescDataObj_getsetlist NULL


static int ModuleDescDataObj_compare(ModuleDescDataObject *self, ModuleDescDataObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define ModuleDescDataObj_repr NULL

static int ModuleDescDataObj_hash(ModuleDescDataObject *self)
{
	return (int)self->ob_itself;
}
static int ModuleDescDataObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::ModuleDescData* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((ModuleDescDataObject *)_self)->ob_itself = new amis::ModuleDescData();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, ModuleDescDataObj_Convert, &itself))
	{
		((ModuleDescDataObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define ModuleDescDataObj_tp_alloc PyType_GenericAlloc

static PyObject *ModuleDescDataObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((ModuleDescDataObject *)_self)->ob_itself = NULL;
	return _self;
}

#define ModuleDescDataObj_tp_free PyObject_Del


PyTypeObject ModuleDescData_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.ModuleDescData", /*tp_name*/
	sizeof(ModuleDescDataObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) ModuleDescDataObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) ModuleDescDataObj_compare, /*tp_compare*/
	(reprfunc) ModuleDescDataObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) ModuleDescDataObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	ModuleDescDataObj_methods, /* tp_methods */
	0, /*tp_members*/
	ModuleDescDataObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	ModuleDescDataObj_tp_init, /* tp_init */
	ModuleDescDataObj_tp_alloc, /* tp_alloc */
	ModuleDescDataObj_tp_new, /* tp_new */
	ModuleDescDataObj_tp_free, /* tp_free */
};

/* ----------------- End object type ModuleDescData ----------------- */


/* ------------------ Object type RecentBookEntry ------------------- */

extern PyTypeObject RecentBookEntry_Type;

inline bool RecentBookEntryObj_Check(PyObject *x)
{
	return ((x)->ob_type == &RecentBookEntry_Type);
}

typedef struct RecentBookEntryObject {
	PyObject_HEAD
	amis::RecentBookEntry* ob_itself;
} RecentBookEntryObject;

PyObject *RecentBookEntryObj_New(amis::RecentBookEntry* itself)
{
	RecentBookEntryObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_RecentBookEntry
	RecentBookEntry *encaps_itself = dynamic_cast<RecentBookEntry *>(itself);
	if (encaps_itself && encaps_itself->py_RecentBookEntry)
	{
		Py_INCREF(encaps_itself->py_RecentBookEntry);
		return encaps_itself->py_RecentBookEntry;
	}
#endif
	it = PyObject_NEW(RecentBookEntryObject, &RecentBookEntry_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int RecentBookEntryObj_Convert(PyObject *v, amis::RecentBookEntry* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_RecentBookEntry
	if (!RecentBookEntryObj_Check(v))
	{
		*p_itself = Py_WrapAs_RecentBookEntry(v);
		if (*p_itself) return 1;
	}
#endif
	if (!RecentBookEntryObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "RecentBookEntry required");
		return 0;
	}
	*p_itself = ((RecentBookEntryObject *)v)->ob_itself;
	return 1;
}

static void RecentBookEntryObj_dealloc(RecentBookEntryObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMethodDef RecentBookEntryObj_methods[] = {
	{NULL, NULL, 0}
};

#define RecentBookEntryObj_getsetlist NULL


static int RecentBookEntryObj_compare(RecentBookEntryObject *self, RecentBookEntryObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define RecentBookEntryObj_repr NULL

static int RecentBookEntryObj_hash(RecentBookEntryObject *self)
{
	return (int)self->ob_itself;
}
static int RecentBookEntryObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::RecentBookEntry* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((RecentBookEntryObject *)_self)->ob_itself = new amis::RecentBookEntry();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, RecentBookEntryObj_Convert, &itself))
	{
		((RecentBookEntryObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define RecentBookEntryObj_tp_alloc PyType_GenericAlloc

static PyObject *RecentBookEntryObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((RecentBookEntryObject *)_self)->ob_itself = NULL;
	return _self;
}

#define RecentBookEntryObj_tp_free PyObject_Del


PyTypeObject RecentBookEntry_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.RecentBookEntry", /*tp_name*/
	sizeof(RecentBookEntryObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) RecentBookEntryObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) RecentBookEntryObj_compare, /*tp_compare*/
	(reprfunc) RecentBookEntryObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) RecentBookEntryObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	RecentBookEntryObj_methods, /* tp_methods */
	0, /*tp_members*/
	RecentBookEntryObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	RecentBookEntryObj_tp_init, /* tp_init */
	RecentBookEntryObj_tp_alloc, /* tp_alloc */
	RecentBookEntryObj_tp_new, /* tp_new */
	RecentBookEntryObj_tp_free, /* tp_free */
};

/* ---------------- End object type RecentBookEntry ----------------- */


/* -------------------- Object type RecentBooks --------------------- */

extern PyTypeObject RecentBooks_Type;

inline bool RecentBooksObj_Check(PyObject *x)
{
	return ((x)->ob_type == &RecentBooks_Type);
}

typedef struct RecentBooksObject {
	PyObject_HEAD
	amis::RecentBooks* ob_itself;
} RecentBooksObject;

PyObject *RecentBooksObj_New(amis::RecentBooks* itself)
{
	RecentBooksObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_RecentBooks
	RecentBooks *encaps_itself = dynamic_cast<RecentBooks *>(itself);
	if (encaps_itself && encaps_itself->py_RecentBooks)
	{
		Py_INCREF(encaps_itself->py_RecentBooks);
		return encaps_itself->py_RecentBooks;
	}
#endif
	it = PyObject_NEW(RecentBooksObject, &RecentBooks_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int RecentBooksObj_Convert(PyObject *v, amis::RecentBooks* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_RecentBooks
	if (!RecentBooksObj_Check(v))
	{
		*p_itself = Py_WrapAs_RecentBooks(v);
		if (*p_itself) return 1;
	}
#endif
	if (!RecentBooksObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "RecentBooks required");
		return 0;
	}
	*p_itself = ((RecentBooksObject *)v)->ob_itself;
	return 1;
}

static void RecentBooksObj_dealloc(RecentBooksObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *RecentBooksObj_getNumberOfEntries(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getNumberOfEntries();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *RecentBooksObj_getEntry(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::RecentBookEntry* _rv = _self->ob_itself->getEntry(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     RecentBookEntryObj_New, _rv);
	return _res;
}

static PyObject *RecentBooksObj_addEntry(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::RecentBookEntry* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      RecentBookEntryObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addEntry(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *RecentBooksObj_deleteEntry(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->deleteEntry(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *RecentBooksObj_getMaxItemsAllowed(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getMaxItemsAllowed();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *RecentBooksObj_setMaxItemsAllowed(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setMaxItemsAllowed(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *RecentBooksObj_getLastRead(RecentBooksObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::RecentBookEntry* _rv = _self->ob_itself->getLastRead();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     RecentBookEntryObj_New, _rv);
	return _res;
}

static PyMethodDef RecentBooksObj_methods[] = {
	{"getNumberOfEntries", (PyCFunction)RecentBooksObj_getNumberOfEntries, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getEntry", (PyCFunction)RecentBooksObj_getEntry, 1,
	 PyDoc_STR("(unsigned int _arg1) -> (amis::RecentBookEntry* _rv)")},
	{"addEntry", (PyCFunction)RecentBooksObj_addEntry, 1,
	 PyDoc_STR("(amis::RecentBookEntry* _arg1) -> None")},
	{"deleteEntry", (PyCFunction)RecentBooksObj_deleteEntry, 1,
	 PyDoc_STR("(unsigned int _arg1) -> None")},
	{"getMaxItemsAllowed", (PyCFunction)RecentBooksObj_getMaxItemsAllowed, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"setMaxItemsAllowed", (PyCFunction)RecentBooksObj_setMaxItemsAllowed, 1,
	 PyDoc_STR("(unsigned int _arg1) -> None")},
	{"getLastRead", (PyCFunction)RecentBooksObj_getLastRead, 1,
	 PyDoc_STR("() -> (amis::RecentBookEntry* _rv)")},
	{NULL, NULL, 0}
};

#define RecentBooksObj_getsetlist NULL


static int RecentBooksObj_compare(RecentBooksObject *self, RecentBooksObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define RecentBooksObj_repr NULL

static int RecentBooksObj_hash(RecentBooksObject *self)
{
	return (int)self->ob_itself;
}
static int RecentBooksObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::RecentBooks* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((RecentBooksObject *)_self)->ob_itself = new amis::RecentBooks();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, RecentBooksObj_Convert, &itself))
	{
		((RecentBooksObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define RecentBooksObj_tp_alloc PyType_GenericAlloc

static PyObject *RecentBooksObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((RecentBooksObject *)_self)->ob_itself = NULL;
	return _self;
}

#define RecentBooksObj_tp_free PyObject_Del


PyTypeObject RecentBooks_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.RecentBooks", /*tp_name*/
	sizeof(RecentBooksObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) RecentBooksObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) RecentBooksObj_compare, /*tp_compare*/
	(reprfunc) RecentBooksObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) RecentBooksObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	RecentBooksObj_methods, /* tp_methods */
	0, /*tp_members*/
	RecentBooksObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	RecentBooksObj_tp_init, /* tp_init */
	RecentBooksObj_tp_alloc, /* tp_alloc */
	RecentBooksObj_tp_new, /* tp_new */
	RecentBooksObj_tp_free, /* tp_free */
};

/* ------------------ End object type RecentBooks ------------------- */


/* -------------------- Object type PositionData -------------------- */

extern PyTypeObject PositionData_Type;

inline bool PositionDataObj_Check(PyObject *x)
{
	return ((x)->ob_type == &PositionData_Type);
}

typedef struct PositionDataObject {
	PyObject_HEAD
	amis::dtb::PositionData* ob_itself;
} PositionDataObject;

PyObject *PositionDataObj_New(amis::dtb::PositionData* itself)
{
	PositionDataObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_PositionData
	PositionData *encaps_itself = dynamic_cast<PositionData *>(itself);
	if (encaps_itself && encaps_itself->py_PositionData)
	{
		Py_INCREF(encaps_itself->py_PositionData);
		return encaps_itself->py_PositionData;
	}
#endif
	it = PyObject_NEW(PositionDataObject, &PositionData_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int PositionDataObj_Convert(PyObject *v, amis::dtb::PositionData* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_PositionData
	if (!PositionDataObj_Check(v))
	{
		*p_itself = Py_WrapAs_PositionData(v);
		if (*p_itself) return 1;
	}
#endif
	if (!PositionDataObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "PositionData required");
		return 0;
	}
	*p_itself = ((PositionDataObject *)v)->ob_itself;
	return 1;
}

static void PositionDataObj_dealloc(PositionDataObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMethodDef PositionDataObj_methods[] = {
	{NULL, NULL, 0}
};

#define PositionDataObj_getsetlist NULL


static int PositionDataObj_compare(PositionDataObject *self, PositionDataObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define PositionDataObj_repr NULL

static int PositionDataObj_hash(PositionDataObject *self)
{
	return (int)self->ob_itself;
}
static int PositionDataObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::PositionData* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((PositionDataObject *)_self)->ob_itself = new amis::dtb::PositionData();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, PositionDataObj_Convert, &itself))
	{
		((PositionDataObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define PositionDataObj_tp_alloc PyType_GenericAlloc

static PyObject *PositionDataObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((PositionDataObject *)_self)->ob_itself = NULL;
	return _self;
}

#define PositionDataObj_tp_free PyObject_Del


PyTypeObject PositionData_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.PositionData", /*tp_name*/
	sizeof(PositionDataObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) PositionDataObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) PositionDataObj_compare, /*tp_compare*/
	(reprfunc) PositionDataObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) PositionDataObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	PositionDataObj_methods, /* tp_methods */
	0, /*tp_members*/
	PositionDataObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	PositionDataObj_tp_init, /* tp_init */
	PositionDataObj_tp_alloc, /* tp_alloc */
	PositionDataObj_tp_new, /* tp_new */
	PositionDataObj_tp_free, /* tp_free */
};

/* ------------------ End object type PositionData ------------------ */


/* -------------------- Object type PositionMark -------------------- */

extern PyTypeObject PositionMark_Type;

inline bool PositionMarkObj_Check(PyObject *x)
{
	return ((x)->ob_type == &PositionMark_Type);
}

typedef struct PositionMarkObject {
	PyObject_HEAD
	amis::dtb::PositionMark* ob_itself;
} PositionMarkObject;

PyObject *PositionMarkObj_New(amis::dtb::PositionMark* itself)
{
	PositionMarkObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_PositionMark
	PositionMark *encaps_itself = dynamic_cast<PositionMark *>(itself);
	if (encaps_itself && encaps_itself->py_PositionMark)
	{
		Py_INCREF(encaps_itself->py_PositionMark);
		return encaps_itself->py_PositionMark;
	}
#endif
	it = PyObject_NEW(PositionMarkObject, &PositionMark_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int PositionMarkObj_Convert(PyObject *v, amis::dtb::PositionMark* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_PositionMark
	if (!PositionMarkObj_Check(v))
	{
		*p_itself = Py_WrapAs_PositionMark(v);
		if (*p_itself) return 1;
	}
#endif
	if (!PositionMarkObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "PositionMark required");
		return 0;
	}
	*p_itself = ((PositionMarkObject *)v)->ob_itself;
	return 1;
}

static void PositionMarkObj_dealloc(PositionMarkObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMethodDef PositionMarkObj_methods[] = {
	{NULL, NULL, 0}
};

#define PositionMarkObj_getsetlist NULL


static int PositionMarkObj_compare(PositionMarkObject *self, PositionMarkObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define PositionMarkObj_repr NULL

static int PositionMarkObj_hash(PositionMarkObject *self)
{
	return (int)self->ob_itself;
}
static int PositionMarkObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::PositionMark* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, PositionMarkObj_Convert, &itself))
	{
		((PositionMarkObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define PositionMarkObj_tp_alloc PyType_GenericAlloc

static PyObject *PositionMarkObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((PositionMarkObject *)_self)->ob_itself = NULL;
	return _self;
}

#define PositionMarkObj_tp_free PyObject_Del


PyTypeObject PositionMark_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.PositionMark", /*tp_name*/
	sizeof(PositionMarkObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) PositionMarkObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) PositionMarkObj_compare, /*tp_compare*/
	(reprfunc) PositionMarkObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) PositionMarkObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	PositionMarkObj_methods, /* tp_methods */
	0, /*tp_members*/
	PositionMarkObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	PositionMarkObj_tp_init, /* tp_init */
	PositionMarkObj_tp_alloc, /* tp_alloc */
	PositionMarkObj_tp_new, /* tp_new */
	PositionMarkObj_tp_free, /* tp_free */
};

/* ------------------ End object type PositionMark ------------------ */


/* ---------------------- Object type Bookmark ---------------------- */

extern PyTypeObject Bookmark_Type;

inline bool BookmarkObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Bookmark_Type);
}

typedef struct BookmarkObject {
	PyObject_HEAD
	amis::dtb::Bookmark* ob_itself;
} BookmarkObject;

PyObject *BookmarkObj_New(amis::dtb::Bookmark* itself)
{
	BookmarkObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Bookmark
	Bookmark *encaps_itself = dynamic_cast<Bookmark *>(itself);
	if (encaps_itself && encaps_itself->py_Bookmark)
	{
		Py_INCREF(encaps_itself->py_Bookmark);
		return encaps_itself->py_Bookmark;
	}
#endif
	it = PyObject_NEW(BookmarkObject, &Bookmark_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int BookmarkObj_Convert(PyObject *v, amis::dtb::Bookmark* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Bookmark
	if (!BookmarkObj_Check(v))
	{
		*p_itself = Py_WrapAs_Bookmark(v);
		if (*p_itself) return 1;
	}
#endif
	if (!BookmarkObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Bookmark required");
		return 0;
	}
	*p_itself = ((BookmarkObject *)v)->ob_itself;
	return 1;
}

static void BookmarkObj_dealloc(BookmarkObject *self)
{
	PositionMark_Type.tp_dealloc((PyObject *)self);
}

static PyMethodDef BookmarkObj_methods[] = {
	{NULL, NULL, 0}
};

#define BookmarkObj_getsetlist NULL


static int BookmarkObj_compare(BookmarkObject *self, BookmarkObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define BookmarkObj_repr NULL

static int BookmarkObj_hash(BookmarkObject *self)
{
	return (int)self->ob_itself;
}
static int BookmarkObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::Bookmark* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((BookmarkObject *)_self)->ob_itself = new amis::dtb::Bookmark();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, BookmarkObj_Convert, &itself))
	{
		((BookmarkObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define BookmarkObj_tp_alloc PyType_GenericAlloc

static PyObject *BookmarkObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((BookmarkObject *)_self)->ob_itself = NULL;
	return _self;
}

#define BookmarkObj_tp_free PyObject_Del


PyTypeObject Bookmark_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Bookmark", /*tp_name*/
	sizeof(BookmarkObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) BookmarkObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) BookmarkObj_compare, /*tp_compare*/
	(reprfunc) BookmarkObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) BookmarkObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	BookmarkObj_methods, /* tp_methods */
	0, /*tp_members*/
	BookmarkObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	BookmarkObj_tp_init, /* tp_init */
	BookmarkObj_tp_alloc, /* tp_alloc */
	BookmarkObj_tp_new, /* tp_new */
	BookmarkObj_tp_free, /* tp_free */
};

/* -------------------- End object type Bookmark -------------------- */


/* ----------------------- Object type Hilite ----------------------- */

extern PyTypeObject Hilite_Type;

inline bool HiliteObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Hilite_Type);
}

typedef struct HiliteObject {
	PyObject_HEAD
	amis::dtb::Hilite* ob_itself;
} HiliteObject;

PyObject *HiliteObj_New(amis::dtb::Hilite* itself)
{
	HiliteObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Hilite
	Hilite *encaps_itself = dynamic_cast<Hilite *>(itself);
	if (encaps_itself && encaps_itself->py_Hilite)
	{
		Py_INCREF(encaps_itself->py_Hilite);
		return encaps_itself->py_Hilite;
	}
#endif
	it = PyObject_NEW(HiliteObject, &Hilite_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int HiliteObj_Convert(PyObject *v, amis::dtb::Hilite* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Hilite
	if (!HiliteObj_Check(v))
	{
		*p_itself = Py_WrapAs_Hilite(v);
		if (*p_itself) return 1;
	}
#endif
	if (!HiliteObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Hilite required");
		return 0;
	}
	*p_itself = ((HiliteObject *)v)->ob_itself;
	return 1;
}

static void HiliteObj_dealloc(HiliteObject *self)
{
	PositionMark_Type.tp_dealloc((PyObject *)self);
}

static PyMethodDef HiliteObj_methods[] = {
	{NULL, NULL, 0}
};

#define HiliteObj_getsetlist NULL


static int HiliteObj_compare(HiliteObject *self, HiliteObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define HiliteObj_repr NULL

static int HiliteObj_hash(HiliteObject *self)
{
	return (int)self->ob_itself;
}
static int HiliteObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::Hilite* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((HiliteObject *)_self)->ob_itself = new amis::dtb::Hilite();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, HiliteObj_Convert, &itself))
	{
		((HiliteObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define HiliteObj_tp_alloc PyType_GenericAlloc

static PyObject *HiliteObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((HiliteObject *)_self)->ob_itself = NULL;
	return _self;
}

#define HiliteObj_tp_free PyObject_Del


PyTypeObject Hilite_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Hilite", /*tp_name*/
	sizeof(HiliteObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) HiliteObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) HiliteObj_compare, /*tp_compare*/
	(reprfunc) HiliteObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) HiliteObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	HiliteObj_methods, /* tp_methods */
	0, /*tp_members*/
	HiliteObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	HiliteObj_tp_init, /* tp_init */
	HiliteObj_tp_alloc, /* tp_alloc */
	HiliteObj_tp_new, /* tp_new */
	HiliteObj_tp_free, /* tp_free */
};

/* --------------------- End object type Hilite --------------------- */


/* -------------------- Object type BookmarkSet --------------------- */

extern PyTypeObject BookmarkSet_Type;

inline bool BookmarkSetObj_Check(PyObject *x)
{
	return ((x)->ob_type == &BookmarkSet_Type);
}

typedef struct BookmarkSetObject {
	PyObject_HEAD
	amis::dtb::BookmarkSet* ob_itself;
} BookmarkSetObject;

PyObject *BookmarkSetObj_New(amis::dtb::BookmarkSet* itself)
{
	BookmarkSetObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_BookmarkSet
	BookmarkSet *encaps_itself = dynamic_cast<BookmarkSet *>(itself);
	if (encaps_itself && encaps_itself->py_BookmarkSet)
	{
		Py_INCREF(encaps_itself->py_BookmarkSet);
		return encaps_itself->py_BookmarkSet;
	}
#endif
	it = PyObject_NEW(BookmarkSetObject, &BookmarkSet_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int BookmarkSetObj_Convert(PyObject *v, amis::dtb::BookmarkSet* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_BookmarkSet
	if (!BookmarkSetObj_Check(v))
	{
		*p_itself = Py_WrapAs_BookmarkSet(v);
		if (*p_itself) return 1;
	}
#endif
	if (!BookmarkSetObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "BookmarkSet required");
		return 0;
	}
	*p_itself = ((BookmarkSetObject *)v)->ob_itself;
	return 1;
}

static void BookmarkSetObj_dealloc(BookmarkSetObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *BookmarkSetObj_getNumberOfItems(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	unsigned int _rv = _self->ob_itself->getNumberOfItems();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *BookmarkSetObj_getItem(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::PositionMark* _rv = _self->ob_itself->getItem(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     PositionMarkObj_New, _rv);
	return _res;
}

static PyObject *BookmarkSetObj_getLastmark(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::PositionData* _rv = _self->ob_itself->getLastmark();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     PositionDataObj_New, _rv);
	return _res;
}

static PyObject *BookmarkSetObj_getUid(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getUid();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *BookmarkSetObj_getTitle(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaGroup* _rv = _self->ob_itself->getTitle();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaGroupObj_New, _rv);
	return _res;
}

static PyObject *BookmarkSetObj_addHilite(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::Hilite* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      HiliteObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addHilite(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_addBookmark(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::Bookmark* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      BookmarkObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addBookmark(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_deleteItem(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->deleteItem(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_printPositionData(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::PositionData* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      PositionDataObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->printPositionData(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_printMediaGroup(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->printMediaGroup(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_setTitle(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setTitle(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_setUid(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setUid(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_setLastmark(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::PositionData* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      PositionDataObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLastmark(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_setFilepath(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setFilepath(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *BookmarkSetObj_getFilepath(BookmarkSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getFilepath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyMethodDef BookmarkSetObj_methods[] = {
	{"getNumberOfItems", (PyCFunction)BookmarkSetObj_getNumberOfItems, 1,
	 PyDoc_STR("() -> (unsigned int _rv)")},
	{"getItem", (PyCFunction)BookmarkSetObj_getItem, 1,
	 PyDoc_STR("(unsigned int _arg1) -> (amis::dtb::PositionMark* _rv)")},
	{"getLastmark", (PyCFunction)BookmarkSetObj_getLastmark, 1,
	 PyDoc_STR("() -> (amis::dtb::PositionData* _rv)")},
	{"getUid", (PyCFunction)BookmarkSetObj_getUid, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getTitle", (PyCFunction)BookmarkSetObj_getTitle, 1,
	 PyDoc_STR("() -> (amis::MediaGroup* _rv)")},
	{"addHilite", (PyCFunction)BookmarkSetObj_addHilite, 1,
	 PyDoc_STR("(amis::dtb::Hilite* _arg1) -> None")},
	{"addBookmark", (PyCFunction)BookmarkSetObj_addBookmark, 1,
	 PyDoc_STR("(amis::dtb::Bookmark* _arg1) -> None")},
	{"deleteItem", (PyCFunction)BookmarkSetObj_deleteItem, 1,
	 PyDoc_STR("(unsigned int _arg1) -> None")},
	{"printPositionData", (PyCFunction)BookmarkSetObj_printPositionData, 1,
	 PyDoc_STR("(amis::dtb::PositionData* _arg1) -> None")},
	{"printMediaGroup", (PyCFunction)BookmarkSetObj_printMediaGroup, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"setTitle", (PyCFunction)BookmarkSetObj_setTitle, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"setUid", (PyCFunction)BookmarkSetObj_setUid, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setLastmark", (PyCFunction)BookmarkSetObj_setLastmark, 1,
	 PyDoc_STR("(amis::dtb::PositionData* _arg1) -> None")},
	{"setFilepath", (PyCFunction)BookmarkSetObj_setFilepath, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> None")},
	{"getFilepath", (PyCFunction)BookmarkSetObj_getFilepath, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{NULL, NULL, 0}
};

#define BookmarkSetObj_getsetlist NULL


static int BookmarkSetObj_compare(BookmarkSetObject *self, BookmarkSetObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define BookmarkSetObj_repr NULL

static int BookmarkSetObj_hash(BookmarkSetObject *self)
{
	return (int)self->ob_itself;
}
static int BookmarkSetObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::BookmarkSet* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((BookmarkSetObject *)_self)->ob_itself = new amis::dtb::BookmarkSet();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, BookmarkSetObj_Convert, &itself))
	{
		((BookmarkSetObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define BookmarkSetObj_tp_alloc PyType_GenericAlloc

static PyObject *BookmarkSetObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((BookmarkSetObject *)_self)->ob_itself = NULL;
	return _self;
}

#define BookmarkSetObj_tp_free PyObject_Del


PyTypeObject BookmarkSet_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.BookmarkSet", /*tp_name*/
	sizeof(BookmarkSetObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) BookmarkSetObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) BookmarkSetObj_compare, /*tp_compare*/
	(reprfunc) BookmarkSetObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) BookmarkSetObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	BookmarkSetObj_methods, /* tp_methods */
	0, /*tp_members*/
	BookmarkSetObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	BookmarkSetObj_tp_init, /* tp_init */
	BookmarkSetObj_tp_alloc, /* tp_alloc */
	BookmarkSetObj_tp_new, /* tp_new */
	BookmarkSetObj_tp_free, /* tp_free */
};

/* ------------------ End object type BookmarkSet ------------------- */


/* --------------------- Object type CustomTest --------------------- */

extern PyTypeObject CustomTest_Type;

inline bool CustomTestObj_Check(PyObject *x)
{
	return ((x)->ob_type == &CustomTest_Type);
}

typedef struct CustomTestObject {
	PyObject_HEAD
	amis::dtb::CustomTest* ob_itself;
} CustomTestObject;

PyObject *CustomTestObj_New(amis::dtb::CustomTest* itself)
{
	CustomTestObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_CustomTest
	CustomTest *encaps_itself = dynamic_cast<CustomTest *>(itself);
	if (encaps_itself && encaps_itself->py_CustomTest)
	{
		Py_INCREF(encaps_itself->py_CustomTest);
		return encaps_itself->py_CustomTest;
	}
#endif
	it = PyObject_NEW(CustomTestObject, &CustomTest_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int CustomTestObj_Convert(PyObject *v, amis::dtb::CustomTest* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_CustomTest
	if (!CustomTestObj_Check(v))
	{
		*p_itself = Py_WrapAs_CustomTest(v);
		if (*p_itself) return 1;
	}
#endif
	if (!CustomTestObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "CustomTest required");
		return 0;
	}
	*p_itself = ((CustomTestObject *)v)->ob_itself;
	return 1;
}

static void CustomTestObj_dealloc(CustomTestObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *CustomTestObj_setId(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *CustomTestObj_setOverride(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setOverride(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *CustomTestObj_setDefaultState(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setDefaultState(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *CustomTestObj_setBookStruct(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setBookStruct(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *CustomTestObj_setCurrentState(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setCurrentState(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *CustomTestObj_getId(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *CustomTestObj_getOverride(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->getOverride();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *CustomTestObj_getDefaultState(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->getDefaultState();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *CustomTestObj_getBookStruct(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const std::string _rv = _self->ob_itself->getBookStruct();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *CustomTestObj_getCurrentState(CustomTestObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->getCurrentState();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyMethodDef CustomTestObj_methods[] = {
	{"setId", (PyCFunction)CustomTestObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setOverride", (PyCFunction)CustomTestObj_setOverride, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{"setDefaultState", (PyCFunction)CustomTestObj_setDefaultState, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{"setBookStruct", (PyCFunction)CustomTestObj_setBookStruct, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setCurrentState", (PyCFunction)CustomTestObj_setCurrentState, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{"getId", (PyCFunction)CustomTestObj_getId, 1,
	 PyDoc_STR("() -> (const std::string _rv)")},
	{"getOverride", (PyCFunction)CustomTestObj_getOverride, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"getDefaultState", (PyCFunction)CustomTestObj_getDefaultState, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"getBookStruct", (PyCFunction)CustomTestObj_getBookStruct, 1,
	 PyDoc_STR("() -> (const std::string _rv)")},
	{"getCurrentState", (PyCFunction)CustomTestObj_getCurrentState, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{NULL, NULL, 0}
};

#define CustomTestObj_getsetlist NULL


static int CustomTestObj_compare(CustomTestObject *self, CustomTestObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define CustomTestObj_repr NULL

static int CustomTestObj_hash(CustomTestObject *self)
{
	return (int)self->ob_itself;
}
static int CustomTestObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::CustomTest* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((CustomTestObject *)_self)->ob_itself = new amis::dtb::CustomTest();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, CustomTestObj_Convert, &itself))
	{
		((CustomTestObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define CustomTestObj_tp_alloc PyType_GenericAlloc

static PyObject *CustomTestObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((CustomTestObject *)_self)->ob_itself = NULL;
	return _self;
}

#define CustomTestObj_tp_free PyObject_Del


PyTypeObject CustomTest_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.CustomTest", /*tp_name*/
	sizeof(CustomTestObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) CustomTestObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) CustomTestObj_compare, /*tp_compare*/
	(reprfunc) CustomTestObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) CustomTestObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	CustomTestObj_methods, /* tp_methods */
	0, /*tp_members*/
	CustomTestObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	CustomTestObj_tp_init, /* tp_init */
	CustomTestObj_tp_alloc, /* tp_alloc */
	CustomTestObj_tp_new, /* tp_new */
	CustomTestObj_tp_free, /* tp_free */
};

/* ------------------- End object type CustomTest ------------------- */


/* ------------------- Object type CustomTestSet -------------------- */

extern PyTypeObject CustomTestSet_Type;

inline bool CustomTestSetObj_Check(PyObject *x)
{
	return ((x)->ob_type == &CustomTestSet_Type);
}

typedef struct CustomTestSetObject {
	PyObject_HEAD
	amis::dtb::CustomTestSet* ob_itself;
} CustomTestSetObject;

PyObject *CustomTestSetObj_New(amis::dtb::CustomTestSet* itself)
{
	CustomTestSetObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_CustomTestSet
	CustomTestSet *encaps_itself = dynamic_cast<CustomTestSet *>(itself);
	if (encaps_itself && encaps_itself->py_CustomTestSet)
	{
		Py_INCREF(encaps_itself->py_CustomTestSet);
		return encaps_itself->py_CustomTestSet;
	}
#endif
	it = PyObject_NEW(CustomTestSetObject, &CustomTestSet_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int CustomTestSetObj_Convert(PyObject *v, amis::dtb::CustomTestSet* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_CustomTestSet
	if (!CustomTestSetObj_Check(v))
	{
		*p_itself = Py_WrapAs_CustomTestSet(v);
		if (*p_itself) return 1;
	}
#endif
	if (!CustomTestSetObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "CustomTestSet required");
		return 0;
	}
	*p_itself = ((CustomTestSetObject *)v)->ob_itself;
	return 1;
}

static void CustomTestSetObj_dealloc(CustomTestSetObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *CustomTestSetObj_addCustomTest(CustomTestSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::CustomTest* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      CustomTestObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->addCustomTest(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *CustomTestSetObj_getCustomTest(CustomTestSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::CustomTest* _rv = _self->ob_itself->getCustomTest(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     CustomTestObj_New, _rv);
	return _res;
}

static PyObject *CustomTestSetObj_getLength(CustomTestSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getLength();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyMethodDef CustomTestSetObj_methods[] = {
	{"addCustomTest", (PyCFunction)CustomTestSetObj_addCustomTest, 1,
	 PyDoc_STR("(amis::dtb::CustomTest* _arg1) -> (bool _rv)")},
	{"getCustomTest", (PyCFunction)CustomTestSetObj_getCustomTest, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::CustomTest* _rv)")},
	{"getLength", (PyCFunction)CustomTestSetObj_getLength, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{NULL, NULL, 0}
};

#define CustomTestSetObj_getsetlist NULL


static int CustomTestSetObj_compare(CustomTestSetObject *self, CustomTestSetObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define CustomTestSetObj_repr NULL

static int CustomTestSetObj_hash(CustomTestSetObject *self)
{
	return (int)self->ob_itself;
}
static int CustomTestSetObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::CustomTestSet* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((CustomTestSetObject *)_self)->ob_itself = new amis::dtb::CustomTestSet();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, CustomTestSetObj_Convert, &itself))
	{
		((CustomTestSetObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define CustomTestSetObj_tp_alloc PyType_GenericAlloc

static PyObject *CustomTestSetObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((CustomTestSetObject *)_self)->ob_itself = NULL;
	return _self;
}

#define CustomTestSetObj_tp_free PyObject_Del


PyTypeObject CustomTestSet_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.CustomTestSet", /*tp_name*/
	sizeof(CustomTestSetObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) CustomTestSetObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) CustomTestSetObj_compare, /*tp_compare*/
	(reprfunc) CustomTestSetObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) CustomTestSetObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	CustomTestSetObj_methods, /* tp_methods */
	0, /*tp_members*/
	CustomTestSetObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	CustomTestSetObj_tp_init, /* tp_init */
	CustomTestSetObj_tp_alloc, /* tp_alloc */
	CustomTestSetObj_tp_new, /* tp_new */
	CustomTestSetObj_tp_free, /* tp_free */
};

/* ----------------- End object type CustomTestSet ------------------ */


/* ------------------------ Object type Dtb ------------------------- */

extern PyTypeObject Dtb_Type;

inline bool DtbObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Dtb_Type);
}

typedef struct DtbObject {
	PyObject_HEAD
	amis::dtb::Dtb* ob_itself;
} DtbObject;

PyObject *DtbObj_New(amis::dtb::Dtb* itself)
{
	DtbObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Dtb
	Dtb *encaps_itself = dynamic_cast<Dtb *>(itself);
	if (encaps_itself && encaps_itself->py_Dtb)
	{
		Py_INCREF(encaps_itself->py_Dtb);
		return encaps_itself->py_Dtb;
	}
#endif
	it = PyObject_NEW(DtbObject, &Dtb_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int DtbObj_Convert(PyObject *v, amis::dtb::Dtb* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Dtb
	if (!DtbObj_Check(v))
	{
		*p_itself = Py_WrapAs_Dtb(v);
		if (*p_itself) return 1;
	}
#endif
	if (!DtbObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Dtb required");
		return 0;
	}
	*p_itself = ((DtbObject *)v)->ob_itself;
	return 1;
}

static void DtbObj_dealloc(DtbObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *DtbObj_open_1(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	ambulant::net::url* _arg2;
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      ambulant_url_Convert, &_arg1,
	                      ambulant_url_Convert, &_arg2))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->open(_arg1,
	                                  _arg2);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *DtbObj_open_2(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	std::string _arg2;
	char *_arg1_cstr;
	char *_arg2_cstr;
	if (!PyArg_ParseTuple(_args, "ss",
	                      &_arg1_cstr,
	                      &_arg2_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	_arg2 = _arg2_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->open(_arg1,
	                                  _arg2);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *DtbObj_startReading(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->startReading(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_nextPhrase(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->nextPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_previousPhrase(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->previousPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_loadSmilFromUrl(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->loadSmilFromUrl(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_getMetadata(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::Metadata* _rv = _self->ob_itself->getMetadata();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MetadataObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_getSpine(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::Spine* _rv = _self->ob_itself->getSpine();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SpineObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_getNavModel(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavModel* _rv = _self->ob_itself->getNavModel();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavModelObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_getFileSet(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::DtbFileSet* _rv = _self->ob_itself->getFileSet();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     DtbFileSetObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_getUid(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::wstring _rv = _self->ob_itself->getUid();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("u",
	                     _rv.c_str());
	return _res;
}

static PyObject *DtbObj_getBookmarks(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::BookmarkSet* _rv = _self->ob_itself->getBookmarks();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     BookmarkSetObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_searchFullText(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::wstring _arg1;
	wchar_t *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "u",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->searchFullText(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *DtbObj_getSmilTree(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilTree* _rv = _self->ob_itself->getSmilTree();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilTreeObj_New, _rv);
	return _res;
}

static PyObject *DtbObj_searchFullTextNext(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->searchFullTextNext();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *DtbObj_searchFullTextPrevious(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->searchFullTextPrevious();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *DtbObj_calculateUriOfData(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::smil::SmilMediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      SmilMediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	ambulant::net::url _rv = _self->ob_itself->calculateUriOfData(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O",
	                     ambulant_url_New(_rv));
	return _res;
}

static PyObject *DtbObj_getDaisyVersion(DtbObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getDaisyVersion();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyMethodDef DtbObj_methods[] = {
	{"open_1", (PyCFunction)DtbObj_open_1, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1, ambulant::net::url* _arg2) -> (bool _rv)")},
	{"open_2", (PyCFunction)DtbObj_open_2, 1,
	 PyDoc_STR("(std::string _arg1, std::string _arg2) -> (bool _rv)")},
	{"startReading", (PyCFunction)DtbObj_startReading, 1,
	 PyDoc_STR("(bool _arg1) -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"nextPhrase", (PyCFunction)DtbObj_nextPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"previousPhrase", (PyCFunction)DtbObj_previousPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"loadSmilFromUrl", (PyCFunction)DtbObj_loadSmilFromUrl, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"getMetadata", (PyCFunction)DtbObj_getMetadata, 1,
	 PyDoc_STR("() -> (amis::dtb::Metadata* _rv)")},
	{"getSpine", (PyCFunction)DtbObj_getSpine, 1,
	 PyDoc_STR("() -> (amis::dtb::Spine* _rv)")},
	{"getNavModel", (PyCFunction)DtbObj_getNavModel, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavModel* _rv)")},
	{"getFileSet", (PyCFunction)DtbObj_getFileSet, 1,
	 PyDoc_STR("() -> (amis::dtb::DtbFileSet* _rv)")},
	{"getUid", (PyCFunction)DtbObj_getUid, 1,
	 PyDoc_STR("() -> (std::wstring _rv)")},
	{"getBookmarks", (PyCFunction)DtbObj_getBookmarks, 1,
	 PyDoc_STR("() -> (amis::dtb::BookmarkSet* _rv)")},
	{"searchFullText", (PyCFunction)DtbObj_searchFullText, 1,
	 PyDoc_STR("(std::wstring _arg1) -> (std::string _rv)")},
	{"getSmilTree", (PyCFunction)DtbObj_getSmilTree, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilTree* _rv)")},
	{"searchFullTextNext", (PyCFunction)DtbObj_searchFullTextNext, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"searchFullTextPrevious", (PyCFunction)DtbObj_searchFullTextPrevious, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"calculateUriOfData", (PyCFunction)DtbObj_calculateUriOfData, 1,
	 PyDoc_STR("(amis::dtb::smil::SmilMediaGroup* _arg1) -> (ambulant::net::url _rv)")},
	{"getDaisyVersion", (PyCFunction)DtbObj_getDaisyVersion, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{NULL, NULL, 0}
};

#define DtbObj_getsetlist NULL


static int DtbObj_compare(DtbObject *self, DtbObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define DtbObj_repr NULL

static int DtbObj_hash(DtbObject *self)
{
	return (int)self->ob_itself;
}
static int DtbObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::Dtb* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((DtbObject *)_self)->ob_itself = new amis::dtb::Dtb();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, DtbObj_Convert, &itself))
	{
		((DtbObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define DtbObj_tp_alloc PyType_GenericAlloc

static PyObject *DtbObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((DtbObject *)_self)->ob_itself = NULL;
	return _self;
}

#define DtbObj_tp_free PyObject_Del


PyTypeObject Dtb_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Dtb", /*tp_name*/
	sizeof(DtbObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) DtbObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) DtbObj_compare, /*tp_compare*/
	(reprfunc) DtbObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) DtbObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	DtbObj_methods, /* tp_methods */
	0, /*tp_members*/
	DtbObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	DtbObj_tp_init, /* tp_init */
	DtbObj_tp_alloc, /* tp_alloc */
	DtbObj_tp_new, /* tp_new */
	DtbObj_tp_free, /* tp_free */
};

/* ---------------------- End object type Dtb ----------------------- */


/* --------------------- Object type DtbFileSet --------------------- */

extern PyTypeObject DtbFileSet_Type;

inline bool DtbFileSetObj_Check(PyObject *x)
{
	return ((x)->ob_type == &DtbFileSet_Type);
}

typedef struct DtbFileSetObject {
	PyObject_HEAD
	amis::dtb::DtbFileSet* ob_itself;
} DtbFileSetObject;

PyObject *DtbFileSetObj_New(amis::dtb::DtbFileSet* itself)
{
	DtbFileSetObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_DtbFileSet
	DtbFileSet *encaps_itself = dynamic_cast<DtbFileSet *>(itself);
	if (encaps_itself && encaps_itself->py_DtbFileSet)
	{
		Py_INCREF(encaps_itself->py_DtbFileSet);
		return encaps_itself->py_DtbFileSet;
	}
#endif
	it = PyObject_NEW(DtbFileSetObject, &DtbFileSet_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int DtbFileSetObj_Convert(PyObject *v, amis::dtb::DtbFileSet* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_DtbFileSet
	if (!DtbFileSetObj_Check(v))
	{
		*p_itself = Py_WrapAs_DtbFileSet(v);
		if (*p_itself) return 1;
	}
#endif
	if (!DtbFileSetObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "DtbFileSet required");
		return 0;
	}
	*p_itself = ((DtbFileSetObject *)v)->ob_itself;
	return 1;
}

static void DtbFileSetObj_dealloc(DtbFileSetObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *DtbFileSetObj_initWithNccOrOpf(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->initWithNccOrOpf(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *DtbFileSetObj_setBookmarksDirectory(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setBookmarksDirectory(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *DtbFileSetObj_setAdditionalDataAfterInitialParse(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::wstring _arg1;
	ambulant::net::url* _arg2;
	ambulant::net::url* _arg3;
	wchar_t *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "uO&O&",
	                      &_arg1_cstr,
	                      ambulant_url_Convert, &_arg2,
	                      ambulant_url_Convert, &_arg3))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setAdditionalDataAfterInitialParse(_arg1,
	                                                     _arg2,
	                                                     _arg3);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *DtbFileSetObj_getBookDirectory(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getBookDirectory();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_getNavFilepath(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getNavFilepath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_getOpfFilepath(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getOpfFilepath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_getResourceFilepath(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getResourceFilepath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_getBookmarksFilepath(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getBookmarksFilepath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_setSmilFile(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setSmilFile(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *DtbFileSetObj_getSmilFile(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getSmilFile();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *DtbFileSetObj_setTextFile(DtbFileSetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setTextFile(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef DtbFileSetObj_methods[] = {
	{"initWithNccOrOpf", (PyCFunction)DtbFileSetObj_initWithNccOrOpf, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> None")},
	{"setBookmarksDirectory", (PyCFunction)DtbFileSetObj_setBookmarksDirectory, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> None")},
	{"setAdditionalDataAfterInitialParse", (PyCFunction)DtbFileSetObj_setAdditionalDataAfterInitialParse, 1,
	 PyDoc_STR("(std::wstring _arg1, ambulant::net::url* _arg2, ambulant::net::url* _arg3) -> None")},
	{"getBookDirectory", (PyCFunction)DtbFileSetObj_getBookDirectory, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getNavFilepath", (PyCFunction)DtbFileSetObj_getNavFilepath, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getOpfFilepath", (PyCFunction)DtbFileSetObj_getOpfFilepath, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getResourceFilepath", (PyCFunction)DtbFileSetObj_getResourceFilepath, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getBookmarksFilepath", (PyCFunction)DtbFileSetObj_getBookmarksFilepath, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"setSmilFile", (PyCFunction)DtbFileSetObj_setSmilFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> None")},
	{"getSmilFile", (PyCFunction)DtbFileSetObj_getSmilFile, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"setTextFile", (PyCFunction)DtbFileSetObj_setTextFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> None")},
	{NULL, NULL, 0}
};

#define DtbFileSetObj_getsetlist NULL


static int DtbFileSetObj_compare(DtbFileSetObject *self, DtbFileSetObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define DtbFileSetObj_repr NULL

static int DtbFileSetObj_hash(DtbFileSetObject *self)
{
	return (int)self->ob_itself;
}
static int DtbFileSetObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::DtbFileSet* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((DtbFileSetObject *)_self)->ob_itself = new amis::dtb::DtbFileSet();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, DtbFileSetObj_Convert, &itself))
	{
		((DtbFileSetObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define DtbFileSetObj_tp_alloc PyType_GenericAlloc

static PyObject *DtbFileSetObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((DtbFileSetObject *)_self)->ob_itself = NULL;
	return _self;
}

#define DtbFileSetObj_tp_free PyObject_Del


PyTypeObject DtbFileSet_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.DtbFileSet", /*tp_name*/
	sizeof(DtbFileSetObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) DtbFileSetObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) DtbFileSetObj_compare, /*tp_compare*/
	(reprfunc) DtbFileSetObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) DtbFileSetObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	DtbFileSetObj_methods, /* tp_methods */
	0, /*tp_members*/
	DtbFileSetObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	DtbFileSetObj_tp_init, /* tp_init */
	DtbFileSetObj_tp_alloc, /* tp_alloc */
	DtbFileSetObj_tp_new, /* tp_new */
	DtbFileSetObj_tp_free, /* tp_free */
};

/* ------------------- End object type DtbFileSet ------------------- */


/* ---------------------- Object type MetaItem ---------------------- */

extern PyTypeObject MetaItem_Type;

inline bool MetaItemObj_Check(PyObject *x)
{
	return ((x)->ob_type == &MetaItem_Type);
}

typedef struct MetaItemObject {
	PyObject_HEAD
	amis::dtb::MetaItem* ob_itself;
} MetaItemObject;

PyObject *MetaItemObj_New(amis::dtb::MetaItem* itself)
{
	MetaItemObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_MetaItem
	MetaItem *encaps_itself = dynamic_cast<MetaItem *>(itself);
	if (encaps_itself && encaps_itself->py_MetaItem)
	{
		Py_INCREF(encaps_itself->py_MetaItem);
		return encaps_itself->py_MetaItem;
	}
#endif
	it = PyObject_NEW(MetaItemObject, &MetaItem_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int MetaItemObj_Convert(PyObject *v, amis::dtb::MetaItem* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_MetaItem
	if (!MetaItemObj_Check(v))
	{
		*p_itself = Py_WrapAs_MetaItem(v);
		if (*p_itself) return 1;
	}
#endif
	if (!MetaItemObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "MetaItem required");
		return 0;
	}
	*p_itself = ((MetaItemObject *)v)->ob_itself;
	return 1;
}

static void MetaItemObj_dealloc(MetaItemObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyMethodDef MetaItemObj_methods[] = {
	{NULL, NULL, 0}
};

#define MetaItemObj_getsetlist NULL


static int MetaItemObj_compare(MetaItemObject *self, MetaItemObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define MetaItemObj_repr NULL

static int MetaItemObj_hash(MetaItemObject *self)
{
	return (int)self->ob_itself;
}
static int MetaItemObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::MetaItem* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((MetaItemObject *)_self)->ob_itself = new amis::dtb::MetaItem();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, MetaItemObj_Convert, &itself))
	{
		((MetaItemObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define MetaItemObj_tp_alloc PyType_GenericAlloc

static PyObject *MetaItemObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((MetaItemObject *)_self)->ob_itself = NULL;
	return _self;
}

#define MetaItemObj_tp_free PyObject_Del


PyTypeObject MetaItem_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.MetaItem", /*tp_name*/
	sizeof(MetaItemObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) MetaItemObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) MetaItemObj_compare, /*tp_compare*/
	(reprfunc) MetaItemObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) MetaItemObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	MetaItemObj_methods, /* tp_methods */
	0, /*tp_members*/
	MetaItemObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	MetaItemObj_tp_init, /* tp_init */
	MetaItemObj_tp_alloc, /* tp_alloc */
	MetaItemObj_tp_new, /* tp_new */
	MetaItemObj_tp_free, /* tp_free */
};

/* -------------------- End object type MetaItem -------------------- */


/* ---------------------- Object type Metadata ---------------------- */

extern PyTypeObject Metadata_Type;

inline bool MetadataObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Metadata_Type);
}

typedef struct MetadataObject {
	PyObject_HEAD
	amis::dtb::Metadata* ob_itself;
} MetadataObject;

PyObject *MetadataObj_New(amis::dtb::Metadata* itself)
{
	MetadataObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Metadata
	Metadata *encaps_itself = dynamic_cast<Metadata *>(itself);
	if (encaps_itself && encaps_itself->py_Metadata)
	{
		Py_INCREF(encaps_itself->py_Metadata);
		return encaps_itself->py_Metadata;
	}
#endif
	it = PyObject_NEW(MetadataObject, &Metadata_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int MetadataObj_Convert(PyObject *v, amis::dtb::Metadata* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Metadata
	if (!MetadataObj_Check(v))
	{
		*p_itself = Py_WrapAs_Metadata(v);
		if (*p_itself) return 1;
	}
#endif
	if (!MetadataObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Metadata required");
		return 0;
	}
	*p_itself = ((MetadataObject *)v)->ob_itself;
	return 1;
}

static void MetadataObj_dealloc(MetadataObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *MetadataObj_getMetadata(MetadataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	std::wstring _rv = _self->ob_itself->getMetadata(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("u",
	                     _rv.c_str());
	return _res;
}

static PyObject *MetadataObj_addMetadata(MetadataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::MetaItem* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MetaItemObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addMetadata(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *MetadataObj_getLastAdded(MetadataObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::MetaItem* _rv = _self->ob_itself->getLastAdded();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MetaItemObj_New, _rv);
	return _res;
}

static PyMethodDef MetadataObj_methods[] = {
	{"getMetadata", (PyCFunction)MetadataObj_getMetadata, 1,
	 PyDoc_STR("(std::string _arg1) -> (std::wstring _rv)")},
	{"addMetadata", (PyCFunction)MetadataObj_addMetadata, 1,
	 PyDoc_STR("(amis::dtb::MetaItem* _arg1) -> None")},
	{"getLastAdded", (PyCFunction)MetadataObj_getLastAdded, 1,
	 PyDoc_STR("() -> (amis::dtb::MetaItem* _rv)")},
	{NULL, NULL, 0}
};

#define MetadataObj_getsetlist NULL


static int MetadataObj_compare(MetadataObject *self, MetadataObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define MetadataObj_repr NULL

static int MetadataObj_hash(MetadataObject *self)
{
	return (int)self->ob_itself;
}
static int MetadataObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::Metadata* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((MetadataObject *)_self)->ob_itself = new amis::dtb::Metadata();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, MetadataObj_Convert, &itself))
	{
		((MetadataObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define MetadataObj_tp_alloc PyType_GenericAlloc

static PyObject *MetadataObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((MetadataObject *)_self)->ob_itself = NULL;
	return _self;
}

#define MetadataObj_tp_free PyObject_Del


PyTypeObject Metadata_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Metadata", /*tp_name*/
	sizeof(MetadataObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) MetadataObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) MetadataObj_compare, /*tp_compare*/
	(reprfunc) MetadataObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) MetadataObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	MetadataObj_methods, /* tp_methods */
	0, /*tp_members*/
	MetadataObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	MetadataObj_tp_init, /* tp_init */
	MetadataObj_tp_alloc, /* tp_alloc */
	MetadataObj_tp_new, /* tp_new */
	MetadataObj_tp_free, /* tp_free */
};

/* -------------------- End object type Metadata -------------------- */


/* ----------------------- Object type Spine ------------------------ */

extern PyTypeObject Spine_Type;

inline bool SpineObj_Check(PyObject *x)
{
	return ((x)->ob_type == &Spine_Type);
}

typedef struct SpineObject {
	PyObject_HEAD
	amis::dtb::Spine* ob_itself;
} SpineObject;

PyObject *SpineObj_New(amis::dtb::Spine* itself)
{
	SpineObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_Spine
	Spine *encaps_itself = dynamic_cast<Spine *>(itself);
	if (encaps_itself && encaps_itself->py_Spine)
	{
		Py_INCREF(encaps_itself->py_Spine);
		return encaps_itself->py_Spine;
	}
#endif
	it = PyObject_NEW(SpineObject, &Spine_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int SpineObj_Convert(PyObject *v, amis::dtb::Spine* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_Spine
	if (!SpineObj_Check(v))
	{
		*p_itself = Py_WrapAs_Spine(v);
		if (*p_itself) return 1;
	}
#endif
	if (!SpineObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "Spine required");
		return 0;
	}
	*p_itself = ((SpineObject *)v)->ob_itself;
	return 1;
}

static void SpineObj_dealloc(SpineObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *SpineObj_addFile_1(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	bool _arg2;
	if (!PyArg_ParseTuple(_args, "O&O&",
	                      ambulant_url_Convert, &_arg1,
	                      bool_Convert, &_arg2))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addFile(_arg1,
	                          _arg2);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SpineObj_addFile_2(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	bool _arg2;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "sO&",
	                      &_arg1_cstr,
	                      bool_Convert, &_arg2))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addFile(_arg1,
	                          _arg2);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SpineObj_isFilePresent(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->isFilePresent(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *SpineObj_isEmpty(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->isEmpty();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *SpineObj_getNextFile(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getNextFile();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *SpineObj_getPreviousFile(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getPreviousFile();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *SpineObj_getFirstFile(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getFirstFile();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *SpineObj_getLastFile(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getLastFile();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyObject *SpineObj_goToFile(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->goToFile(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *SpineObj_getNumberOfSmilFiles(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getNumberOfSmilFiles();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *SpineObj_getSmilFilePath(SpineObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	const ambulant::net::url* _rv = _self->ob_itself->getSmilFilePath(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ambulant_url_New, _rv);
	return _res;
}

static PyMethodDef SpineObj_methods[] = {
	{"addFile_1", (PyCFunction)SpineObj_addFile_1, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1, bool _arg2) -> None")},
	{"addFile_2", (PyCFunction)SpineObj_addFile_2, 1,
	 PyDoc_STR("(std::string _arg1, bool _arg2) -> None")},
	{"isFilePresent", (PyCFunction)SpineObj_isFilePresent, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (bool _rv)")},
	{"isEmpty", (PyCFunction)SpineObj_isEmpty, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"getNextFile", (PyCFunction)SpineObj_getNextFile, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getPreviousFile", (PyCFunction)SpineObj_getPreviousFile, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getFirstFile", (PyCFunction)SpineObj_getFirstFile, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"getLastFile", (PyCFunction)SpineObj_getLastFile, 1,
	 PyDoc_STR("() -> (const ambulant::net::url* _rv)")},
	{"goToFile", (PyCFunction)SpineObj_goToFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (bool _rv)")},
	{"getNumberOfSmilFiles", (PyCFunction)SpineObj_getNumberOfSmilFiles, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getSmilFilePath", (PyCFunction)SpineObj_getSmilFilePath, 1,
	 PyDoc_STR("(unsigned int _arg1) -> (const ambulant::net::url* _rv)")},
	{NULL, NULL, 0}
};

#define SpineObj_getsetlist NULL


static int SpineObj_compare(SpineObject *self, SpineObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define SpineObj_repr NULL

static int SpineObj_hash(SpineObject *self)
{
	return (int)self->ob_itself;
}
static int SpineObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::Spine* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((SpineObject *)_self)->ob_itself = new amis::dtb::Spine();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, SpineObj_Convert, &itself))
	{
		((SpineObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define SpineObj_tp_alloc PyType_GenericAlloc

static PyObject *SpineObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((SpineObject *)_self)->ob_itself = NULL;
	return _self;
}

#define SpineObj_tp_free PyObject_Del


PyTypeObject Spine_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.Spine", /*tp_name*/
	sizeof(SpineObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) SpineObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) SpineObj_compare, /*tp_compare*/
	(reprfunc) SpineObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) SpineObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	SpineObj_methods, /* tp_methods */
	0, /*tp_members*/
	SpineObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	SpineObj_tp_init, /* tp_init */
	SpineObj_tp_alloc, /* tp_alloc */
	SpineObj_tp_new, /* tp_new */
	SpineObj_tp_free, /* tp_free */
};

/* --------------------- End object type Spine ---------------------- */


/* -------------------- Object type NavContainer -------------------- */

extern PyTypeObject NavContainer_Type;

inline bool NavContainerObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavContainer_Type);
}

typedef struct NavContainerObject {
	PyObject_HEAD
	amis::dtb::nav::NavContainer* ob_itself;
} NavContainerObject;

PyObject *NavContainerObj_New(amis::dtb::nav::NavContainer* itself)
{
	NavContainerObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavContainer
	NavContainer *encaps_itself = dynamic_cast<NavContainer *>(itself);
	if (encaps_itself && encaps_itself->py_NavContainer)
	{
		Py_INCREF(encaps_itself->py_NavContainer);
		return encaps_itself->py_NavContainer;
	}
#endif
	it = PyObject_NEW(NavContainerObject, &NavContainer_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavContainerObj_Convert(PyObject *v, amis::dtb::nav::NavContainer* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavContainer
	if (!NavContainerObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavContainer(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavContainerObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavContainer required");
		return 0;
	}
	*p_itself = ((NavContainerObject *)v)->ob_itself;
	return 1;
}

static void NavContainerObj_dealloc(NavContainerObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *NavContainerObj_goToPlayOrder(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToPlayOrder(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavContainerObj_goToContentRef(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToContentRef(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavContainerObj_goToId(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToId(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavContainerObj_setLabel(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLabel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavContainerObj_getLabel(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaGroup* _rv = _self->ob_itself->getLabel();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaGroupObj_New, _rv);
	return _res;
}

static PyObject *NavContainerObj_setNavInfo(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setNavInfo(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavContainerObj_getNavInfo(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaGroup* _rv = _self->ob_itself->getNavInfo();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaGroupObj_New, _rv);
	return _res;
}

static PyObject *NavContainerObj_setId(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavContainerObj_getId(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *NavContainerObj_setNavModel(NavContainerObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavModel* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavModelObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setNavModel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef NavContainerObj_methods[] = {
	{"goToPlayOrder", (PyCFunction)NavContainerObj_goToPlayOrder, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToContentRef", (PyCFunction)NavContainerObj_goToContentRef, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToId", (PyCFunction)NavContainerObj_goToId, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"setLabel", (PyCFunction)NavContainerObj_setLabel, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"getLabel", (PyCFunction)NavContainerObj_getLabel, 1,
	 PyDoc_STR("() -> (amis::MediaGroup* _rv)")},
	{"setNavInfo", (PyCFunction)NavContainerObj_setNavInfo, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"getNavInfo", (PyCFunction)NavContainerObj_getNavInfo, 1,
	 PyDoc_STR("() -> (amis::MediaGroup* _rv)")},
	{"setId", (PyCFunction)NavContainerObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getId", (PyCFunction)NavContainerObj_getId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setNavModel", (PyCFunction)NavContainerObj_setNavModel, 1,
	 PyDoc_STR("(amis::dtb::nav::NavModel* _arg1) -> None")},
	{NULL, NULL, 0}
};

#define NavContainerObj_getsetlist NULL


static int NavContainerObj_compare(NavContainerObject *self, NavContainerObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavContainerObj_repr NULL

static int NavContainerObj_hash(NavContainerObject *self)
{
	return (int)self->ob_itself;
}
static int NavContainerObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavContainer* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavContainerObj_Convert, &itself))
	{
		((NavContainerObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavContainerObj_tp_alloc PyType_GenericAlloc

static PyObject *NavContainerObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavContainerObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavContainerObj_tp_free PyObject_Del


PyTypeObject NavContainer_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavContainer", /*tp_name*/
	sizeof(NavContainerObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavContainerObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavContainerObj_compare, /*tp_compare*/
	(reprfunc) NavContainerObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavContainerObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavContainerObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavContainerObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavContainerObj_tp_init, /* tp_init */
	NavContainerObj_tp_alloc, /* tp_alloc */
	NavContainerObj_tp_new, /* tp_new */
	NavContainerObj_tp_free, /* tp_free */
};

/* ------------------ End object type NavContainer ------------------ */


/* ---------------------- Object type NavList ----------------------- */

extern PyTypeObject NavList_Type;

inline bool NavListObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavList_Type);
}

typedef struct NavListObject {
	PyObject_HEAD
	amis::dtb::nav::NavList* ob_itself;
} NavListObject;

PyObject *NavListObj_New(amis::dtb::nav::NavList* itself)
{
	NavListObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavList
	NavList *encaps_itself = dynamic_cast<NavList *>(itself);
	if (encaps_itself && encaps_itself->py_NavList)
	{
		Py_INCREF(encaps_itself->py_NavList);
		return encaps_itself->py_NavList;
	}
#endif
	it = PyObject_NEW(NavListObject, &NavList_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavListObj_Convert(PyObject *v, amis::dtb::nav::NavList* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavList
	if (!NavListObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavList(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavListObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavList required");
		return 0;
	}
	*p_itself = ((NavListObject *)v)->ob_itself;
	return 1;
}

static void NavListObj_dealloc(NavListObject *self)
{
	NavContainer_Type.tp_dealloc((PyObject *)self);
}

static PyObject *NavListObj_addNode(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavTarget* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavTargetObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addNode(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavListObj_getLength(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getLength();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavListObj_getAtIndex(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavTarget* _rv = _self->ob_itself->getAtIndex(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavTargetObj_New, _rv);
	return _res;
}

static PyObject *NavListObj_goToContentRef(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToContentRef(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavListObj_goToId(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToId(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavListObj_goToPlayOrder(NavListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToPlayOrder(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyMethodDef NavListObj_methods[] = {
	{"addNode", (PyCFunction)NavListObj_addNode, 1,
	 PyDoc_STR("(amis::dtb::nav::NavTarget* _arg1) -> None")},
	{"getLength", (PyCFunction)NavListObj_getLength, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getAtIndex", (PyCFunction)NavListObj_getAtIndex, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::nav::NavTarget* _rv)")},
	{"goToContentRef", (PyCFunction)NavListObj_goToContentRef, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToId", (PyCFunction)NavListObj_goToId, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToPlayOrder", (PyCFunction)NavListObj_goToPlayOrder, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{NULL, NULL, 0}
};

#define NavListObj_getsetlist NULL


static int NavListObj_compare(NavListObject *self, NavListObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavListObj_repr NULL

static int NavListObj_hash(NavListObject *self)
{
	return (int)self->ob_itself;
}
static int NavListObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavList* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((NavListObject *)_self)->ob_itself = new amis::dtb::nav::NavList();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavListObj_Convert, &itself))
	{
		((NavListObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavListObj_tp_alloc PyType_GenericAlloc

static PyObject *NavListObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavListObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavListObj_tp_free PyObject_Del


PyTypeObject NavList_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavList", /*tp_name*/
	sizeof(NavListObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavListObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavListObj_compare, /*tp_compare*/
	(reprfunc) NavListObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavListObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavListObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavListObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavListObj_tp_init, /* tp_init */
	NavListObj_tp_alloc, /* tp_alloc */
	NavListObj_tp_new, /* tp_new */
	NavListObj_tp_free, /* tp_free */
};

/* -------------------- End object type NavList --------------------- */


/* ----------------------- Object type NavMap ----------------------- */

extern PyTypeObject NavMap_Type;

inline bool NavMapObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavMap_Type);
}

typedef struct NavMapObject {
	PyObject_HEAD
	amis::dtb::nav::NavMap* ob_itself;
} NavMapObject;

PyObject *NavMapObj_New(amis::dtb::nav::NavMap* itself)
{
	NavMapObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavMap
	NavMap *encaps_itself = dynamic_cast<NavMap *>(itself);
	if (encaps_itself && encaps_itself->py_NavMap)
	{
		Py_INCREF(encaps_itself->py_NavMap);
		return encaps_itself->py_NavMap;
	}
#endif
	it = PyObject_NEW(NavMapObject, &NavMap_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavMapObj_Convert(PyObject *v, amis::dtb::nav::NavMap* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavMap
	if (!NavMapObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavMap(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavMapObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavMap required");
		return 0;
	}
	*p_itself = ((NavMapObject *)v)->ob_itself;
	return 1;
}

static void NavMapObj_dealloc(NavMapObject *self)
{
	NavContainer_Type.tp_dealloc((PyObject *)self);
}

static PyObject *NavMapObj_getRoot(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->getRoot();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavMapObj_printNode(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavPoint* _arg1;
	int _arg2;
	if (!PyArg_ParseTuple(_args, "O&i",
	                      NavPointObj_Convert, &_arg1,
	                      &_arg2))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->printNode(_arg1,
	                            _arg2);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavMapObj_isEmpty(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->isEmpty();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *NavMapObj_goToPlayOrder(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToPlayOrder(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavMapObj_goToContentRef(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToContentRef(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavMapObj_goToId(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->goToId(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavMapObj_getMaxDepth(NavMapObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getMaxDepth();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyMethodDef NavMapObj_methods[] = {
	{"getRoot", (PyCFunction)NavMapObj_getRoot, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavPoint* _rv)")},
	{"printNode", (PyCFunction)NavMapObj_printNode, 1,
	 PyDoc_STR("(amis::dtb::nav::NavPoint* _arg1, int _arg2) -> None")},
	{"isEmpty", (PyCFunction)NavMapObj_isEmpty, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"goToPlayOrder", (PyCFunction)NavMapObj_goToPlayOrder, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToContentRef", (PyCFunction)NavMapObj_goToContentRef, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"goToId", (PyCFunction)NavMapObj_goToId, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavNode* _rv)")},
	{"getMaxDepth", (PyCFunction)NavMapObj_getMaxDepth, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{NULL, NULL, 0}
};

#define NavMapObj_getsetlist NULL


static int NavMapObj_compare(NavMapObject *self, NavMapObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavMapObj_repr NULL

static int NavMapObj_hash(NavMapObject *self)
{
	return (int)self->ob_itself;
}
static int NavMapObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavMap* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((NavMapObject *)_self)->ob_itself = new amis::dtb::nav::NavMap();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavMapObj_Convert, &itself))
	{
		((NavMapObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavMapObj_tp_alloc PyType_GenericAlloc

static PyObject *NavMapObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavMapObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavMapObj_tp_free PyObject_Del


PyTypeObject NavMap_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavMap", /*tp_name*/
	sizeof(NavMapObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavMapObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavMapObj_compare, /*tp_compare*/
	(reprfunc) NavMapObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavMapObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavMapObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavMapObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavMapObj_tp_init, /* tp_init */
	NavMapObj_tp_alloc, /* tp_alloc */
	NavMapObj_tp_new, /* tp_new */
	NavMapObj_tp_free, /* tp_free */
};

/* --------------------- End object type NavMap --------------------- */


/* ---------------------- Object type NavModel ---------------------- */

extern PyTypeObject NavModel_Type;

inline bool NavModelObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavModel_Type);
}

typedef struct NavModelObject {
	PyObject_HEAD
	amis::dtb::nav::NavModel* ob_itself;
} NavModelObject;

PyObject *NavModelObj_New(amis::dtb::nav::NavModel* itself)
{
	NavModelObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavModel
	NavModel *encaps_itself = dynamic_cast<NavModel *>(itself);
	if (encaps_itself && encaps_itself->py_NavModel)
	{
		Py_INCREF(encaps_itself->py_NavModel);
		return encaps_itself->py_NavModel;
	}
#endif
	it = PyObject_NEW(NavModelObject, &NavModel_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavModelObj_Convert(PyObject *v, amis::dtb::nav::NavModel* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavModel
	if (!NavModelObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavModel(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavModelObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavModel required");
		return 0;
	}
	*p_itself = ((NavModelObject *)v)->ob_itself;
	return 1;
}

static void NavModelObj_dealloc(NavModelObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *NavModelObj_getNumberOfNavLists(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	unsigned int _rv = _self->ob_itself->getNumberOfNavLists();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *NavModelObj_getNavList_1(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	unsigned int _arg1;
	if (!PyArg_ParseTuple(_args, "l",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavList* _rv = _self->ob_itself->getNavList(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavListObj_New, _rv);
	return _res;
}

static PyObject *NavModelObj_getNavList_2(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavList* _rv = _self->ob_itself->getNavList(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavListObj_New, _rv);
	return _res;
}

static PyObject *NavModelObj_addNavList(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->addNavList(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavModelObj_hasPages(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->hasPages();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *NavModelObj_getPageList(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::PageList* _rv = _self->ob_itself->getPageList();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     PageListObj_New, _rv);
	return _res;
}

static PyObject *NavModelObj_getNavMap(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavMap* _rv = _self->ob_itself->getNavMap();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavMapObj_New, _rv);
	return _res;
}

static PyObject *NavModelObj_getNodeForSmilId(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	amis::dtb::nav::NavContainer* _arg2;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "sO&",
	                      &_arg1_cstr,
	                      NavContainerObj_Convert, &_arg2))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode* _rv = _self->ob_itself->getNodeForSmilId(_arg1,
	                                                                  _arg2);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavNodeObj_New, _rv);
	return _res;
}

static PyObject *NavModelObj_testMap(NavModelObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->testMap();
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef NavModelObj_methods[] = {
	{"getNumberOfNavLists", (PyCFunction)NavModelObj_getNumberOfNavLists, 1,
	 PyDoc_STR("() -> (unsigned int _rv)")},
	{"getNavList_1", (PyCFunction)NavModelObj_getNavList_1, 1,
	 PyDoc_STR("(unsigned int _arg1) -> (amis::dtb::nav::NavList* _rv)")},
	{"getNavList_2", (PyCFunction)NavModelObj_getNavList_2, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::nav::NavList* _rv)")},
	{"addNavList", (PyCFunction)NavModelObj_addNavList, 1,
	 PyDoc_STR("(std::string _arg1) -> (int _rv)")},
	{"hasPages", (PyCFunction)NavModelObj_hasPages, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"getPageList", (PyCFunction)NavModelObj_getPageList, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::PageList* _rv)")},
	{"getNavMap", (PyCFunction)NavModelObj_getNavMap, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavMap* _rv)")},
	{"getNodeForSmilId", (PyCFunction)NavModelObj_getNodeForSmilId, 1,
	 PyDoc_STR("(std::string _arg1, amis::dtb::nav::NavContainer* _arg2) -> (amis::dtb::nav::NavNode* _rv)")},
	{"testMap", (PyCFunction)NavModelObj_testMap, 1,
	 PyDoc_STR("() -> None")},
	{NULL, NULL, 0}
};

#define NavModelObj_getsetlist NULL


static int NavModelObj_compare(NavModelObject *self, NavModelObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavModelObj_repr NULL

static int NavModelObj_hash(NavModelObject *self)
{
	return (int)self->ob_itself;
}
static int NavModelObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavModel* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((NavModelObject *)_self)->ob_itself = new amis::dtb::nav::NavModel();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavModelObj_Convert, &itself))
	{
		((NavModelObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavModelObj_tp_alloc PyType_GenericAlloc

static PyObject *NavModelObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavModelObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavModelObj_tp_free PyObject_Del


PyTypeObject NavModel_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavModel", /*tp_name*/
	sizeof(NavModelObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavModelObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavModelObj_compare, /*tp_compare*/
	(reprfunc) NavModelObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavModelObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavModelObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavModelObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavModelObj_tp_init, /* tp_init */
	NavModelObj_tp_alloc, /* tp_alloc */
	NavModelObj_tp_new, /* tp_new */
	NavModelObj_tp_free, /* tp_free */
};

/* -------------------- End object type NavModel -------------------- */


/* ---------------------- Object type NavNode ----------------------- */

extern PyTypeObject NavNode_Type;

inline bool NavNodeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavNode_Type);
}

typedef struct NavNodeObject {
	PyObject_HEAD
	amis::dtb::nav::NavNode* ob_itself;
} NavNodeObject;

PyObject *NavNodeObj_New(amis::dtb::nav::NavNode* itself)
{
	NavNodeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavNode
	NavNode *encaps_itself = dynamic_cast<NavNode *>(itself);
	if (encaps_itself && encaps_itself->py_NavNode)
	{
		Py_INCREF(encaps_itself->py_NavNode);
		return encaps_itself->py_NavNode;
	}
#endif
	it = PyObject_NEW(NavNodeObject, &NavNode_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavNodeObj_Convert(PyObject *v, amis::dtb::nav::NavNode* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavNode
	if (!NavNodeObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavNode(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavNodeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavNode required");
		return 0;
	}
	*p_itself = ((NavNodeObject *)v)->ob_itself;
	return 1;
}

static void NavNodeObj_dealloc(NavNodeObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *NavNodeObj_getTypeOfNode(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavNode::TypeOfNode _rv = _self->ob_itself->getTypeOfNode();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("l",
	                     _rv);
	return _res;
}

static PyObject *NavNodeObj_getLabel(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::MediaGroup* _rv = _self->ob_itself->getLabel();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     MediaGroupObj_New, _rv);
	return _res;
}

static PyObject *NavNodeObj_getContent(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getContent();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *NavNodeObj_getId(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *NavNodeObj_getPlayOrder(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getPlayOrder();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavNodeObj_getClass(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getClass();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *NavNodeObj_setLabel(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::MediaGroup* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      MediaGroupObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLabel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_setContent(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setContent(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_setId(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_setPlayOrder(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setPlayOrder(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_setClass(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setClass(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_setNavModel(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavModel* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavModelObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setNavModel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavNodeObj_getNavContainer(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavContainer* _rv = _self->ob_itself->getNavContainer();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavContainerObj_New, _rv);
	return _res;
}

static PyObject *NavNodeObj_setNavContainer(NavNodeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavContainer* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavContainerObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setNavContainer(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef NavNodeObj_methods[] = {
	{"getTypeOfNode", (PyCFunction)NavNodeObj_getTypeOfNode, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavNode::TypeOfNode _rv)")},
	{"getLabel", (PyCFunction)NavNodeObj_getLabel, 1,
	 PyDoc_STR("() -> (amis::MediaGroup* _rv)")},
	{"getContent", (PyCFunction)NavNodeObj_getContent, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getId", (PyCFunction)NavNodeObj_getId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getPlayOrder", (PyCFunction)NavNodeObj_getPlayOrder, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getClass", (PyCFunction)NavNodeObj_getClass, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setLabel", (PyCFunction)NavNodeObj_setLabel, 1,
	 PyDoc_STR("(amis::MediaGroup* _arg1) -> None")},
	{"setContent", (PyCFunction)NavNodeObj_setContent, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setId", (PyCFunction)NavNodeObj_setId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setPlayOrder", (PyCFunction)NavNodeObj_setPlayOrder, 1,
	 PyDoc_STR("(int _arg1) -> None")},
	{"setClass", (PyCFunction)NavNodeObj_setClass, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"setNavModel", (PyCFunction)NavNodeObj_setNavModel, 1,
	 PyDoc_STR("(amis::dtb::nav::NavModel* _arg1) -> None")},
	{"getNavContainer", (PyCFunction)NavNodeObj_getNavContainer, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavContainer* _rv)")},
	{"setNavContainer", (PyCFunction)NavNodeObj_setNavContainer, 1,
	 PyDoc_STR("(amis::dtb::nav::NavContainer* _arg1) -> None")},
	{NULL, NULL, 0}
};

#define NavNodeObj_getsetlist NULL


static int NavNodeObj_compare(NavNodeObject *self, NavNodeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavNodeObj_repr NULL

static int NavNodeObj_hash(NavNodeObject *self)
{
	return (int)self->ob_itself;
}
static int NavNodeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavNode* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavNodeObj_Convert, &itself))
	{
		((NavNodeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavNodeObj_tp_alloc PyType_GenericAlloc

static PyObject *NavNodeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavNodeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavNodeObj_tp_free PyObject_Del


PyTypeObject NavNode_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavNode", /*tp_name*/
	sizeof(NavNodeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavNodeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavNodeObj_compare, /*tp_compare*/
	(reprfunc) NavNodeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavNodeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavNodeObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavNodeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavNodeObj_tp_init, /* tp_init */
	NavNodeObj_tp_alloc, /* tp_alloc */
	NavNodeObj_tp_new, /* tp_new */
	NavNodeObj_tp_free, /* tp_free */
};

/* -------------------- End object type NavNode --------------------- */


/* ---------------------- Object type NavPoint ---------------------- */

extern PyTypeObject NavPoint_Type;

inline bool NavPointObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavPoint_Type);
}

typedef struct NavPointObject {
	PyObject_HEAD
	amis::dtb::nav::NavPoint* ob_itself;
} NavPointObject;

PyObject *NavPointObj_New(amis::dtb::nav::NavPoint* itself)
{
	NavPointObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavPoint
	NavPoint *encaps_itself = dynamic_cast<NavPoint *>(itself);
	if (encaps_itself && encaps_itself->py_NavPoint)
	{
		Py_INCREF(encaps_itself->py_NavPoint);
		return encaps_itself->py_NavPoint;
	}
#endif
	it = PyObject_NEW(NavPointObject, &NavPoint_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavPointObj_Convert(PyObject *v, amis::dtb::nav::NavPoint* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavPoint
	if (!NavPointObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavPoint(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavPointObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavPoint required");
		return 0;
	}
	*p_itself = ((NavPointObject *)v)->ob_itself;
	return 1;
}

static void NavPointObj_dealloc(NavPointObject *self)
{
	NavNode_Type.tp_dealloc((PyObject *)self);
}

static PyObject *NavPointObj_getFirstSibling(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->getFirstSibling();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavPointObj_getChild(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->getChild(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavPointObj_getLevel(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getLevel();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavPointObj_getParent(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->getParent();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavPointObj_next(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->next();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavPointObj_previous(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::NavPoint* _rv = _self->ob_itself->previous();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     NavPointObj_New, _rv);
	return _res;
}

static PyObject *NavPointObj_resetChildCount(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->resetChildCount();
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavPointObj_getNumChildren(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getNumChildren();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavPointObj_addChild(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavPoint* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavPointObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addChild(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavPointObj_setParent(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavPoint* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavPointObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setParent(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *NavPointObj_setLevel(NavPointObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setLevel(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef NavPointObj_methods[] = {
	{"getFirstSibling", (PyCFunction)NavPointObj_getFirstSibling, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavPoint* _rv)")},
	{"getChild", (PyCFunction)NavPointObj_getChild, 1,
	 PyDoc_STR("(int _arg1) -> (amis::dtb::nav::NavPoint* _rv)")},
	{"getLevel", (PyCFunction)NavPointObj_getLevel, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getParent", (PyCFunction)NavPointObj_getParent, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavPoint* _rv)")},
	{"next", (PyCFunction)NavPointObj_next, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavPoint* _rv)")},
	{"previous", (PyCFunction)NavPointObj_previous, 1,
	 PyDoc_STR("() -> (amis::dtb::nav::NavPoint* _rv)")},
	{"resetChildCount", (PyCFunction)NavPointObj_resetChildCount, 1,
	 PyDoc_STR("() -> None")},
	{"getNumChildren", (PyCFunction)NavPointObj_getNumChildren, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"addChild", (PyCFunction)NavPointObj_addChild, 1,
	 PyDoc_STR("(amis::dtb::nav::NavPoint* _arg1) -> None")},
	{"setParent", (PyCFunction)NavPointObj_setParent, 1,
	 PyDoc_STR("(amis::dtb::nav::NavPoint* _arg1) -> None")},
	{"setLevel", (PyCFunction)NavPointObj_setLevel, 1,
	 PyDoc_STR("(int _arg1) -> None")},
	{NULL, NULL, 0}
};

#define NavPointObj_getsetlist NULL


static int NavPointObj_compare(NavPointObject *self, NavPointObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavPointObj_repr NULL

static int NavPointObj_hash(NavPointObject *self)
{
	return (int)self->ob_itself;
}
static int NavPointObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavPoint* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((NavPointObject *)_self)->ob_itself = new amis::dtb::nav::NavPoint();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavPointObj_Convert, &itself))
	{
		((NavPointObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavPointObj_tp_alloc PyType_GenericAlloc

static PyObject *NavPointObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavPointObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavPointObj_tp_free PyObject_Del


PyTypeObject NavPoint_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavPoint", /*tp_name*/
	sizeof(NavPointObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavPointObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavPointObj_compare, /*tp_compare*/
	(reprfunc) NavPointObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavPointObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavPointObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavPointObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavPointObj_tp_init, /* tp_init */
	NavPointObj_tp_alloc, /* tp_alloc */
	NavPointObj_tp_new, /* tp_new */
	NavPointObj_tp_free, /* tp_free */
};

/* -------------------- End object type NavPoint -------------------- */


/* --------------------- Object type NavTarget ---------------------- */

extern PyTypeObject NavTarget_Type;

inline bool NavTargetObj_Check(PyObject *x)
{
	return ((x)->ob_type == &NavTarget_Type);
}

typedef struct NavTargetObject {
	PyObject_HEAD
	amis::dtb::nav::NavTarget* ob_itself;
} NavTargetObject;

PyObject *NavTargetObj_New(amis::dtb::nav::NavTarget* itself)
{
	NavTargetObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_NavTarget
	NavTarget *encaps_itself = dynamic_cast<NavTarget *>(itself);
	if (encaps_itself && encaps_itself->py_NavTarget)
	{
		Py_INCREF(encaps_itself->py_NavTarget);
		return encaps_itself->py_NavTarget;
	}
#endif
	it = PyObject_NEW(NavTargetObject, &NavTarget_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int NavTargetObj_Convert(PyObject *v, amis::dtb::nav::NavTarget* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_NavTarget
	if (!NavTargetObj_Check(v))
	{
		*p_itself = Py_WrapAs_NavTarget(v);
		if (*p_itself) return 1;
	}
#endif
	if (!NavTargetObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "NavTarget required");
		return 0;
	}
	*p_itself = ((NavTargetObject *)v)->ob_itself;
	return 1;
}

static void NavTargetObj_dealloc(NavTargetObject *self)
{
	NavNode_Type.tp_dealloc((PyObject *)self);
}

static PyObject *NavTargetObj_getIndex(NavTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getIndex();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *NavTargetObj_setIndex(NavTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setIndex(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef NavTargetObj_methods[] = {
	{"getIndex", (PyCFunction)NavTargetObj_getIndex, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"setIndex", (PyCFunction)NavTargetObj_setIndex, 1,
	 PyDoc_STR("(int _arg1) -> None")},
	{NULL, NULL, 0}
};

#define NavTargetObj_getsetlist NULL


static int NavTargetObj_compare(NavTargetObject *self, NavTargetObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define NavTargetObj_repr NULL

static int NavTargetObj_hash(NavTargetObject *self)
{
	return (int)self->ob_itself;
}
static int NavTargetObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::NavTarget* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((NavTargetObject *)_self)->ob_itself = new amis::dtb::nav::NavTarget();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, NavTargetObj_Convert, &itself))
	{
		((NavTargetObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define NavTargetObj_tp_alloc PyType_GenericAlloc

static PyObject *NavTargetObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((NavTargetObject *)_self)->ob_itself = NULL;
	return _self;
}

#define NavTargetObj_tp_free PyObject_Del


PyTypeObject NavTarget_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.NavTarget", /*tp_name*/
	sizeof(NavTargetObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) NavTargetObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) NavTargetObj_compare, /*tp_compare*/
	(reprfunc) NavTargetObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) NavTargetObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	NavTargetObj_methods, /* tp_methods */
	0, /*tp_members*/
	NavTargetObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	NavTargetObj_tp_init, /* tp_init */
	NavTargetObj_tp_alloc, /* tp_alloc */
	NavTargetObj_tp_new, /* tp_new */
	NavTargetObj_tp_free, /* tp_free */
};

/* ------------------- End object type NavTarget -------------------- */


/* ---------------------- Object type PageList ---------------------- */

extern PyTypeObject PageList_Type;

inline bool PageListObj_Check(PyObject *x)
{
	return ((x)->ob_type == &PageList_Type);
}

typedef struct PageListObject {
	PyObject_HEAD
	amis::dtb::nav::PageList* ob_itself;
} PageListObject;

PyObject *PageListObj_New(amis::dtb::nav::PageList* itself)
{
	PageListObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_PageList
	PageList *encaps_itself = dynamic_cast<PageList *>(itself);
	if (encaps_itself && encaps_itself->py_PageList)
	{
		Py_INCREF(encaps_itself->py_PageList);
		return encaps_itself->py_PageList;
	}
#endif
	it = PyObject_NEW(PageListObject, &PageList_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int PageListObj_Convert(PyObject *v, amis::dtb::nav::PageList* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_PageList
	if (!PageListObj_Check(v))
	{
		*p_itself = Py_WrapAs_PageList(v);
		if (*p_itself) return 1;
	}
#endif
	if (!PageListObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "PageList required");
		return 0;
	}
	*p_itself = ((PageListObject *)v)->ob_itself;
	return 1;
}

static void PageListObj_dealloc(PageListObject *self)
{
	NavList_Type.tp_dealloc((PyObject *)self);
}

static PyObject *PageListObj_countPagesInRange(PageListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	int _arg2;
	if (!PyArg_ParseTuple(_args, "ii",
	                      &_arg1,
	                      &_arg2))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->countPagesInRange(_arg1,
	                                              _arg2);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *PageListObj_findPage(PageListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::wstring _arg1;
	wchar_t *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "u",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::nav::PageTarget* _rv = _self->ob_itself->findPage(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     PageTargetObj_New, _rv);
	return _res;
}

static PyObject *PageListObj_addNode(PageListObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::nav::NavTarget* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      NavTargetObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->addNode(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef PageListObj_methods[] = {
	{"countPagesInRange", (PyCFunction)PageListObj_countPagesInRange, 1,
	 PyDoc_STR("(int _arg1, int _arg2) -> (int _rv)")},
	{"findPage", (PyCFunction)PageListObj_findPage, 1,
	 PyDoc_STR("(std::wstring _arg1) -> (amis::dtb::nav::PageTarget* _rv)")},
	{"addNode", (PyCFunction)PageListObj_addNode, 1,
	 PyDoc_STR("(amis::dtb::nav::NavTarget* _arg1) -> None")},
	{NULL, NULL, 0}
};

#define PageListObj_getsetlist NULL


static int PageListObj_compare(PageListObject *self, PageListObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define PageListObj_repr NULL

static int PageListObj_hash(PageListObject *self)
{
	return (int)self->ob_itself;
}
static int PageListObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::PageList* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((PageListObject *)_self)->ob_itself = new amis::dtb::nav::PageList();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, PageListObj_Convert, &itself))
	{
		((PageListObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define PageListObj_tp_alloc PyType_GenericAlloc

static PyObject *PageListObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((PageListObject *)_self)->ob_itself = NULL;
	return _self;
}

#define PageListObj_tp_free PyObject_Del


PyTypeObject PageList_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.PageList", /*tp_name*/
	sizeof(PageListObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) PageListObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) PageListObj_compare, /*tp_compare*/
	(reprfunc) PageListObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) PageListObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	PageListObj_methods, /* tp_methods */
	0, /*tp_members*/
	PageListObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	PageListObj_tp_init, /* tp_init */
	PageListObj_tp_alloc, /* tp_alloc */
	PageListObj_tp_new, /* tp_new */
	PageListObj_tp_free, /* tp_free */
};

/* -------------------- End object type PageList -------------------- */


/* --------------------- Object type PageTarget --------------------- */

extern PyTypeObject PageTarget_Type;

inline bool PageTargetObj_Check(PyObject *x)
{
	return ((x)->ob_type == &PageTarget_Type);
}

typedef struct PageTargetObject {
	PyObject_HEAD
	amis::dtb::nav::PageTarget* ob_itself;
} PageTargetObject;

PyObject *PageTargetObj_New(amis::dtb::nav::PageTarget* itself)
{
	PageTargetObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_PageTarget
	PageTarget *encaps_itself = dynamic_cast<PageTarget *>(itself);
	if (encaps_itself && encaps_itself->py_PageTarget)
	{
		Py_INCREF(encaps_itself->py_PageTarget);
		return encaps_itself->py_PageTarget;
	}
#endif
	it = PyObject_NEW(PageTargetObject, &PageTarget_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int PageTargetObj_Convert(PyObject *v, amis::dtb::nav::PageTarget* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_PageTarget
	if (!PageTargetObj_Check(v))
	{
		*p_itself = Py_WrapAs_PageTarget(v);
		if (*p_itself) return 1;
	}
#endif
	if (!PageTargetObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "PageTarget required");
		return 0;
	}
	*p_itself = ((PageTargetObject *)v)->ob_itself;
	return 1;
}

static void PageTargetObj_dealloc(PageTargetObject *self)
{
	NavTarget_Type.tp_dealloc((PyObject *)self);
}

static PyObject *PageTargetObj_getType(PageTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getType();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *PageTargetObj_getIndex(PageTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	int _rv = _self->ob_itself->getIndex();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("i",
	                     _rv);
	return _res;
}

static PyObject *PageTargetObj_setIndex(PageTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setIndex(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *PageTargetObj_setType(PageTargetObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	int _arg1;
	if (!PyArg_ParseTuple(_args, "i",
	                      &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setType(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyMethodDef PageTargetObj_methods[] = {
	{"getType", (PyCFunction)PageTargetObj_getType, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"getIndex", (PyCFunction)PageTargetObj_getIndex, 1,
	 PyDoc_STR("() -> (int _rv)")},
	{"setIndex", (PyCFunction)PageTargetObj_setIndex, 1,
	 PyDoc_STR("(int _arg1) -> None")},
	{"setType", (PyCFunction)PageTargetObj_setType, 1,
	 PyDoc_STR("(int _arg1) -> None")},
	{NULL, NULL, 0}
};

#define PageTargetObj_getsetlist NULL


static int PageTargetObj_compare(PageTargetObject *self, PageTargetObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define PageTargetObj_repr NULL

static int PageTargetObj_hash(PageTargetObject *self)
{
	return (int)self->ob_itself;
}
static int PageTargetObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::nav::PageTarget* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((PageTargetObject *)_self)->ob_itself = new amis::dtb::nav::PageTarget();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, PageTargetObj_Convert, &itself))
	{
		((PageTargetObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define PageTargetObj_tp_alloc PyType_GenericAlloc

static PyObject *PageTargetObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((PageTargetObject *)_self)->ob_itself = NULL;
	return _self;
}

#define PageTargetObj_tp_free PyObject_Del


PyTypeObject PageTarget_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.PageTarget", /*tp_name*/
	sizeof(PageTargetObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) PageTargetObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) PageTargetObj_compare, /*tp_compare*/
	(reprfunc) PageTargetObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) PageTargetObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	PageTargetObj_methods, /* tp_methods */
	0, /*tp_members*/
	PageTargetObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	PageTargetObj_tp_init, /* tp_init */
	PageTargetObj_tp_alloc, /* tp_alloc */
	PageTargetObj_tp_new, /* tp_new */
	PageTargetObj_tp_free, /* tp_free */
};

/* ------------------- End object type PageTarget ------------------- */


/* ------------------- Object type SmilMediaGroup ------------------- */

extern PyTypeObject SmilMediaGroup_Type;

inline bool SmilMediaGroupObj_Check(PyObject *x)
{
	return ((x)->ob_type == &SmilMediaGroup_Type);
}

typedef struct SmilMediaGroupObject {
	PyObject_HEAD
	amis::dtb::smil::SmilMediaGroup* ob_itself;
} SmilMediaGroupObject;

PyObject *SmilMediaGroupObj_New(amis::dtb::smil::SmilMediaGroup* itself)
{
	SmilMediaGroupObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_SmilMediaGroup
	SmilMediaGroup *encaps_itself = dynamic_cast<SmilMediaGroup *>(itself);
	if (encaps_itself && encaps_itself->py_SmilMediaGroup)
	{
		Py_INCREF(encaps_itself->py_SmilMediaGroup);
		return encaps_itself->py_SmilMediaGroup;
	}
#endif
	it = PyObject_NEW(SmilMediaGroupObject, &SmilMediaGroup_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int SmilMediaGroupObj_Convert(PyObject *v, amis::dtb::smil::SmilMediaGroup* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_SmilMediaGroup
	if (!SmilMediaGroupObj_Check(v))
	{
		*p_itself = Py_WrapAs_SmilMediaGroup(v);
		if (*p_itself) return 1;
	}
#endif
	if (!SmilMediaGroupObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "SmilMediaGroup required");
		return 0;
	}
	*p_itself = ((SmilMediaGroupObject *)v)->ob_itself;
	return 1;
}

static void SmilMediaGroupObj_dealloc(SmilMediaGroupObject *self)
{
	MediaGroup_Type.tp_dealloc((PyObject *)self);
}

static PyObject *SmilMediaGroupObj_setEscape(SmilMediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setEscape(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilMediaGroupObj_getEscape(SmilMediaGroupObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->getEscape();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyMethodDef SmilMediaGroupObj_methods[] = {
	{"setEscape", (PyCFunction)SmilMediaGroupObj_setEscape, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{"getEscape", (PyCFunction)SmilMediaGroupObj_getEscape, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{NULL, NULL, 0}
};

#define SmilMediaGroupObj_getsetlist NULL


static int SmilMediaGroupObj_compare(SmilMediaGroupObject *self, SmilMediaGroupObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define SmilMediaGroupObj_repr NULL

static int SmilMediaGroupObj_hash(SmilMediaGroupObject *self)
{
	return (int)self->ob_itself;
}
static int SmilMediaGroupObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::smil::SmilMediaGroup* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((SmilMediaGroupObject *)_self)->ob_itself = new amis::dtb::smil::SmilMediaGroup();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, SmilMediaGroupObj_Convert, &itself))
	{
		((SmilMediaGroupObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define SmilMediaGroupObj_tp_alloc PyType_GenericAlloc

static PyObject *SmilMediaGroupObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((SmilMediaGroupObject *)_self)->ob_itself = NULL;
	return _self;
}

#define SmilMediaGroupObj_tp_free PyObject_Del


PyTypeObject SmilMediaGroup_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.SmilMediaGroup", /*tp_name*/
	sizeof(SmilMediaGroupObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) SmilMediaGroupObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) SmilMediaGroupObj_compare, /*tp_compare*/
	(reprfunc) SmilMediaGroupObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) SmilMediaGroupObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	SmilMediaGroupObj_methods, /* tp_methods */
	0, /*tp_members*/
	SmilMediaGroupObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	SmilMediaGroupObj_tp_init, /* tp_init */
	SmilMediaGroupObj_tp_alloc, /* tp_alloc */
	SmilMediaGroupObj_tp_new, /* tp_new */
	SmilMediaGroupObj_tp_free, /* tp_free */
};

/* ----------------- End object type SmilMediaGroup ----------------- */


/* ---------------------- Object type SmilTree ---------------------- */

extern PyTypeObject SmilTree_Type;

inline bool SmilTreeObj_Check(PyObject *x)
{
	return ((x)->ob_type == &SmilTree_Type);
}

typedef struct SmilTreeObject {
	PyObject_HEAD
	amis::dtb::smil::SmilTree* ob_itself;
} SmilTreeObject;

PyObject *SmilTreeObj_New(amis::dtb::smil::SmilTree* itself)
{
	SmilTreeObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_SmilTree
	SmilTree *encaps_itself = dynamic_cast<SmilTree *>(itself);
	if (encaps_itself && encaps_itself->py_SmilTree)
	{
		Py_INCREF(encaps_itself->py_SmilTree);
		return encaps_itself->py_SmilTree;
	}
#endif
	it = PyObject_NEW(SmilTreeObject, &SmilTree_Type);
	if (it == NULL) return NULL;
	/* XXXX Should we tp_init or tp_new our basetype? */
	it->ob_itself = itself;
	return (PyObject *)it;
}

int SmilTreeObj_Convert(PyObject *v, amis::dtb::smil::SmilTree* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_SmilTree
	if (!SmilTreeObj_Check(v))
	{
		*p_itself = Py_WrapAs_SmilTree(v);
		if (*p_itself) return 1;
	}
#endif
	if (!SmilTreeObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "SmilTree required");
		return 0;
	}
	*p_itself = ((SmilTreeObject *)v)->ob_itself;
	return 1;
}

static void SmilTreeObj_dealloc(SmilTreeObject *self)
{
	MediaGroup_Type.tp_dealloc((PyObject *)self);
}

static PyObject *SmilTreeObj_setSmilFilePath(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setSmilFilePath(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilTreeObj_getSmilFilePath(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getSmilFilePath();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *SmilTreeObj_getCurrentId(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getCurrentId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *SmilTreeObj_setCurrentId(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setCurrentId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilTreeObj_getRecentTextId(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	std::string _rv = _self->ob_itself->getRecentTextId();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("s",
	                     _rv.c_str());
	return _res;
}

static PyObject *SmilTreeObj_setRecentTextId(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setRecentTextId(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilTreeObj_getCouldEscape(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->getCouldEscape();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_setCouldEscape(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      bool_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setCouldEscape(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilTreeObj_setCustomTestSet(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	amis::dtb::CustomTestSet* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      CustomTestSetObj_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_self->ob_itself->setCustomTestSet(_arg1);
	PyEval_RestoreThread(_save);
	Py_INCREF(Py_None);
	_res = Py_None;
	return _res;
}

static PyObject *SmilTreeObj_firstPhrase(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->firstPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_lastPhrase(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->lastPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_nextPhrase(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->nextPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_previousPhrase(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->previousPhrase();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_goToId(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	std::string _arg1;
	char *_arg1_cstr;
	if (!PyArg_ParseTuple(_args, "s",
	                      &_arg1_cstr))
		return NULL;
	_arg1 = _arg1_cstr;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->goToId(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_escapeCurrentStructure(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::dtb::smil::SmilMediaGroup* _rv = _self->ob_itself->escapeCurrentStructure();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     SmilMediaGroupObj_New, _rv);
	return _res;
}

static PyObject *SmilTreeObj_isEmpty(SmilTreeObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->isEmpty();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyMethodDef SmilTreeObj_methods[] = {
	{"setSmilFilePath", (PyCFunction)SmilTreeObj_setSmilFilePath, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getSmilFilePath", (PyCFunction)SmilTreeObj_getSmilFilePath, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"getCurrentId", (PyCFunction)SmilTreeObj_getCurrentId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setCurrentId", (PyCFunction)SmilTreeObj_setCurrentId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getRecentTextId", (PyCFunction)SmilTreeObj_getRecentTextId, 1,
	 PyDoc_STR("() -> (std::string _rv)")},
	{"setRecentTextId", (PyCFunction)SmilTreeObj_setRecentTextId, 1,
	 PyDoc_STR("(std::string _arg1) -> None")},
	{"getCouldEscape", (PyCFunction)SmilTreeObj_getCouldEscape, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{"setCouldEscape", (PyCFunction)SmilTreeObj_setCouldEscape, 1,
	 PyDoc_STR("(bool _arg1) -> None")},
	{"setCustomTestSet", (PyCFunction)SmilTreeObj_setCustomTestSet, 1,
	 PyDoc_STR("(amis::dtb::CustomTestSet* _arg1) -> None")},
	{"firstPhrase", (PyCFunction)SmilTreeObj_firstPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"lastPhrase", (PyCFunction)SmilTreeObj_lastPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"nextPhrase", (PyCFunction)SmilTreeObj_nextPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"previousPhrase", (PyCFunction)SmilTreeObj_previousPhrase, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"goToId", (PyCFunction)SmilTreeObj_goToId, 1,
	 PyDoc_STR("(std::string _arg1) -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"escapeCurrentStructure", (PyCFunction)SmilTreeObj_escapeCurrentStructure, 1,
	 PyDoc_STR("() -> (amis::dtb::smil::SmilMediaGroup* _rv)")},
	{"isEmpty", (PyCFunction)SmilTreeObj_isEmpty, 1,
	 PyDoc_STR("() -> (bool _rv)")},
	{NULL, NULL, 0}
};

#define SmilTreeObj_getsetlist NULL


static int SmilTreeObj_compare(SmilTreeObject *self, SmilTreeObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define SmilTreeObj_repr NULL

static int SmilTreeObj_hash(SmilTreeObject *self)
{
	return (int)self->ob_itself;
}
static int SmilTreeObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::dtb::smil::SmilTree* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((SmilTreeObject *)_self)->ob_itself = new amis::dtb::smil::SmilTree();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, SmilTreeObj_Convert, &itself))
	{
		((SmilTreeObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define SmilTreeObj_tp_alloc PyType_GenericAlloc

static PyObject *SmilTreeObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((SmilTreeObject *)_self)->ob_itself = NULL;
	return _self;
}

#define SmilTreeObj_tp_free PyObject_Del


PyTypeObject SmilTree_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.SmilTree", /*tp_name*/
	sizeof(SmilTreeObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) SmilTreeObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) SmilTreeObj_compare, /*tp_compare*/
	(reprfunc) SmilTreeObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) SmilTreeObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	SmilTreeObj_methods, /* tp_methods */
	0, /*tp_members*/
	SmilTreeObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	SmilTreeObj_tp_init, /* tp_init */
	SmilTreeObj_tp_alloc, /* tp_alloc */
	SmilTreeObj_tp_new, /* tp_new */
	SmilTreeObj_tp_free, /* tp_free */
};

/* -------------------- End object type SmilTree -------------------- */


/* ------------------ Object type ModuleDescReader ------------------ */

extern PyTypeObject ModuleDescReader_Type;

inline bool ModuleDescReaderObj_Check(PyObject *x)
{
	return ((x)->ob_type == &ModuleDescReader_Type);
}

typedef struct ModuleDescReaderObject {
	PyObject_HEAD
	amis::io::ModuleDescReader* ob_itself;
} ModuleDescReaderObject;

PyObject *ModuleDescReaderObj_New(amis::io::ModuleDescReader* itself)
{
	ModuleDescReaderObject *it;
	if (itself == NULL)
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
#ifdef BGEN_BACK_SUPPORT_ModuleDescReader
	ModuleDescReader *encaps_itself = dynamic_cast<ModuleDescReader *>(itself);
	if (encaps_itself && encaps_itself->py_ModuleDescReader)
	{
		Py_INCREF(encaps_itself->py_ModuleDescReader);
		return encaps_itself->py_ModuleDescReader;
	}
#endif
	it = PyObject_NEW(ModuleDescReaderObject, &ModuleDescReader_Type);
	if (it == NULL) return NULL;
	it->ob_itself = itself;
	return (PyObject *)it;
}

int ModuleDescReaderObj_Convert(PyObject *v, amis::io::ModuleDescReader* *p_itself)
{
	if (v == Py_None)
	{
		*p_itself = NULL;
		return 1;
	}
#ifdef BGEN_BACK_SUPPORT_ModuleDescReader
	if (!ModuleDescReaderObj_Check(v))
	{
		*p_itself = Py_WrapAs_ModuleDescReader(v);
		if (*p_itself) return 1;
	}
#endif
	if (!ModuleDescReaderObj_Check(v))
	{
		PyErr_SetString(PyExc_TypeError, "ModuleDescReader required");
		return 0;
	}
	*p_itself = ((ModuleDescReaderObject *)v)->ob_itself;
	return 1;
}

static void ModuleDescReaderObj_dealloc(ModuleDescReaderObject *self)
{
	self->ob_type->tp_free((PyObject *)self);
}

static PyObject *ModuleDescReaderObj_readFromFile(ModuleDescReaderObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	bool _rv = _self->ob_itself->readFromFile(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *ModuleDescReaderObj_getModuleDescData(ModuleDescReaderObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	if (!PyArg_ParseTuple(_args, ""))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	amis::ModuleDescData* _rv = _self->ob_itself->getModuleDescData();
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     ModuleDescDataObj_New, _rv);
	return _res;
}

static PyMethodDef ModuleDescReaderObj_methods[] = {
	{"readFromFile", (PyCFunction)ModuleDescReaderObj_readFromFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (bool _rv)")},
	{"getModuleDescData", (PyCFunction)ModuleDescReaderObj_getModuleDescData, 1,
	 PyDoc_STR("() -> (amis::ModuleDescData* _rv)")},
	{NULL, NULL, 0}
};

#define ModuleDescReaderObj_getsetlist NULL


static int ModuleDescReaderObj_compare(ModuleDescReaderObject *self, ModuleDescReaderObject *other)
{
	if ( self->ob_itself > other->ob_itself ) return 1;
	if ( self->ob_itself < other->ob_itself ) return -1;
	return 0;
}

#define ModuleDescReaderObj_repr NULL

static int ModuleDescReaderObj_hash(ModuleDescReaderObject *self)
{
	return (int)self->ob_itself;
}
static int ModuleDescReaderObj_tp_init(PyObject *_self, PyObject *_args, PyObject *_kwds)
{
	amis::io::ModuleDescReader* itself;
	Py_KEYWORDS_STRING_TYPE *kw[] = {"itself", 0};

	{
		if (PyArg_ParseTuple(_args, ""))
		{
			((ModuleDescReaderObject *)_self)->ob_itself = new amis::io::ModuleDescReader();
			return 0;
		}
	}

	if (PyArg_ParseTupleAndKeywords(_args, _kwds, "O&", kw, ModuleDescReaderObj_Convert, &itself))
	{
		((ModuleDescReaderObject *)_self)->ob_itself = itself;
		return 0;
	}
	return -1;
}

#define ModuleDescReaderObj_tp_alloc PyType_GenericAlloc

static PyObject *ModuleDescReaderObj_tp_new(PyTypeObject *type, PyObject *_args, PyObject *_kwds)
{
	PyObject *_self;

	if ((_self = type->tp_alloc(type, 0)) == NULL) return NULL;
	((ModuleDescReaderObject *)_self)->ob_itself = NULL;
	return _self;
}

#define ModuleDescReaderObj_tp_free PyObject_Del


PyTypeObject ModuleDescReader_Type = {
	PyObject_HEAD_INIT(NULL)
	0, /*ob_size*/
	"amiscore.ModuleDescReader", /*tp_name*/
	sizeof(ModuleDescReaderObject), /*tp_basicsize*/
	0, /*tp_itemsize*/
	/* methods */
	(destructor) ModuleDescReaderObj_dealloc, /*tp_dealloc*/
	0, /*tp_print*/
	(getattrfunc)0, /*tp_getattr*/
	(setattrfunc)0, /*tp_setattr*/
	(cmpfunc) ModuleDescReaderObj_compare, /*tp_compare*/
	(reprfunc) ModuleDescReaderObj_repr, /*tp_repr*/
	(PyNumberMethods *)0, /* tp_as_number */
	(PySequenceMethods *)0, /* tp_as_sequence */
	(PyMappingMethods *)0, /* tp_as_mapping */
	(hashfunc) ModuleDescReaderObj_hash, /*tp_hash*/
	0, /*tp_call*/
	0, /*tp_str*/
	PyObject_GenericGetAttr, /*tp_getattro*/
	PyObject_GenericSetAttr, /*tp_setattro */
	0, /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT|Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /*tp_doc*/
	0, /*tp_traverse*/
	0, /*tp_clear*/
	0, /*tp_richcompare*/
	0, /*tp_weaklistoffset*/
	0, /*tp_iter*/
	0, /*tp_iternext*/
	ModuleDescReaderObj_methods, /* tp_methods */
	0, /*tp_members*/
	ModuleDescReaderObj_getsetlist, /*tp_getset*/
	0, /*tp_base*/
	0, /*tp_dict*/
	0, /*tp_descr_get*/
	0, /*tp_descr_set*/
	0, /*tp_dictoffset*/
	ModuleDescReaderObj_tp_init, /* tp_init */
	ModuleDescReaderObj_tp_alloc, /* tp_alloc */
	ModuleDescReaderObj_tp_new, /* tp_new */
	ModuleDescReaderObj_tp_free, /* tp_free */
};

/* ---------------- End object type ModuleDescReader ---------------- */


static PyObject *PyAm_isNccFile(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _rv;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_rv = amis::dtb::DtbFileSet::isNccFile(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyObject *PyAm_isOpfFile(PyObject *_self, PyObject *_args)
{
	PyObject *_res = NULL;
	bool _rv;
	ambulant::net::url* _arg1;
	if (!PyArg_ParseTuple(_args, "O&",
	                      ambulant_url_Convert, &_arg1))
		return NULL;
	PyThreadState *_save = PyEval_SaveThread();
	_rv = amis::dtb::DtbFileSet::isOpfFile(_arg1);
	PyEval_RestoreThread(_save);
	_res = Py_BuildValue("O&",
	                     bool_New, _rv);
	return _res;
}

static PyMethodDef PyAm_methods[] = {
	{"isNccFile", (PyCFunction)PyAm_isNccFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (bool _rv)")},
	{"isOpfFile", (PyCFunction)PyAm_isOpfFile, 1,
	 PyDoc_STR("(ambulant::net::url* _arg1) -> (bool _rv)")},
	{NULL, NULL, 0}
};



// Declare initamiscore as a C external:

extern "C" void initamiscore(); 


void initamiscore(void)
{
	PyObject *m;
	PyObject *d;


	PyEval_InitThreads();


	m = Py_InitModule("amiscore", PyAm_methods);
	d = PyModule_GetDict(m);
	PyAm_Error = PyErr_NewException("amiscore.Error", NULL, NULL);
	if (PyAm_Error == NULL ||
	    PyDict_SetItemString(d, "Error", PyAm_Error) != 0)
		return;
	Error_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&Error_Type) < 0) return;
	Py_INCREF(&Error_Type);
	PyModule_AddObject(m, "Error", (PyObject *)&Error_Type);
	MediaNode_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&MediaNode_Type) < 0) return;
	Py_INCREF(&MediaNode_Type);
	PyModule_AddObject(m, "MediaNode", (PyObject *)&MediaNode_Type);
	TextNode_Type.ob_type = &PyType_Type;
	TextNode_Type.tp_base = &MediaNode_Type;
	if (PyType_Ready(&TextNode_Type) < 0) return;
	Py_INCREF(&TextNode_Type);
	PyModule_AddObject(m, "TextNode", (PyObject *)&TextNode_Type);
	AudioNode_Type.ob_type = &PyType_Type;
	AudioNode_Type.tp_base = &MediaNode_Type;
	if (PyType_Ready(&AudioNode_Type) < 0) return;
	Py_INCREF(&AudioNode_Type);
	PyModule_AddObject(m, "AudioNode", (PyObject *)&AudioNode_Type);
	ImageNode_Type.ob_type = &PyType_Type;
	ImageNode_Type.tp_base = &MediaNode_Type;
	if (PyType_Ready(&ImageNode_Type) < 0) return;
	Py_INCREF(&ImageNode_Type);
	PyModule_AddObject(m, "ImageNode", (PyObject *)&ImageNode_Type);
	MediaGroup_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&MediaGroup_Type) < 0) return;
	Py_INCREF(&MediaGroup_Type);
	PyModule_AddObject(m, "MediaGroup", (PyObject *)&MediaGroup_Type);
	ModuleDescData_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&ModuleDescData_Type) < 0) return;
	Py_INCREF(&ModuleDescData_Type);
	PyModule_AddObject(m, "ModuleDescData", (PyObject *)&ModuleDescData_Type);
	RecentBookEntry_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&RecentBookEntry_Type) < 0) return;
	Py_INCREF(&RecentBookEntry_Type);
	PyModule_AddObject(m, "RecentBookEntry", (PyObject *)&RecentBookEntry_Type);
	RecentBooks_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&RecentBooks_Type) < 0) return;
	Py_INCREF(&RecentBooks_Type);
	PyModule_AddObject(m, "RecentBooks", (PyObject *)&RecentBooks_Type);
	PositionData_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&PositionData_Type) < 0) return;
	Py_INCREF(&PositionData_Type);
	PyModule_AddObject(m, "PositionData", (PyObject *)&PositionData_Type);
	PositionMark_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&PositionMark_Type) < 0) return;
	Py_INCREF(&PositionMark_Type);
	PyModule_AddObject(m, "PositionMark", (PyObject *)&PositionMark_Type);
	Bookmark_Type.ob_type = &PyType_Type;
	Bookmark_Type.tp_base = &PositionMark_Type;
	if (PyType_Ready(&Bookmark_Type) < 0) return;
	Py_INCREF(&Bookmark_Type);
	PyModule_AddObject(m, "Bookmark", (PyObject *)&Bookmark_Type);
	Hilite_Type.ob_type = &PyType_Type;
	Hilite_Type.tp_base = &PositionMark_Type;
	if (PyType_Ready(&Hilite_Type) < 0) return;
	Py_INCREF(&Hilite_Type);
	PyModule_AddObject(m, "Hilite", (PyObject *)&Hilite_Type);
	BookmarkSet_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&BookmarkSet_Type) < 0) return;
	Py_INCREF(&BookmarkSet_Type);
	PyModule_AddObject(m, "BookmarkSet", (PyObject *)&BookmarkSet_Type);
	CustomTest_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&CustomTest_Type) < 0) return;
	Py_INCREF(&CustomTest_Type);
	PyModule_AddObject(m, "CustomTest", (PyObject *)&CustomTest_Type);
	CustomTestSet_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&CustomTestSet_Type) < 0) return;
	Py_INCREF(&CustomTestSet_Type);
	PyModule_AddObject(m, "CustomTestSet", (PyObject *)&CustomTestSet_Type);
	Dtb_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&Dtb_Type) < 0) return;
	Py_INCREF(&Dtb_Type);
	PyModule_AddObject(m, "Dtb", (PyObject *)&Dtb_Type);
	DtbFileSet_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&DtbFileSet_Type) < 0) return;
	Py_INCREF(&DtbFileSet_Type);
	PyModule_AddObject(m, "DtbFileSet", (PyObject *)&DtbFileSet_Type);
	MetaItem_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&MetaItem_Type) < 0) return;
	Py_INCREF(&MetaItem_Type);
	PyModule_AddObject(m, "MetaItem", (PyObject *)&MetaItem_Type);
	Metadata_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&Metadata_Type) < 0) return;
	Py_INCREF(&Metadata_Type);
	PyModule_AddObject(m, "Metadata", (PyObject *)&Metadata_Type);
	Spine_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&Spine_Type) < 0) return;
	Py_INCREF(&Spine_Type);
	PyModule_AddObject(m, "Spine", (PyObject *)&Spine_Type);
	NavContainer_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&NavContainer_Type) < 0) return;
	Py_INCREF(&NavContainer_Type);
	PyModule_AddObject(m, "NavContainer", (PyObject *)&NavContainer_Type);
	NavList_Type.ob_type = &PyType_Type;
	NavList_Type.tp_base = &NavContainer_Type;
	if (PyType_Ready(&NavList_Type) < 0) return;
	Py_INCREF(&NavList_Type);
	PyModule_AddObject(m, "NavList", (PyObject *)&NavList_Type);
	NavMap_Type.ob_type = &PyType_Type;
	NavMap_Type.tp_base = &NavContainer_Type;
	if (PyType_Ready(&NavMap_Type) < 0) return;
	Py_INCREF(&NavMap_Type);
	PyModule_AddObject(m, "NavMap", (PyObject *)&NavMap_Type);
	NavModel_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&NavModel_Type) < 0) return;
	Py_INCREF(&NavModel_Type);
	PyModule_AddObject(m, "NavModel", (PyObject *)&NavModel_Type);
	NavNode_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&NavNode_Type) < 0) return;
	Py_INCREF(&NavNode_Type);
	PyModule_AddObject(m, "NavNode", (PyObject *)&NavNode_Type);
	NavPoint_Type.ob_type = &PyType_Type;
	NavPoint_Type.tp_base = &NavNode_Type;
	if (PyType_Ready(&NavPoint_Type) < 0) return;
	Py_INCREF(&NavPoint_Type);
	PyModule_AddObject(m, "NavPoint", (PyObject *)&NavPoint_Type);
	NavTarget_Type.ob_type = &PyType_Type;
	NavTarget_Type.tp_base = &NavNode_Type;
	if (PyType_Ready(&NavTarget_Type) < 0) return;
	Py_INCREF(&NavTarget_Type);
	PyModule_AddObject(m, "NavTarget", (PyObject *)&NavTarget_Type);
	PageList_Type.ob_type = &PyType_Type;
	PageList_Type.tp_base = &NavList_Type;
	if (PyType_Ready(&PageList_Type) < 0) return;
	Py_INCREF(&PageList_Type);
	PyModule_AddObject(m, "PageList", (PyObject *)&PageList_Type);
	PageTarget_Type.ob_type = &PyType_Type;
	PageTarget_Type.tp_base = &NavTarget_Type;
	if (PyType_Ready(&PageTarget_Type) < 0) return;
	Py_INCREF(&PageTarget_Type);
	PyModule_AddObject(m, "PageTarget", (PyObject *)&PageTarget_Type);
	SmilMediaGroup_Type.ob_type = &PyType_Type;
	SmilMediaGroup_Type.tp_base = &MediaGroup_Type;
	if (PyType_Ready(&SmilMediaGroup_Type) < 0) return;
	Py_INCREF(&SmilMediaGroup_Type);
	PyModule_AddObject(m, "SmilMediaGroup", (PyObject *)&SmilMediaGroup_Type);
	SmilTree_Type.ob_type = &PyType_Type;
	SmilTree_Type.tp_base = &MediaGroup_Type;
	if (PyType_Ready(&SmilTree_Type) < 0) return;
	Py_INCREF(&SmilTree_Type);
	PyModule_AddObject(m, "SmilTree", (PyObject *)&SmilTree_Type);
	ModuleDescReader_Type.ob_type = &PyType_Type;
	if (PyType_Ready(&ModuleDescReader_Type) < 0) return;
	Py_INCREF(&ModuleDescReader_Type);
	PyModule_AddObject(m, "ModuleDescReader", (PyObject *)&ModuleDescReader_Type);



}

/* ====================== End module amiscore ======================= */

