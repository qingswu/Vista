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
#include <string>
using namespace std;

#include "VistaMutex.h"
#include "Imp/VistaMutexImp.h"


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

VistaMutex::VistaMutex ()
{
	m_pImp = IVistaMutexImp::CreateMutexImp("", IVistaMutexImp::eIntraProcess);
}

VistaMutex::VistaMutex(const string &sName)
{
	/** @TODO fixme - inter- and intraprocess mixed! */
	m_pImp = IVistaMutexImp::CreateMutexImp(sName, IVistaMutexImp::eInterProcess);
}

VistaMutex::VistaMutex ( const VistaMutex & )
{
}

VistaMutex & VistaMutex::operator=   ( const VistaMutex & )
{
	return *this;
}

VistaMutex::~VistaMutex ()
{
	delete m_pImp;
}

void VistaMutex::Lock ()
{
	m_pImp->Lock();
}

void VistaMutex::Unlock ()
{
	m_pImp->Unlock();
}


bool VistaMutex::TryLock ()
{
	return m_pImp->TryLock();
}







