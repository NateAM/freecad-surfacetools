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
#include <BRepFill_Filling.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <Base/Tools.h>
#include <Base/Exception.h>
#include <string.h>

using namespace SurfaceTools;

PROPERTY_SOURCE(SurfaceTools::Filling, Part::Feature)

//Initial values

Filling::Filling()
{
    ADD_PROPERTY(Border,(0,"TopoDS_Edge"));
//    ADD_PROPERTY(Border,(0));
    ADD_PROPERTY(BOrd,(-1));
    ADD_PROPERTY(Curves,(0,"TopoDS_Edge"));
//    ADD_PROPERTY(Curves,(0));
    ADD_PROPERTY(COrd,(-1));
    ADD_PROPERTY(Points,(0,"TopoDS_Vertex"));
//    ADD_PROPERTY(Points,(0));

    ADD_PROPERTY(Degree,(3));
    ADD_PROPERTY(NbPtsOnCur,(3));
    ADD_PROPERTY(NbIter,(2));
    ADD_PROPERTY(Anisotropie,(false));
    ADD_PROPERTY(Tol2d,(0.00001));
    ADD_PROPERTY(Tol3d,(0.0001));
    ADD_PROPERTY(TolAng,(0.001));
    ADD_PROPERTY(TolCurv,(0.01));
    ADD_PROPERTY(MaxDeg,(8));
    ADD_PROPERTY(MaxSegments,(10000));
}

//Define Functions

bool appconstr_crv(BRepFill_Filling& builder,const App::PropertyLinkSubList& anEdge,const App::PropertyIntegerList& Order, bool bnd);

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

    //Assign Variables

    

    unsigned int Deg  = Degree.getValue();
    unsigned int NPOC = NbPtsOnCur.getValue();
    unsigned int NI   = NbIter.getValue();
    bool Anis = Anisotropie.getValue();
    double T2d = Tol2d.getValue();
    double T3d = Tol3d.getValue();
    double TG1 = TolAng.getValue();
    double TG2 = TolCurv.getValue();
    unsigned int Mdeg = MaxDeg.getValue();
    unsigned int Mseg = MaxSegments.getValue();

    bool res;

    //Perform error checking


    //Begin Construction
    try{

        //Generate Builder with Algorithm Variables

        BRepFill_Filling builder(Deg,NPOC,NI,Anis,T2d,T3d,TG1,TG2,Mdeg,Mseg);

        //Assign Boundaries

        res = appconstr_crv(builder, Border, BOrd, true);

//        if(!res){
//            Standard_Failure::Raise("Failed to create a boundary constraint. Check constraints on boundaries.");
//        }

        //Assign Additional Curves

        res = appconstr_crv(builder, Curves, COrd, false);

//        if(!res){
//            Standard_Failure::Raise("Failed to create a curve constraint. Check constraints on curves.");
//        }

        //Assign Point Constraints (Do this next)

        printf("Building...\n");

        //Build the face
        builder.Build();

        printf("Build Complete\n");

        //Return the face
        TopoDS_Face aFace = builder.Face();

        printf("We have a face!\n");

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

bool appconstr_crv(BRepFill_Filling& builder,const App::PropertyLinkSubList& anEdge,const App::PropertyIntegerList& Order, bool bnd){

    printf("Inside appconstr_crv\n");

    GeomAbs_Shape ordtmp;

    std::vector<long int>::const_iterator bc = Order.getValues().begin(); //Get the order values

//    std::vector<App::DocumentObject*> edges = anEdge.getValues(); //Get the edges
//    Base::Type type = anEdge.getTypeId(); //Get the type id
    int res;

    printf("Entering for loop\n");

//    for (std::vector<App::DocumentObject*>::const_iterator it = edges.begin(); it != edges.end(); ++it) {
    for(int i=0; i<anEdge.getSize(); i++) {

        printf("Processing curve %i\n",i);

        Part::TopoShape ts;
//        Part::TopoShape sub;
        TopoDS_Shape sub;
        TopoDS_Edge etmp;
        std::vector< const char * > temp;
       
       //the subset has the documentobject and the element name which belongs to it,
       // in our case for example the cube object and the "Edge1" string
        App::PropertyLinkSubList::SubSet set = anEdge[i];

        //set.obj should be our box, but just to make sure no one set something stupid
        if(set.obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
       
            //we get the shape of the document object which resemble the whole box
            ts = static_cast<Part::Feature*>(set.obj)->Shape.getShape();
           
            //we want only the subshape which is linked
            sub = ts.getSubShape(set.sub);
            
            if(sub.ShapeType() == TopAbs_EDGE) {etmp = TopoDS::Edge(sub);} //Check Shape type and assign edge
            else{Standard_Failure::Raise("Curves must be type TopoDS_Edge");return false;} //Raise exception
                
        }

        else{Standard_Failure::Raise("Boundary or Curve not from Part::Feature");return false;}

        if(*bc==0){ordtmp = GeomAbs_C0;}
        else if(*bc==1){ordtmp = GeomAbs_G1;}
        else if(*bc==2){ordtmp = GeomAbs_G2;}
	else{Standard_Failure::Raise("Continuity constraint must be 0, 1 or 2.");return false;}

        printf("*bc: %li\n",*bc);

        res = builder.Add(etmp,ordtmp,bnd);

        printf("Result of builder.Add: %i\n",res);

        bc++;

    }

    return true;

}/*
    for (std::vector<App::DocumentObject*>::const_iterator it = edges.begin(); it != edges.end(); ++it) {

        Part::TopoShape ts = static_cast<Part::Feature*>(*it)->Shape.getShape();//Convert from App::DocumentObject* to Part::Feature

        TopoDS_Shape s = ts.getSubShape(type.getName()); //Setup a shape object

        cout << "TopoDS_Shape s: " << s << endl; 

        TopoDS_Edge edge; //Setup an edge object
        if(s.ShapeType() == TopAbs_EDGE) {edge = TopoDS::Edge(s);} //Check Shape type and assign edge
        else{Standard_Failure::Raise("Curves must be type TopoDS_Edge");return false;} //Raise exception

        cout << "TopoDS_Edge edge: " << edge << endl; 

	//PropertyEnumerateList doesn't exist yet. Fix when implemented

	if(*bc==0){ordtmp = GeomAbs_C0;}
        else if(*bc==1){ordtmp = GeomAbs_G1;}
        else if(*bc==2){ordtmp = GeomAbs_G2;}
	else{return false;}

        res = builder.Add(edge,ordtmp,bnd);

        if(!res){return false;}

        bc++;

    }

    return true;

}*/
