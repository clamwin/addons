/*
 * Win32 Process Helpers
 *
 * Copyright (c) 2008 Gianluigi Tiesi <sherpya@netfarm.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "pyhelpers.h"

typedef struct {
    PyObject_HEAD
    HANDLE hSnap;
    PROCESSENTRY32 ProcStruct;
    int NoMoreItems;
    /* Type-specific fields go here. */
} ToolHelpObject;

static void ToolHelp_dealloc(ToolHelpObject *self)
{
    if (self->hSnap != INVALID_HANDLE_VALUE)
        CloseHandle(self->hSnap);
    self->ob_type->tp_free((PyObject*) self);
}

static int
ToolHelp_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    ToolHelpObject *toself = (ToolHelpObject *) self;

    if (!EnablePrivilege(SE_DEBUG_NAME, SE_PRIVILEGE_ENABLED))
        fprintf(stderr, "EnablePrivilege() failed\n");

    toself->hSnap = pyhi_helpers.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (toself->hSnap == INVALID_HANDLE_VALUE)
    {
        PyErr_SetString(PyhelpersError, "CreateToolhelp32Snapshot() failed");
        return -1;
    }

    memset(&toself->ProcStruct, 0, sizeof(PROCESSENTRY32));
    toself->ProcStruct.dwSize = sizeof(PROCESSENTRY32);

    if (!pyhi_helpers.Process32First(toself->hSnap, &toself->ProcStruct))
    {
        PyErr_SetString(PyhelpersError, "Process32First() failed\n");
        CloseHandle(toself->hSnap);
    }

    toself->NoMoreItems = 0;
    return 0;
}

static PyObject *ToolHelp_self(ToolHelpObject *f)
{
    return (PyObject *) f;
}

static PyObject *ToolHelp_iternext(ToolHelpObject *self)
{
    int pid;
    char szExeFile[MAX_PATH];
    MODULEENTRY32 me32;
    HANDLE hModuleSnap;
    PyObject *list;

    if (self->NoMoreItems) return NULL;

    pid = self->ProcStruct.th32ProcessID;
    memcpy(szExeFile, self->ProcStruct.szExeFile, MAX_PATH);

    if (!pyhi_helpers.Process32Next(self->hSnap, &self->ProcStruct))
        self->NoMoreItems = 1;

    if (!pid) return ToolHelp_iternext(self);  /* 0 is currentprocess for th32_snapmodule, but it's also system process */

    list = PyList_New(0);

    hModuleSnap = pyhi_helpers.CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32, pid);

    if (hModuleSnap != INVALID_HANDLE_VALUE)
    {
        me32.dwSize = sizeof(MODULEENTRY32);

        if(pyhi_helpers.Module32First(hModuleSnap, &me32))
        {
            do
                PyList_Append(list, Py_BuildValue("(s,l)", me32.szExePath, me32.hModule));
            while (pyhi_helpers.Module32Next(hModuleSnap, &me32));
        }
        else
            fprintf(stderr, "Module32First() Failed %d\n", GetLastError());

        CloseHandle(hModuleSnap);
    }
    /*
    else
        fprintf(stderr, "SnapModule() Failed %d\n", GetLastError());
    */

    return Py_BuildValue("(l,s,O)", pid, szExeFile, list);
}

static PyMethodDef ToolHelp_methods[] = {
    {NULL}  /* Sentinel */
};

static PyTypeObject ToolHelpType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /* ob_size */
    "pyhelpers.ToolHelp",      /* tp_name */
    sizeof(ToolHelpObject),   /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor) ToolHelp_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Tool Help Objects",       /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    (getiterfunc) ToolHelp_self,      /* tp_iter */
    (iternextfunc) ToolHelp_iternext, /* tp_iternext */
    ToolHelp_methods,          /* tp_methods */
    0,//ToolHelp_members       /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc) ToolHelp_init,  /* tp_init */
    0,                         /* tp_alloc */
    0,                         /* tp_new */
};

int initToolHelp(PyObject *module)
{
    ToolHelpType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&ToolHelpType) < 0)
        return -1;

    Py_INCREF(&ToolHelpType);
    PyModule_AddObject(module, "ToolHelp", (PyObject *) &ToolHelpType);
    return 0;
}
