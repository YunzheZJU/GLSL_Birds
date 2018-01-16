/*************************************************************************\

  Copyright 2007 Zhejiang University.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
   fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL ZHEJIANG UNIVERSITY HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  ZHEJIANG UNIVERSITY SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  Authors:  Min Tang, Ruo-feng Tong
  EMail:    {tang_m, trf}@zju.edu.cn

\**************************************************************************/



// textfile.cpp
//
// simple reading and writing for text files
//
//////////////////////////////////////////////////////////////////////

#include "textfile.h"

char *textFileRead(const char *fn) {
    FILE *fp;
    char *content = nullptr;
    int count = 0;

    if (fn != nullptr) {
        fp = fopen(fn, "r");
        if (fp != nullptr) {
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            if (count > 0) {
                content = (char *) malloc(sizeof(char) * (count + 1));
                count = fread(content, sizeof(char), static_cast<size_t>(count), fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}
