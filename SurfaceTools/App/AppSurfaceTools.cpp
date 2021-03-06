/***************************************************************************
 *   Copyright (c) 2014 Nathan Miller         <Nathan.A.Mill[at]gmail.com> *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <Python.h>
#endif

#include <Base/Console.h>
#include "FeatureFilling.h"
#include "FeatureSewing.h"
#include "FeatureCut.h"
#include "FeatureBezSurf.h"
#include "FeatureBSplineSurf.h"

#include <Base/Interpreter.h>
#include <Base/Parameter.h>


/* registration table  */
extern struct PyMethodDef SurfaceTools_methods[];

PyDoc_STRVAR(module_SurfaceTools_doc,
"This module is the SurfaceTools module.");


/* Python entry */
extern "C" {
void SurfaceToolsExport initSurfaceTools() {

    try {
        Base::Interpreter().runString("import Part");
    }
    catch(const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        return;
    }

    // ADD YOUR CODE HERE
    //
    //
    (void) Py_InitModule3("SurfaceTools", SurfaceTools_methods, module_SurfaceTools_doc);   /* mod name, table ptr */
    Base::Console().Log("Loading SurfaceTools module... done\n");

    // Add types to module
    SurfaceTools::Filling        ::init();
    SurfaceTools::Sewing         ::init();
    SurfaceTools::Cut            ::init();
    SurfaceTools::BezSurf        ::init();
    SurfaceTools::BSplineSurf    ::init();
}

} // extern "C"
