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

#include "FeatureFilling.h"
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>

using namespace SurfaceTools;

PROPERTY_SOURCE(SurfaceTools::Filling, Part::Feature)

//Initial values

Filling::Filling()
{
    ADD_PROPERTY(aFaceList,(0,"TopoDS_Face"));
    ADD_PROPERTY(aEdgeList,(0,"TopoDS_Edge"));

    App::PropertyFloat tol;
    App::PropertyBool sewopt;         //Option for sewing (if false only control)
    App::PropertyBool degenshp;       //Option for analysis of degenerated shapes
    App::PropertyBool cutfreeedges;   //Option for cutting of free edges
    App::PropertyBool nonmanifold;    //Option for non-manifold processing

    ADD_PROPERTY(tol,(0.0000001));
    ADD_PROPERTY(sewopt,(true));
    ADD_PROPERTY(degenshp,(true));
    ADD_PROPERTY(cutfreeedges,(true));
    ADD_PROPERTY(nonmanifold,(false));

}

//Function Definitions

bool addshape(BRepFill_Filling& builder,const App::PropertyLinkSubList& aShapeList);

//Check if any components of the surface have been modified

short Filling::mustExecute() const
{
    if (aFaceList.isTouched() ||
        aEdgeList.isTouched() ||
        tol.isTouched() ||
        sewopt.isTouched() ||
        degenshp.isTouched() ||
        cutfreeedges.isTouched() ||
        nonmanifold.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *Filling::execute(void)
{

    //Assign Variables

    double atol = tol.getValue();
    bool opt1 = sewopt.getValue();
    bool opt2 = degenshp.getValue();
    bool opt3 = cutfreeedges.getValue();
    bool opt4 = nonmanifold.getValue();

    //Perform error checking


    bool res;

    //Begin Construction
    try{

        BRepBuilderAPI_Sewing builder(atol,opt1,opt2,opt3,opt4);

        res = addshape(builder,aFaceList);

        if(!res){
            Standard_Failure::Raise("Failed to add face. Check list of faces.");
        }

        res = addshape(builder,aEdgeList);

        if(!res){
            Standard_Failure::Raise("Failed to add face. Check list of faces.");
        }

        builder.perform(); //Perform Sewing

        TopoDS_Shape aShape = builder.SewedShape(); //Get Shape
        
        if (swept.IsNull())
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        this->Shape.setValue(aShape);


    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute

bool addshape(BRepFill_Filling& builder,const App::PropertyLinkSubList& aShapeList){

    std::vector<App::DocumentObject*> aShapeVec = aShapeList.getValues(); //Get the shapes to add
    Base::Type type = aShapeList.getTypeId(); //Get the type id

    for (std::vector<App::DocumentObject*>::const_iterator it = aShapeVec.begin(); it != aShapeVec.end(); ++it) {

        Part::TopoShape ts = static_cast<Part::Feature*>(*it)->Shape.getShape();//Convert from App::DocumentObject* to Part::Feature
        TopoDS_Shape s = ts.getSubShape(type.getName()); //Setup a shape object

        builder.add(s);

    }

 return true;

}
