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

#ifndef _VISTABASETYPES_H
#define _VISTABASETYPES_H

#include "VistaBaseConfig.h"

/*============================================================================*/
/* TYPEDEFS                                                                   */
/*============================================================================*/

#if !defined(WIN32)
 #if defined(HANDLE)
  #error HANDLE already defined?
 #else
  typedef long unsigned int HANDLE;
 #endif
#else
 #if !defined(HANDLE)
  typedef void* HANDLE;
 #endif
#endif

#if !defined(WIN32)
 #if !defined(SOCKET)
 typedef int SOCKET;
 #endif
#endif




namespace VistaType
{
	typedef signed int              sint32;
	typedef unsigned int            uint32;
	typedef unsigned short		  ushort16;
	typedef short			      sshort16;
	typedef float                  float32;
	typedef unsigned char           ubyte8;
	typedef char                     byte8;
	typedef signed long long        sint64;
	typedef unsigned long long      uint64;
	typedef double                 float64;

	typedef double  microtime;   /**< sec.msec since arbitrary point in time      */
	typedef double  systemtime;  /**< sec.msec since 01.01.1970
													 (less precise than VistaType::microtime)*/
	typedef uint64  microstamp;  /**< rtc or register value                       */

	/**
	 * a union datatype of 32 bits width
	 *
	 * may be used for automatic conversion into unsigned integer
	 * which is the most suitable datatype for bytewise manipulation
	 * (like byteorder swapping, etc.)
	 */
	union VISTABASEAPI val32
	{
		/// constructor for automatic conversion of float values
		val32( const VistaType::float32 init ) : asFloat( init ) {};

		/// constructor for automatic conversion of long values
		val32( const VistaType::sint32  init ) : asSignedInt( init ) {};

		/// explicit conversion constructor (only for internal use)
		val32( const VistaType::uint32  init ) : asUnsignedInt( init ) {};

		VistaType::float32        asFloat;
		VistaType::sint32         asSignedInt;
		VistaType::uint32         asUnsignedInt;
		unsigned char  asBytes[4];
	};

	/**
	 * a union datatype of 64 bits width
	 *
	 * may be used for automatic conversion into unsigned integer
	 * which is the most suitable datatype for bytewise manipulation
	 * (like byteorder swapping, etc.)
	 */
	union VISTABASEAPI val64
	{
		/// constructor for automatic conversion of double values
		val64( const float64 init ) : asFloat( init ) {};

		/// constructor for automatic conversion of long long values
		val64( const sint64  init ) : asSignedInt( init ) {};

		/// explicit conversion constructor (only for internal use)
		explicit val64( const uint64  init ) : asUnsignedInt( init ) {};


		float64        asFloat;
		sint64         asSignedInt;
		uint64         asUnsignedInt;
		unsigned char  asBytes[8];
	};

} // namespace VistaType

#endif // _VISTABASETYPES_H
