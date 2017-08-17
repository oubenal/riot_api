/*
 *  $Id: styles.h,v 1.3 2007/07/02 18:48:19 sebdiaz Exp $
 *
 *  Copyright (C) 1996 - 2004 Stephen F. Booth
 *                       2007 Sebastien DIAZ <sebastien.diaz@gmail.com>
 *  Part of the GNU cgicc library, http://www.gnu.org/software/cgicc
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA 
 */
#ifndef _STYLES_H_
#define _STYLES_H_ 1

#include <string>

const std::string styles = 
"body, html {width: 100%; height: 100%; margin: 0;}\n"
".leftpane {width: 25%;height: 100%;float: left;background-color: darkslategray;border-collapse: collapse;padding: 0px 70px;}\n"
".middlepane {width: 50%;height: 100%;float: left;background-color: royalblue;border-collapse: collapse;padding: 0px 70px;}\n"
".match{width: 50%;height: 100%;float: left;background-color: brown;border-collapse: collapse;padding: 0px 70px;border : double;}\n"
".rightpane {width: 25%;height: 100%;position: relative;float: right;background-color: yellow;border-collapse: collapse;}\n"
".toppane {width: 100%;height: 100px;border-collapse: collapse;background-color: #4da6ff;}\n"
"\n"
;

#endif /* _STYLES_H_ */
