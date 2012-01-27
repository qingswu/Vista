/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
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

#include "VistaEnvironment.h"

#include <stdlib.h>
#ifdef WIN32
#define SYSTEM "WIN32"
#include <Windows.h>
#pragma warning(disable: 4996)
#elif IRIX
#define SYSTEM "IRIX"
#elif LINUX
#define SYSTEM "LINUX"
#elif SUNOS
#define SYSTEM "SUNOS"
#else
#define SYSTEM "HPUX"
#endif
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

static std::string CheckedReturn(char *pcString)
{
	if(pcString)
		return std::string(pcString);
	else
		return std::string("");
}


/*============================================================================*/
/* CONSTRUCTORS                                                               */
/*============================================================================*/

VistaEnvironment::VistaEnvironment()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/


std::string VistaEnvironment::GetCompileFlagSYSTEM()
{
	return SYSTEM;
};


std::string VistaEnvironment::GetCompileTime()
{
	return __TIME__;
};


std::string VistaEnvironment::GetCompileDate()
{
	return __DATE__;
};

std::string VistaEnvironment::GetHostname()
{
#ifdef WIN32
	return CheckedReturn(getenv("Computername"));
#else
	return CheckedReturn(getenv("HOSTNAME"));
#endif
}


std::string VistaEnvironment::GetOSystem()
{
#ifdef WIN32
	return CheckedReturn(getenv("OS"));
#else
	std::string a = CheckedReturn(getenv("OSNAME"));
	a = a + " " + CheckedReturn(getenv("R_OSVERSION"));
	return a;
#endif
}

std::string VistaEnvironment::GetCPUType()
{
#ifdef WIN32
	return CheckedReturn(getenv("PROCESSOR_ARCHITECTURE"));
#else
	return CheckedReturn(getenv("R_HWARCH"));
#endif
}

std::string VistaEnvironment::GetNumberOfProcessors()
{
#ifdef WIN32
	return CheckedReturn(getenv("NUMBER_OF_PROCESSORS"));
#else
	return CheckedReturn(getenv("R_HW_NPROC"));
#endif
}


std::string VistaEnvironment::GetMemory()
{
#ifdef WIN32
	return CheckedReturn((getenv("REALMEM")==0)?("No entry"):(getenv("REALMEM")));
#else
	return CheckedReturn(getenv("R_HW_REALMEM"));
#endif
}

std::string VistaEnvironment::GetEnv(const std::string &sEnv)
{
	return CheckedReturn(getenv(sEnv.c_str()));
};

void VistaEnvironment::SetEnv(const std::string &sKey, const std::string &sValue)
{
	putenv((char*)(sKey+std::string("=")+sValue).c_str());
};

std::string VistaEnvironment::GetLibraryPathEnv()
{
#if defined WIN32 
	return CheckedReturn( getenv( "PATH" ) );
#elif defined DARWIN
	return CheckedReturn( getenv( "DYLD_LIBRARY_PATH" ) );
#else // UNIX
	return CheckedReturn( getenv( "LD_LIBRARY_PATH" ) );
#endif
};

void VistaEnvironment::SetLibraryPathEnv( const std::string &sValue )
{
#if defined WIN32 
	putenv( ( "PATH=" + sValue ).c_str() );
#elif defined DARWIN
	putenv( ( "DYLD_LIBRARY_PATH=" + sValue ).c_str() );
#else // UNIX
	putenv( (char*)( "LD_LIBRARY_PATH=" + sValue ).c_str() );
#endif
};

void VistaEnvironment::AddPathToLibraryPathEnv( const std::string &sValue, bool bAddAtBack )
{
	/** @todo check Darwin format */
#if defined WIN32
	std::string sNewPath = getenv( "PATH" );
	if( bAddAtBack )
		sNewPath = sNewPath + ";" + sValue;
	else
		sNewPath = sValue + ";" + sNewPath;
	putenv( ( "PATH=" + sNewPath ).c_str() );
#elif defined DARWIN
	std::string sNewPath = getenv( "DYLD_LIBRARY_PATH" );
	if( bAddAtBack )
		sNewPath = sNewPath + ":" + sValue;
	else
		sNewPath = sValue + ":" + sNewPath;
	putenv( ( "DYLD_LIBRARY_PATH=" + sNewPath ).c_str() );
#else // UNIX
	std::string sNewPath = GetEnv( "LD_LIBRARY_PATH" );
	if( bAddAtBack )
		sNewPath = sNewPath + ":" + sValue;
	else
		sNewPath = sValue + ":" + sNewPath;
	SetEnv( "LD_LIBRARY_PATH", sNewPath );
#endif
}

char VistaEnvironment::GetOSEnvironmentPathSeparator()
{
#ifdef WIN32
	return ';';
#else
	return ':';
#endif
}

std::string VistaEnvironment::ReplaceOSEnvironemntPathSeparators( const std::string& sPathListEntry,
														const char cReplaceWith )
{
	std::string sReturn( sPathListEntry );
	const char cSep = GetOSEnvironmentPathSeparator();
	for( std::string::iterator itChar = sReturn.begin();
			itChar != sReturn.end(); ++itChar )
	{
		if( (*itChar) == cSep )
			(*itChar) = cReplaceWith;
	}
	return sReturn;
}




