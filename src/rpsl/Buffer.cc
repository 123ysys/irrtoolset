//
// Copyright (c) 2001,2002                        RIPE NCC
//
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in
// supporting documentation, and that the name of the author not be
// used in advertising or publicity pertaining to distribution of the
// software without specific, written prior permission.
//
// THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
// AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
// DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
// AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$
//
// Author: Ramesh Govindan <govindan@isi.edu>
//         Cengiz Alaettinoglu
//         WeeSan Lee <wlee@isi.edu>
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <iomanip>
#include <cctype>
#include <ostream>

using namespace std;

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif // HAVE_STRINGS_H

#include "Buffer.hh"

ostream &operator<<(ostream &os, const Buffer &b)
{
  os << setw(b.getSize()) << b.getContents() << " (" << b.getSize() << ")";
  return os;
}

void Buffer::extend(unsigned long minExtend) {
   assert(!callerAllocated);	// !!!
   if (capacity + BufferExtendIncrement > size + minExtend) {
      capacity = capacity + BufferExtendIncrement;
   } else {
      capacity = size + minExtend;
   }
   contents = (char *)realloc(contents, capacity);
}

void 
Buffer::append(const char *buf, unsigned long sz)
{
   if (size + sz + 1 > capacity)
      extend(sz);
   
   memmove(contents+size, buf, sz+1);
   size += sz;
}

void Buffer::appendf(const char *format, ...) {
   if (size + BufferExtendIncrement / 2 > capacity)
      extend();

   va_list ap;
    
   va_start(ap, format);
#ifdef HAVE_VSNPRINTF
   (void) vsnprintf(contents + size, capacity - size, format, ap);
#else
   (void) vsprintf(contents + size, format, ap);
#endif
   va_end(ap);
   size += strlen(contents + size);
}

void Buffer::append(Buffer &buf) {
   if (size + buf.size > capacity)
      extend(buf.size);
   memcpy(contents + size, buf.contents, buf.size);
   size += buf.size;
}

void Buffer::insert(Buffer &buf, unsigned long atOffset) {
   if (size + buf.size > capacity)
      extend(buf.size);
   memmove(contents + atOffset + buf.size, 
	   contents + atOffset, 
	   size - atOffset);
   memcpy(contents + atOffset, buf.contents, buf.size);
   size += buf.size;
}

void Buffer::flush(unsigned long sz, unsigned long atOffset) {
   if (sz == 0 || size == 0)
      return;

   assert(atOffset + sz <= size);

   memmove(contents + atOffset, contents + atOffset + sz, size - atOffset - sz);

   size -= sz;
   if (offset >= atOffset)
      offset = (offset >= atOffset + sz) ? offset-sz : atOffset;
}

bool Buffer::contains(const char *needle) { // search for needle in buffer
   if (capacity > size) {
      *(contents + size) = 0;
      return strstr(contents, needle);
   }

   char last = *(contents + size - 1);
   *(contents + size - 1) = 0;

   if (strstr(contents, needle)) {
      *(contents + size - 1) = last;
      return true;
   }

   char last2 = *(needle + strlen(needle) - 1);
   if (last != last2) {
      *(contents + size - 1) = last;
      return false;
   }

   char *needle2 = strdup(needle);
   *(needle2 + strlen(needle2) - 1) = 0;

   if (strstr(contents + size - strlen(needle), needle2)) {
      *(contents + size - 1) = last;
      free(needle2);
      return true;
   }

   *(contents + size - 1) = last;
   free(needle2);
   return false;
}

void Buffer::toLowerCase() {
   for (char *p = contents; p < contents + size; ++p)
      if (isascii(*p) && isupper(*p))
         *p = tolower(*p);
}

// 
//  Copyright (c) 1994 by the University of Southern California.
//  All rights reserved.
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  irrtoolset@cs.usc.edu.
//
//  Author(s): Ramesh Govindan <govindan@isi.edu>

