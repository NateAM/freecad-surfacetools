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
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Precision.hxx>
#endif

#include "FeatureCut.h"
#include <BRepAlgoAPI_Cut.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>

using namespace SurfaceTools;

PROPERTY_SOURCE(SurfaceTools::Cut, Part::Feature)

//Initial values

Cut::Cut()
{
    ADD_PROPERTY(aShapeList,(0,"TopoDS_Shape"));

}

//Check if any components of the surface have been modified

short Cut::mustExecute() const
{
    if (aShapeList.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Cut::execute(void)
{

    //Perform error checking

    bool res;

    //Begin Construction
    try{

        if(aShapeList.getSize()>2){
            return new App::DocumentObjectExecReturn("Only two shapes may be entered at a time for a cut operation");
        }

        Part::TopoShape ts1;
        TopoDS_Shape sub1;
        App::PropertyLinkSubList::SubSet set1 = aShapeList[0];

        Part::TopoShape ts2;
        TopoDS_Shape sub2;
        App::PropertyLinkSubList::SubSet set2 = aShapeList[1];

        if(set1.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {

            ts1 = static_cast<Part::Feature*>(set1.obj)->Shape.getShape();
            sub1 = ts1.getSubShape(set1.sub); //Shape 1

        }
        else{return new App::DocumentObjectExecReturn("Shape1 not from Part::Feature");}

        if(set2.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {

            ts2 = static_cast<Part::Feature*>(set2.obj)->Shape.getShape();
            sub2 = ts2.getSubShape(set2.sub); //Shape 2

        }
        else{return new App::DocumentObjectExecReturn("Shape2 not from Part::Feature");}

        BRepAlgoAPI_Cut builder(sub1,sub2);

        builder.Build();
        builder.RefineEdges();

        TopoDS_Shape aShape = builder.Shape();

        if (aShape.IsNull()){
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        }
        this->Shape.setValue(builder.Shape());

    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute
