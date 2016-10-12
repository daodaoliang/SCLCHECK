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


UCSChar *ba0,*ba1,*ba2,*ba3,*ba4,*ba5;
void convert(VTDNav *vn, XMLModifier& xm){
	
	int i = -1;
	vn->toElement(FIRST_CHILD);
	i = vn->getText();
	xm.updateToken(i,ba0);

	vn->toElement(NEXT_SIBLING);
	i = vn->getText();
	xm.updateToken(i,ba1);

	vn->toElement(NEXT_SIBLING);
	i = vn->getText();
	xm.updateToken(i,ba2);

	vn->toElement(NEXT_SIBLING);
	i = vn->getText();
	xm.updateToken(i,ba3);

	vn->toElement(NEXT_SIBLING);
	i = vn->getText();
	xm.updateToken(i,ba4);

	vn->toElement(NEXT_SIBLING);
	i = vn->getText();
	xm.updateToken(i,ba5);

	vn->toElement(PARENT);
}

int main(){
	VTDNav *vn = NULL;

	int i;
	Long l;   
	ba0=L"                      ";
	ba1=L"              ";
	ba2=L"       ";
	ba3=L"              ";
	ba4=L"        ";
	ba5=L"        ";
	try{			
		VTDGen vg;		
		if (vg.parseFile(true,"old_cd.xml")==false){
			delete(vg.getXML());
			return 0;
		}	
		XMLModifier xm;
		AutoPilot ap1 ;
		ap1.selectXPath(L"/CATALOG/CD");
		vn = vg.getNav();
		ap1.bind(vn);
		xm.bind(vn);		
		while((i=ap1.evalXPath())!=-1){
			convert(vn,xm);		
		}	
		xm.output("cd_template.xml");
		delete(vn->getXML());
	}catch(...){// handle various types of exceptions here
	}
	delete(vn);
	return 0;
}