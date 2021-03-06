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

#include <Python.h>
#include "pyhelpers.h"

PyObject *PyhelpersError;

/* Methods */
static PyObject *pyhi_killProcess(PyObject *self, PyObject *args)
{
    DWORD pid;

    if (!PyArg_ParseTuple(args, "i", &pid))
    {
        PyErr_SetString(PyExc_TypeError, "usage killProcess(pid)");
        return NULL;
    }

    return (killProcess(pid) ? Py_False : Py_True);
}

static PyObject *pyhi_unloadModule(PyObject *self, PyObject *args)
{
    DWORD pid;
    HANDLE hModule;

    if (!PyArg_ParseTuple(args, "il", &pid, (long *) &hModule))
    {
        PyErr_SetString(PyExc_TypeError, "usage unloadModule(pid, handle)");
        return NULL;
    }

    return (unloadModule(pid, hModule) ? Py_False : Py_True);
}

/* Methods Table */
static PyMethodDef pyhelpersMethods[] =
{
    { "unloadModule", pyhi_unloadModule, METH_VARARGS, "Unload a module from a running process" },
    { "killProcess",  pyhi_killProcess,  METH_VARARGS, "Kill a running process"                 },
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC
initpyhelpers(void)
{
    PyObject *m = Py_InitModule("pyhelpers", pyhelpersMethods);

    PyhelpersError = PyErr_NewException("pyhelpers.PyhelpersError", NULL, NULL);
    PyModule_AddObject(m, "PyhelpersError", PyhelpersError);

    PyModule_AddStringConstant(m, "__version__", PYHELPERS_VERSION);

    if (pyhi_dynlink())
        initToolHelp(m);
    else
        fprintf(stderr, "ToolHelp module not supported on this OS\n");

    /* Py_AtExit(pyhi_cleanup); */
}

int main(int argc, char *argv[])
{
    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(argv[0]);

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();

    /* Add a static module */
    initpyhelpers();
    return 0;
}
