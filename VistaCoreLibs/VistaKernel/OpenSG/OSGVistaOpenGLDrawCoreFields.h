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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGVistaOpenGLDrawCoreFIELDS_H_
#define _OSGVistaOpenGLDrawCoreFIELDS_H_
#ifdef __sgi
#pragma once
#endif

#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(push)
#pragma warning(disable: 4127)
#pragma warning(disable: 4189)
#pragma warning(disable: 4231)
#pragma warning(disable: 4267)
#endif
#include <OpenSG/OSGConfig.h>

#include <OpenSG/OSGFieldContainerPtr.h>
#include <OpenSG/OSGNodeCoreFieldDataType.h>

#include <OpenSG/OSGMaterialDrawableFields.h>
#ifdef WIN32
// disable warnings from OpenSG
#pragma warning(pop)
#endif

OSG_BEGIN_NAMESPACE

class VistaOpenGLDrawCore;

#if !defined(OSG_DO_DOC)   // created as a dummy class, remove to prevent doubles
//! VistaOpenGLDrawCorePtr

typedef FCPtr<MaterialDrawablePtr, VistaOpenGLDrawCore> VistaOpenGLDrawCorePtr;

#endif

#if !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)
#if !defined(OSG_DOC_DEV_TRAITS)
/*! \hideinhierarchy */
#endif

template <>
struct FieldDataTraits<VistaOpenGLDrawCorePtr> :
    public FieldTraitsRecurseMapper<VistaOpenGLDrawCorePtr, true>
{
    static DataType             _type;

    enum                        { StringConvertable = 0x00 };
    enum                        { bHasParent        = 0x01 };

    static DataType   &getType (void) { return _type;        }

    static const char *getSName(void) { return "SFVistaOpenGLDrawCorePtr"; }
    static const char *getMName(void) { return "MFVistaOpenGLDrawCorePtr"; }
};

#if !defined(OSG_DOC_DEV_TRAITS)
/*! \class  FieldTraitsRecurseMapper<VistaOpenGLDrawCorePtr, true>
    \hideinhierarchy
 */
#endif

#endif // !defined(OSG_DO_DOC) || (OSG_DOC_LEVEL >= 3)


#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS)
typedef SField<VistaOpenGLDrawCorePtr> SFVistaOpenGLDrawCorePtr;
#endif


#if !defined(OSG_DO_DOC) || defined(OSG_DOC_FIELD_TYPEDEFS)
typedef MField<VistaOpenGLDrawCorePtr> MFVistaOpenGLDrawCorePtr;
#endif

OSG_END_NAMESPACE

#define OSGVistaOpenGLDrawCoreFIELDS_HEADER_CVSID "@(#)$Id: OSGVistaOpenGLDrawCoreFields.h 23493 2011-09-22 16:12:15Z dr165799 $"

#endif /* _OSGVistaOpenGLDrawCoreFIELDS_H_ */
