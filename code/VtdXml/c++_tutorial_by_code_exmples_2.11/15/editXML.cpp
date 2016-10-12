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

void fillCD(VTDNav *vn, char *title, 
			char *artist, char *country, 
			char *company, float price, int year){
				int i = -1;
				char a[10],b[10];
				sprintf(a,"%4.2f",price);
				sprintf(b,"%d",year);
				
				if (vn->toElement(FIRST_CHILD))
				{
					vn->overWrite(vn->getText(), title, 0, strlen(title)); // the assumption --getText won't return 1
					vn->toElement(NEXT_SIBLING);
					vn->overWrite(vn->getText(), artist,0, strlen(artist));
					vn->toElement(NEXT_SIBLING);
					vn->overWrite(vn->getText(), country,0, strlen(country));
					vn->toElement(NEXT_SIBLING);
					vn->overWrite(vn->getText(), company, 0, strlen(company));
					vn->toElement(NEXT_SIBLING);
					vn->overWrite(vn->getText(), a, 0, strlen((char *)a));
					vn->toElement(NEXT_SIBLING);
					vn->overWrite(vn->getText(), b, 0,strlen((char *)b));
				}
				vn->toElement(PARENT);
}

int main(){
	
	VTDGen *vg = NULL;
	VTDNav *vn = NULL;
	AutoPilot *ap1 = NULL;
	FILE *f = NULL;
	UCSChar *string = NULL;

	int i;

	try{			
		VTDGen vg;		
		if (vg.parseFile(true,"cd_template.xml")==false){
			delete(vg.getXML());
			return 0;
		}	
		AutoPilot ap1;
		ap1.selectXPath(L"/CATALOG/CD");
		vn = vg.getNav();
		ap1.bind(vn);
		if((i=ap1.evalXPath())!=-1){
			fillCD(vn,"Empire Burlesque", "Bob Dylan", "USA", "Columbia", 10.90, 1985);		
		}	
		if((i=ap1.evalXPath())!=-1){
			fillCD(vn,"Still Got the Blues", "Gary More", "UK", "Virgin Records", 10.20, 1990);		
		}
		vn->dumpXML("new_cd.xml");
		delete(vn->getXML());
	}catch(...){// handle various types of exceptions here
	}
	
	delete(vn);
	return 0;
}