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

using namespace SurfaceTools;

PROPERTY_SOURCE(SurfaceTools::Filling, Part::Feature)

//Initial values

Filling::Filling()
{
    ADD_PROPERTY(Border,(0));
    ADD_PROPERTY(BOrd,(-1));
    ADD_PROPERTY(Curves,(0));
    ADD_PROPERTY(COrd,(-1));
    ADD_PROPERTY(Points,(0));

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

    //Perform error checking


    //Begin Construction
    try{

        BRepFill_Filling builder;

        //BRepFill_Filling::BRepFill_Filling(Deg,NPOC,NI,Anis,T2d,T3d,TG1,TG2,Mdeg,Mseg) builder; //Generate builder

        //Assign Boundaries


    } //End Try
    catch (Standard_Failure) {
        Handle_Standard_Failure e = Standard_Failure::Caught();
        return new App::DocumentObjectExecReturn(e->GetMessageString());
    } //End Catch

} //End execute
