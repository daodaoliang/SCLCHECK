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
	UCSChar *string = NULL; 
	int i;
	Long l;    
	try{
		AutoPilot ap1 ; 
		AutoPilot ap2; 
		AutoPilot ap3;
		ap1.selectXPath(L"/root/a");
		ap2.selectXPath(L"/root/b");
		ap3.selectXPath(L"/root/c");
		f = fopen("output.xml","wb");
		VTDGen vg;		
		if (vg.parseFile(true,"input.xml")==false){
			delete(vg.getXML());
			return 0;
		}		
		vn = vg.getNav();
		ap1.bind(vn); 
		ap2.bind(vn);
		ap3.bind(vn);
		fwrite("<root>",1,6,f);
		while((i=ap1.evalXPath())!=-1){
			l = vn->getElementFragment();
			fwrite("\n",1,1,f);
			fwrite(vn->getXML()+(int)l,1,(int)(l>>32),f);
		}
		while((i=ap2.evalXPath())!=-1){
			l = vn->getElementFragment();
			fwrite("\n",1,1,f);
			fwrite(vn->getXML()+(int)l,1,(int)(l>>32),f);
		}
		while((i=ap3.evalXPath())!=-1){
			l = vn->getElementFragment();
			fwrite("\n",1,1,f);
			fwrite(vn->getXML()+(int)l,1,(int)(l>>32),f);
		}
		fwrite("\n</root>",1,8,f);		
		free(vn->getXML());
	}catch(...){// handle various types of exceptions here
	}
	delete (vn);
	return 0;
}