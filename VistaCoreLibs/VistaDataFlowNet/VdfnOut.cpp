/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2011 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

#include "VdfnOut.h"
#include <iostream>
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
std::ostream* VdfnOut::m_out = &std::cout;
std::ostream* VdfnOut::m_err = &std::cerr;
std::ostream* VdfnOut::m_log = &std::clog;

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/
std::ostream& VdfnOut::out()
{
	return *m_out;
}

std::ostream& VdfnOut::err()
{
	return *m_err;
}

std::ostream& VdfnOut::log()
{
	return *m_log;
}
void VdfnOut::SetOutStream(std::ostream * ostream)
{
	m_out = ostream;
}

void VdfnOut::SetErrorStream(std::ostream * ostream)
{
	m_err = ostream;
}

void VdfnOut::SetLogStream(std::ostream * ostream)
{
	m_log = ostream;
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

