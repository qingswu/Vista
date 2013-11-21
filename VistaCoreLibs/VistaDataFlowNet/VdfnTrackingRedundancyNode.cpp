/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2013 RWTH Aachen University               */
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

#include "VdfnTrackingRedundancyNode.h" 

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnTrackingRedundancyNode::VdfnTrackingRedundancyNode()
: m_pInPosMain(NULL)
, m_pInOriMain(NULL)
, m_pInPosSecondary(NULL)
, m_pInOriSecondary(NULL)
, m_pOutPos( new TVdfnPort<VistaVector3D> )
, m_pOutOri( new TVdfnPort<VistaQuaternion> )
, m_pOutValid( new TVdfnPort<bool> )
, m_pOutComputed( new TVdfnPort<bool> )
, m_bLastTransformValid(false)
{
	RegisterInPortPrototype( "position_main", 
		new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "orientation_main", 
		new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );
	RegisterInPortPrototype( "position_secondary", 
		new TVdfnPortTypeCompare<TVdfnPort<VistaVector3D> > );
	RegisterInPortPrototype( "orientation_secondary", 
		new TVdfnPortTypeCompare<TVdfnPort<VistaQuaternion> > );

	RegisterOutPort( "position", m_pOutPos );
	RegisterOutPort( "orientation", m_pOutOri );
	RegisterOutPort( "valid", m_pOutValid );
	RegisterOutPort( "computed", m_pOutComputed );
}
VdfnTrackingRedundancyNode::~VdfnTrackingRedundancyNode()
{
}

/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VdfnTrackingRedundancyNode::GetIsValid() const
{
	// everything may be NULL and it's valid; the output then defaults to
	// the last known positions and orientations
	return true;
}

bool VdfnTrackingRedundancyNode::PrepareEvaluationRun()
{
	m_pInPosMain = dynamic_cast<TVdfnPort<VistaVector3D>*>( GetInPort( "position_main" ) );
	m_pInOriMain = dynamic_cast<TVdfnPort<VistaQuaternion>*>( GetInPort( "orientation_main" ) );
	m_pInPosSecondary = dynamic_cast<TVdfnPort<VistaVector3D>*>( GetInPort( "position_secondary" ) );
	m_pInOriSecondary = dynamic_cast<TVdfnPort<VistaQuaternion>*>( GetInPort( "orientation_secondary" ) );
	return GetIsValid();
}

bool VdfnTrackingRedundancyNode::DoEvalNode()
{
	// get in data
	VistaVector3D v3Main;
	if(m_pInPosMain != NULL)
		v3Main = m_pInPosMain->GetValueConstRef();
	VistaQuaternion qMain;
	if(m_pInOriMain != NULL)
		qMain = m_pInOriMain->GetValueConstRef();
	VistaVector3D v3Sec;
	if(m_pInPosSecondary != NULL)
		v3Sec = m_pInPosSecondary->GetValueConstRef();
	VistaQuaternion qSec;
	if(m_pInOriSecondary != NULL)
		qSec = m_pInOriSecondary->GetValueConstRef();

	VistaVector3D& v3OutPos = m_pOutPos->GetValueRef();
	VistaQuaternion& qOutOri = m_pOutOri->GetValueRef();
	bool& bOutValid = m_pOutValid->GetValueRef();
	bool& bOutComputed = m_pOutComputed->GetValueRef();

	// if main data available: just forward that
	if(m_pInPosMain != NULL && m_pInOriMain != NULL)
	{
		v3OutPos = v3Main;
		qOutOri = qMain;
		bOutValid = true;
		bOutComputed = false;
		m_pOutPos->IncUpdateCounter();
		m_pOutOri->IncUpdateCounter();
		m_pOutValid->IncUpdateCounter();
		m_pOutComputed->IncUpdateCounter();
	}
	// if main data is not available, but secondary is: calculate main data
	// from secondary data
	else if( (m_pInPosMain == NULL || m_pInOriMain == NULL) &&
		(m_pInPosSecondary != NULL && m_pInOriSecondary != NULL))
	{
		// transform matrix not valid? calculate it
		if(!m_bLastTransformValid)
		{
			VistaTransformMatrix oTransLastMain(m_qLastMain, m_v3LastMain);
			VistaTransformMatrix oTransLastSec(m_qLastSecondary, m_v3LastSecondary);

			m_matLastTransform = oTransLastSec.GetInverted() * oTransLastMain;
			m_bLastTransformValid = true;
		}

		// calculate main data 
		m_v3LastMain = m_matLastTransform.Transform(v3Sec);
		m_qLastMain = m_matLastTransform.Transform(qSec);
		v3OutPos = m_v3LastMain;
		qOutOri = m_qLastMain;

		// save available data
		m_v3LastSecondary = v3Sec;
		m_qLastSecondary = qSec;

		// additional info
		bOutValid = true;
		bOutComputed = true;

		m_pOutPos->IncUpdateCounter();
		m_pOutOri->IncUpdateCounter();
		m_pOutValid->IncUpdateCounter();
		m_pOutComputed->IncUpdateCounter();
	}
	// if neither main nor secondary data is available, forward last data we have
	else
	{
		v3OutPos = m_v3LastMain;
		qOutOri = m_qLastMain;
		bOutValid = false;
		bOutComputed = false;

		m_pOutPos->IncUpdateCounter();
		m_pOutOri->IncUpdateCounter();
		m_pOutValid->IncUpdateCounter();
		m_pOutComputed->IncUpdateCounter();
	}



	// if all data is available, save it in case the tracking is lost next time
	if(m_pInPosMain != NULL && m_pInOriMain != NULL &&
		m_pInPosSecondary != NULL && m_pInOriSecondary != NULL)
	{
		m_v3LastMain = v3Main;
		m_qLastMain = qMain;
		m_v3LastSecondary = v3Sec;
		m_qLastSecondary = qSec;

		// transform matrix is not valid anymore, but we don't calculate it
		// here, because we probably don't need it
		m_bLastTransformValid = false;
	}

	
	

	return true;
}
/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

