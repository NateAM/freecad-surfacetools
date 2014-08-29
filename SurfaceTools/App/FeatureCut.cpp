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
#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>
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

        //Initialize variables for first toposhape from document object
        Part::TopoShape ts1;
        TopoDS_Shape sub1;
        App::PropertyLinkSubList::SubSet set1 = aShapeList[0];

        //Initialize variables for second toposhape from document object
        Part::TopoShape ts2;
        TopoDS_Shape sub2;
        App::PropertyLinkSubList::SubSet set2 = aShapeList[1];

        //Get first toposhape
        printf("Get first toposhape\n");
        if(set1.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {

            ts1 = static_cast<Part::Feature*>(set1.obj)->Shape.getShape();
            sub1 = ts1.getSubShape(set1.sub); //Shape 1

        }
        else{return new App::DocumentObjectExecReturn("Shape1 not from Part::Feature");}

        //Get second toposhape
        printf("Get second toposhape\n");
        if(set2.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {

            ts2 = static_cast<Part::Feature*>(set2.obj)->Shape.getShape();
            printf("ts2 cast\n");
            sub2 = ts2.getSubShape(set2.sub); //Shape 2
            printf("got sub2\n");

        }
        else{return new App::DocumentObjectExecReturn("Shape2 not from Part::Feature");}

        //Cut every face in Shape1 by every face in Shape2

        TopoDS_Compound aCompound;  //Shape returned by cutting algorithm
        TopoDS_Shape prevCut;       //Shape used in iteration (previous cut face)
        TopoDS_Builder aCompBuilder;   //Builder used for compound shape

        aCompBuilder.MakeCompound(aCompound); //Create an empty compound surface

        //Loop over shape1
        printf("Loop1\n");
        for( TopExp_Explorer ex1(sub1, TopAbs_FACE); ex1.More(); ex1.Next() )
        {
            //Current face in shape1
            TopoDS_Face currentFace1 = TopoDS::Face( ex1.Current() );

            int i=0;

            //Loop over shape2
            printf("Loop2\n");
            for( TopExp_Explorer ex2(sub2, TopAbs_FACE); ex2.More(); ex2.Next() )
            {
                //Current face in shape2
                TopoDS_Face currentFace2 = TopoDS::Face( ex2.Current() );

                //Check if first time cutting currentFace1
                if(i==0){
                    printf("If\n");
                    //Make cut builder
                    BRepAlgoAPI_Cut builder(currentFace1,currentFace2);
                    //Cut Shape
                    builder.Build();
                    //Refine the edges
                    builder.RefineEdges();
                    //Define the previous cut as the current builder shape
                    prevCut = builder.Shape();
                }
                else{
                    printf("Else\n");
                    //Make cut builder
                    BRepAlgoAPI_Cut builder(prevCut,currentFace2);
                    //Cut Shape
                    builder.Build();
                    //Refine the edges
                    builder.RefineEdges();
                    //Define the previous cut as the current builder shape
                    prevCut = builder.Shape();
                }

                i++;    

            }

            //Check if shape is not null
            if(!prevCut.IsNull()){
                printf("Adding to compound\n");
                //Add shape to Shell
                aCompBuilder.Add(aCompound,prevCut);
                printf("Successful compound adding\n");
            }  

        }

        //Check if resulting shell is null
        if (aCompound.IsNull()){
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        }
        this->Shape.setValue(prevCut);

    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute
