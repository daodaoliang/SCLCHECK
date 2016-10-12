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
	VTDNav *vn = NULL, *vn2=NULL; // This is the VTDNav that navigates the VTD records
	UByte *ba1 = NULL, *ba2=NULL;
	ElementFragmentNs *efs = NULL;
	Long l;
	try{
		VTDGen vg;
		// test2.xml is encoded in UTF-16LE
		if (vg.parseFile(true,"test2.xml")==false){
			// parsing failed
			delete(vg.getXML());
			return 1;
		}
		//parse(vg,TRUE);
		vn = vg.getNav();	
		ba1 =vn->getXML();
		efs = vn->getElementFragmentNs();

		if (vg.parseFile(true,"test.xml")==false){
			// parsing failed
			delete(ba1);
			delete(vn);
			delete(vg.getXML());
			return 0;
		}
		vn2 = vg.getNav();
		XMLModifier xm(vn2);
		vn2->toElement( FIRST_CHILD);
		xm.insertAfterElement(efs);
		xm.insertBeforeElement(efs);
		xm.output("output.xml");
		
		delete(vn2->getXML());
		free(ba1);
		delete(vn2);
		delete(efs);
		free(vn);
	}
	catch (...) {
	}
	return 0;
}