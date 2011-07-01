/*============================================================================*/
/*       1         2         3         4         5         6         7        */
/*3456789012345678901234567890123456789012345678901234567890123456789012345678*/
/*============================================================================*/
/*                                             .                              */
/*                                               RRRR WW  WW   WTTTTTTHH  HH  */
/*                                               RR RR WW WWW  W  TT  HH  HH  */
/*      Header   :                               RRRR   WWWWWWWW  TT  HHHHHH  */
/*                                               RR RR   WWW WWW  TT  HH  HH  */
/*      Module   :                               RR  R    WW  WW  TT  HH  HH  */
/*                                                                            */
/*      Project  :  Vista                          Rheinisch-Westfaelische    */
/*                                               Technische Hochschule Aachen */
/*      Purpose  :                                                            */
/*                                                                            */
/*                                                 Copyright (c)  1998-2008   */
/*                                                 by  RWTH-Aachen, Germany   */
/*                                                 All rights reserved.       */
/*                                             .                              */
/*============================================================================*/
/*                                                                            */
/*    THIS SOFTWARE IS PROVIDED 'AS IS'. ANY WARRANTIES ARE DISCLAIMED. IN    */
/*    NO CASE SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DAMAGES.    */
/*    REDISTRIBUTION AND USE OF THE NON MODIFIED TOOLKIT IS PERMITTED. OWN    */
/*    DEVELOPMENTS BASED ON THIS TOOLKIT MUST BE CLEARLY DECLARED AS SUCH.    */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/*      CLASS DEFINITIONS:                                                    */
/*                                                                            */
/*        - CSpatialDfn : IVdfnActionObject                                   */
/*                                                                            */
/*============================================================================*/
/*
 * $Id: SiamSpatialDfn.h,v 1.3 2009/03/03 10:09:51 dr165799 Exp $
 */

#ifndef __SPATIALDFN_H
#define __SPATIALDFN_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <VistaDataFlowNet/VdfnActionObject.h>

#include <VistaBase/VistaVectorMath.h>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaSystem;
class VistaTransformNode;
class VistaSG;
class VistaGeometry;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class KinectHandler : public IVdfnActionObject
{
public:
	KinectHandler( VistaSystem* pVistaSystem );
	~KinectHandler();

	bool SetPosition( const std::vector<VistaVector3D>& vecPos );
	bool SetTransformation( const VistaTransformMatrix& matTransform );

	void SetIsEnabled( const bool bSet );
	bool GetIsEnabled();

	REFL_DECLARE;

private: 
	void CreateElement( int nElement, VistaGeometry* pGeom, const VistaVector3D& v3Offset );
private:
	VistaTransformNode*					m_pManRoot;
	VistaSG*							m_pSceneGraph;
	std::vector<VistaTransformNode*>	m_vecJoints;
	VistaTransformMatrix				m_matTransform;
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/
#endif //__SPATIALDFN_H
