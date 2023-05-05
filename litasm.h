/**********************************************************************
litasm Copyright 2006, 2007, 2008, 2009, 2023
by Al Williams (al.williamsw@awce.com)


This file is part of litasm.

litasm is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public Licenses as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

litasm is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY: without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with axasm (see LICENSE.TXT). 
If not, see http://www.gnu.org/licenses/.

If a non-GPL license is desired, contact the author.

This is the retargetable assembler core file (header)

***********************************************************************/
#ifndef _SOLOASM_H
#define _SOLOASM_H

#include <cstdint>
#include <map>
#include <vector>
#include <string>

class litasm
{
 protected:
    virtual unsigned genasm(unsigned pass) { err=-1; return -1; }
 public:
  unsigned int memsize;
  uint8_t *ary;
  unsigned int psize;
  unsigned int begin;
  unsigned int end;
  unsigned int err;
  litasm() {err=0; psize=32; ary=NULL;    };
  virtual void dopass(int pnum) { genasm(pnum); }
  std::map<std::string,unsigned int> symtab;
};


extern litasm *asmobj;

extern litasm *asmobj_create();

#ifdef _SOLO_MAIN
extern char * __listtext;
#else
char * __listtext;
#endif
#endif
