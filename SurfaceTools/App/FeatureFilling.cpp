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
#include <BRepBuilderAPI_MakeFace.hxx>
# include <TopoDS.hxx>
# include <Precision.hxx>
#endif

#include "FeatureFilling.h"
#include <BRepFill_Filling.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>

using namespace Part;

PROPERTY_SOURCE(Part::Filling, Part::Feature)

//Check if any components of the surface have been modified

short Filling::mustExecute() const
{
    if (Border.isTouched() ||
        BOrd.isTouched() ||
        Curves.isTouched() ||
        COrd.isTouched() ||
        Points.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Filling::execute(void)
{

    try{


    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute
