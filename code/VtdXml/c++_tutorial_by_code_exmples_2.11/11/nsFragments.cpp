/* 
 * Copyright (C) 2002-2010 XimpleWare, info@ximpleware.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
// vtd-xml.cpp : Defines the entry point for the console application.
//

#include "everything.h"

using namespace com_ximpleware;

int main(){
	VTDNav *vn1 = NULL, *vn2=NULL;
	ElementFragmentNs *efn = NULL;
	int i;
	try{
		XMLModifier xm;
		AutoPilot ap;
		ap.selectXPath(L"/root/b");
		VTDGen vg1;
		if (vg1.parseFile(true,"input2.xml")==false){
			delete(vg1.getXML());
			return 0;
		}
		VTDGen vg2;
		if (vg2.parseFile(true,"input1.xml")==false){
			delete(vg2.getXML());
			delete(vg1.getXML());
			return 0;
		}
		vn1 = vg1.getNav();
		if (vn1->toElementNS(FIRST_CHILD,L"someURL2",L"b")){
			efn =vn1->getElementFragmentNs();
			vn2 = vg2.getNav();
			ap.bind(vn2);
			xm.bind(vn2);
			while((i=ap.evalXPath())!=-1){
				xm.insertBeforeElement(efn);
				xm.insertAfterElement(efn);
			}
			xm.output("output.xml");
		}
		delete(vn1->getXML());
		delete(vn2->getXML());	
	}catch(...){// handle various types of exceptions here
	}
	delete(efn);	
	delete(vn1);
	delete(vn2);
	return 0;
}