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

	VTDNav *vn = NULL;
	FILE *f = NULL;
	int i;
	try{			
		VTDGen vg;		
		if (vg.parseFile(true,"old_cd.xml")==false){
			delete(vg.getXML());
			return 0;
		}	
		AutoPilot ap1;
		ap1.selectXPath(L"/CATALOG/CD[PRICE=10.2]/*/text()");
		vn = vg.getNav();
		ap1.bind(vn);
		while((i=ap1.evalXPath())!=-1){
			vn->overWrite(i,"",0,0);		
		}
		vn->dumpXML("new_cd.xml");
		delete(vn->getXML());
	}catch(...){// handle various types of exceptions here
	}	
	delete(vn);
	return 0;
}