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
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_BezierCurve.hxx>
#include <Precision.hxx>
#endif

#include "FeatureBezSurf.h"
#include <GeomFill.hxx>
#include <GeomFill_BezierCurves.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>

using namespace SurfaceTools;

PROPERTY_SOURCE(SurfaceTools::BezSurf, Part::Feature)

//Initial values

BezSurf::BezSurf()
{
    ADD_PROPERTY(aBezList,(0,"Geom_BezierCurve"));
    ADD_PROPERTY(filltype,(1));

}

//Structures

struct crvs{

    Handle_Geom_BezierCurve C1;
    Handle_Geom_BezierCurve C2;
    Handle_Geom_BezierCurve C3;
    Handle_Geom_BezierCurve C4;

};

//Functions

void getCurves(GeomFill_BezierCurves& aBuilder,BRepBuilderAPI_MakeWire& aWireBuilder,const App::PropertyLinkSubList& anEdge, GeomFill_FillingStyle fstyle);
//bool orderCurves(crvs& Cs, int size);

//Check if any components of the surface have been modified

short BezSurf::mustExecute() const
{
    if (aBezList.isTouched() ||
        filltype.isTouched())
        return 1;
    return 0;
}

App::DocumentObjectExecReturn *BezSurf::execute(void)
{

    //Set Variables

    int ftype = filltype.getValue();

    //Begin Construction
    try{

        //Identify filling style

        GeomFill_FillingStyle fstyle;

        if(ftype==1){fstyle = GeomFill_StretchStyle;}
        else if(ftype==2){fstyle = GeomFill_CoonsStyle;}
        else if(ftype==3){fstyle = GeomFill_CurvedStyle;}
        else{return new App::DocumentObjectExecReturn("Filling style must be 1 (Stretch), 2 (Coons), or 3 (Curved).");}

        //Create Bezier Surface

        GeomFill_BezierCurves aSurfBuilder; //Create Surface Builder
        BRepBuilderAPI_MakeWire aWireBuilder; //Create Wire Builder

        //Get Bezier Curves from edges and initialize the builder

        getCurves(aSurfBuilder,aWireBuilder,aBezList,fstyle);

        //Create the surface

        const Handle_Geom_BezierSurface aSurface = aSurfBuilder.Surface();
        aWireBuilder.Build();
        const TopoDS_Wire aWire = aWireBuilder.Wire(); //Get the wire
        if(!aWireBuilder.IsDone()){return new App::DocumentObjectExecReturn("Wire unable to be constructed");}

        BRepBuilderAPI_MakeFace aFaceBuilder(aSurface,aWire,Standard_True); //Create Face Builder
        aFaceBuilder.Build(); //Build Face (Maybe not required?)

        TopoDS_Face aFace = aFaceBuilder.Face(); //Returned Face
        if(!aFaceBuilder.IsDone()){return new App::DocumentObjectExecReturn("Face unable to be constructed");}

        if (aFace.IsNull()){
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        }
        this->Shape.setValue(aFace);

        return App::DocumentObject::StdReturn;

    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute

void getCurves(GeomFill_BezierCurves& aBuilder,BRepBuilderAPI_MakeWire& aWireBuilder, const App::PropertyLinkSubList& anEdge, GeomFill_FillingStyle fstyle){

    crvs bcrv;
    Standard_Real u0 = 0.;
    Standard_Real u1 = 1.;

    if(anEdge.getSize()>4){Standard_Failure::Raise("Only 2-4 continuous Bezier Curves are allowed");return;}

    for(int i=0; i<anEdge.getSize(); i++){
        
        Part::TopoShape ts; //Curve TopoShape
        TopoDS_Shape sub;   //Curve TopoDS_Shape
        TopoDS_Edge etmp;   //Curve TopoDS_Edge
        Handle_Geom_Curve c_geom; //The geometric curve
        Handle_Geom_BezierCurve b_geom;  //The bezier curve
        TopLoc_Location e_loc; //Curve Location
        

        //Get Edge
        App::PropertyLinkSubList::SubSet set = anEdge[i];

        if(set.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
       
            ts = static_cast<Part::Feature*>(set.obj)->Shape.getShape();
               
            //we want only the subshape which is linked
            sub = ts.getSubShape(set.sub);
            
            if(sub.ShapeType() == TopAbs_EDGE) {etmp = TopoDS::Edge(sub);e_loc = etmp.Location();} //Check Shape type and assign edge
            else{Standard_Failure::Raise("Curves must be type TopoDS_Edge");return;} //Raise exception

            aWireBuilder.Add(etmp); //Add edge to wire
            c_geom = BRep_Tool::Curve(etmp,e_loc,u0,u1); //Get Curve Geometry
            b_geom = Handle_Geom_BezierCurve::DownCast(c_geom); //Try to get Bezier curve

            //Error checking
            if (!b_geom.IsNull()) {

                //Store Underlying Geometry
                if(i==0){bcrv.C1 = b_geom;}
                else if(i==1){bcrv.C2 = b_geom;}
                else if(i==2){bcrv.C3 = b_geom;}
                else if(i==3){bcrv.C4 = b_geom;}

            }
            else {
                Standard_Failure::Raise("Curve not a Bezier Curve");
                return;
            }             

        }

        else{Standard_Failure::Raise("Curve not from Part::Feature");return;}

    }

    int ncrv = anEdge.getSize();

    if(ncrv==2){aBuilder.Init(bcrv.C1,bcrv.C2,fstyle);}
    else if(ncrv==3){aBuilder.Init(bcrv.C1,bcrv.C2,bcrv.C3,fstyle);}
    else if(ncrv==4){aBuilder.Init(bcrv.C1,bcrv.C2,bcrv.C3,bcrv.C4,fstyle);}

    return;
}
/*
bool orderCurves(crvs& Cs, int size){

    crvs sort;

    int maxl = 2*size;

    gp_Pnt pts[8];

//    gp_Pnt *ppts;

//    ppts = pts;

    if(size==2){

        pts[0] = Cs.C1.Value(0.); //First Point on Curve 1
        pts[1] = Cs.C1.Value(1.); //Final Point on Curve 1

        pts[2] = Cs.C2.Value(0.); //First Point on Curve 2
        pts[3] = Cs.C2.Value(1.); //Final Point on Curve 2

    }

    else if(size==3){

        pts[0] = Cs.C1.Value(0.); //First Point on Curve 1
        pts[1] = Cs.C1.Value(1.); //Final Point on Curve 1

        pts[2] = Cs.C2.Value(0.); //First Point on Curve 2
        pts[3] = Cs.C2.Value(1.); //Final Point on Curve 2

        pts[4] = Cs.C3.Value(0.); //First Point on Curve 3
        pts[5] = Cs.C3.Value(1.); //Final Point on Curve 3

    }

    else if(size==4){

        pts[0] = Cs.C1.Value(0.); //First Point on Curve 1
        pts[1] = Cs.C1.Value(1.); //Final Point on Curve 1

        pts[2] = Cs.C2.Value(0.); //First Point on Curve 2
        pts[3] = Cs.C2.Value(1.); //Final Point on Curve 2

        pts[4] = Cs.C3.Value(0.); //First Point on Curve 3
        pts[5] = Cs.C3.Value(1.); //Final Point on Curve 3

        pts[6] = Cs.C4.Value(0.); //First Point on Curve 4
        pts[7] = Cs.C4.Value(1.); //Final Point on Curve 4

    }

    for(int i=1;i<maxl;i+=2){

        for(int j=0;j<2;++j){

            

        }

    }

}
*/
