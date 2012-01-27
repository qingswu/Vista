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

#include "VdfnHistoryProjectNode.h"
#include "VdfnUtil.h"

#include <VistaAspects/VistaAspectsUtils.h>

namespace
{
	enum
	{
		MD_LAZY = 0,
		MD_HOT,
		MD_ITERATE
	};
}

/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VdfnHistoryProjectNode::VdfnHistoryProjectNode(const std::list<std::string> &liOutPorts)
: IVdfnReEvalNode(),
  m_liOutPorts(),
  m_liOriginalPorts(liOutPorts),
  m_pHistory(NULL),
  m_pSamplingMode(NULL),
  m_nUnprocessedMeasures( 0 )
{
	// create initial mapping
	for( std::list<std::string>::const_iterator cit = m_liOriginalPorts.begin();
		cit != m_liOriginalPorts.end(); ++cit)
	{
		m_liOutPorts.push_back( _sPortMap( *cit, *cit ) );
	}
	// we project from a history to a sampled
	// value of specified sample types as given in liOutPorts
	RegisterInPortPrototype( "history", new HistoryPortCompare);

	// 0 for lazy
	// all other for hot
	RegisterInPortPrototype( "sampling_mode", new TVdfnPortTypeCompare<TVdfnPort<int> > );
}

bool VdfnHistoryProjectNode::SetInPort(const std::string &sName, IVdfnPort *pPort)
{
	if( IVdfnNode::SetInPort( sName, pPort ) )
	{
		// ok, it's either history or sampling_mode
		HistoryPort *pHist = dynamic_cast<HistoryPort*>(pPort);
		if(pHist)
		{
			// it was history, dyn-cast should be cheaper than strcmp?
			// we need to update the outport map
			UpdateOutPortMap();
		}
		return true;
	}
	return false;
}

bool VdfnHistoryProjectNode::GetIsValid() const
{
	return (m_pHistory != NULL);
}

bool VdfnHistoryProjectNode::PrepareEvaluationRun()
{
	m_pHistory = VdfnUtil::GetInPortTyped<HistoryPort*>("history", this);
	m_pSamplingMode = VdfnUtil::GetInPortTyped<TVdfnPort<int>*>("sampling_mode", this);

	return GetIsValid();
}

bool VdfnHistoryProjectNode::DoEvalNode()
{
	int nMode = ( m_pSamplingMode ? m_pSamplingMode->GetValue() : 0 );
	const VistaSensorMeasure *pMeasure = NULL;

	switch( nMode )
	{
		case MD_ITERATE:
		{
			// History processing: process oldest sample now and store how many
			// newer samples still need processing
			// The non-zero m_nUnprocessedMeasures will cause GetNeedsReEvaluation
			// to return true
			if( m_nUnprocessedMeasures == 0 ) //no re-evaluation, thus read history
				m_nUnprocessedMeasures = m_pHistory->GetValue()->m_nNewMeasures;

			--m_nUnprocessedMeasures;
			pMeasure = m_pHistory->GetValue()->m_oHistory.GetPast( m_nUnprocessedMeasures );
			break;
		}
		case MD_HOT:
		{
			//Hot sampling: take most current value
			pMeasure = m_pHistory->GetValue()->m_oHistory.GetMostCurrent();
			break;
		}
		default:
		case MD_LAZY:
		{
			//Lazy Sampling: take current value
			pMeasure = m_pHistory->GetValue()->m_oHistory.GetCurrentRead();
			break;
		}
	}

	if( pMeasure == NULL )
		return true; // there just are no samples in the history yet

	for( std::list<_sPortMap>::const_iterator itPort = m_liOutPorts.begin();
			itPort != m_liOutPorts.end(); ++itPort )
	{
		// claim out port from ourselves (by name!)
		IVdfnPort *pOut = GetOutPort( (*itPort).m_strTo );
		if(pOut == NULL) // no? should not happen...
			continue;

		// lookup this outport in order to retrieve the set functor
		std::map<IVdfnPort*, _sSetHlp>::iterator itPortSetter = m_mpPortSetter.find(pOut);

		VdfnPortFactory::CPortSetFunctor *pSetFunctor = (*itPortSetter).second.m_pFunctor;
		if(pSetFunctor)
		{
			// get the property functor for the outport from the transcode of
			// the input history
			// this node implements a bijection from the names of the incoming
			// get functors and the names of the outports
			// note that we are using the transcoder API here called "GetMeasureProperty()".
			IVistaPropertyGetFunctor *pGetFunctor
					= m_pHistory->GetValue()->m_pTranscode->GetMeasureProperty( (*itPort).m_strFrom );

			if(pGetFunctor == NULL)
			{
				vstr::warnp() << "VdfnHistoryProjectNode::DoEvalNode() -- node [" << GetNameForNameable() 
						<< "] attached to non-existent get-prop ["
						<< (*itPort).m_strFrom << "]" << std::endl;
				continue;
			}

			// call set functor for the out port
			(*pSetFunctor).Set( pMeasure, pGetFunctor, pOut, GetUpdateTimeStamp(), (*itPortSetter).second.m_nIndex);
		}// if pSetFunctor
	}
	return true;
}


void VdfnHistoryProjectNode::UpdateOutPortMap()
{
	if(!m_pHistory)
		m_pHistory = VdfnUtil::GetInPortTyped<HistoryPort*>("history", this);

	if(m_pHistory->GetValue()) // check for history set (should be always true,
							   // otherwise it is a driver error (might happen)
	{
		VdfnHistoryPortData       *pData  = m_pHistory->GetValue();
		IVistaMeasureTranscode     *pTrans = pData->m_pTranscode;

		VdfnPortFactory *pFac = VdfnPortFactory::GetSingleton();

		std::list<_sPortMap> liSetOutPort;

		// check on all wanted ports whether we can retrieve a transcode for accessing it
		for(std::list<std::string>::const_iterator cit = m_liOriginalPorts.begin();
			cit != m_liOriginalPorts.end(); ++cit)
		{
			IVistaMeasureTranscode::ITranscodeGet *pGet = pTrans->GetMeasureProperty(*cit);
			if(pGet == NULL) // should not happen when user is sane
			{
				vstr::warnp() << "VdfnHistoryProjectNode::UpdateOutPortMap() -- Measure property ["
						<< *cit << "] doesn't exist" << std::endl;
#if defined(DEBUG)
				std::set<std::string> oSet = pTrans->GetMeasureProperties();
				vstr::IndentObject oIndent;
				for(std::set<std::string>::const_iterator cit1 = oSet.begin();
				    cit1 != oSet.end(); ++cit1 )
				{
					vstr::warnp() << vstr::singleindent << "[" << *cit1 << "]" << std::endl;
				}
#endif
				continue;
			}

			// IAR: hack, for now, we use the type of the transcoder to determine
			// a good port setter, currently, we have two: value get and indexed get as double

			// first: try indexed double get
			IVistaMeasureTranscode::CScalarDoubleGet *pIdxTrans
				= dynamic_cast<IVistaMeasureTranscode::CScalarDoubleGet *>(pGet);
			if(pIdxTrans) // yes...
			{
				// a side note: few getters may be index get getters, so the pIdxTrans will usually
				// evaluate to NULL, one might think it is faster then to check on the ordinary
				// setter then, first. Yes, this might be true, but the UpdateOutPortMap() is
				// called at a <i>very</i> low frequency...

				// add a new outport for every scalar we can get
				for(unsigned int n=0; n < pTrans->GetNumberOfScalars(); ++n)
				{
					// claim an accessor for the property
					VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( pGet->GetReturnType().name() );
					if(pAccess) // worked?
					{
						// create outport
						IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();

						// register outport
						// note the naming scheme: post-fixed index added to the name
						std::string sOutPort = (*cit) + "_" + VistaConversion::ToString( n );
						liSetOutPort.push_back( _sPortMap(*cit, sOutPort) );
						RegisterOutPort( sOutPort, pPort );

//						std::cout << "create port [" << sOutPort << "] at "
//						          << pPort << std::endl;

						// add to lookup for evaluation, this time with a proper index (!= ~0)
						m_mpPortSetter[ pPort ] = _sSetHlp(pAccess->m_pSetFunctor, n);
					}
				}
			}
			else
			{
				// it is no indexed double get, so it must be an 'ordinary' get
				VdfnPortFactory::CPortAccess *pAccess = pFac->GetPortAccess( pGet->GetReturnType().name() );
				if(pAccess)
				{
					// create outport
					IVdfnPort *pPort = pAccess->m_pCreationMethod->CreatePort();

					// register outport
					RegisterOutPort( *cit, pPort );
					liSetOutPort.push_back( _sPortMap(*cit, *cit) );

					// add to lookup for evaluation, note the 'hack', giving ~0 as index
					// will cause the setter to assume a non-indexed transcode.
					m_mpPortSetter[ pPort ] = _sSetHlp(pAccess->m_pSetFunctor, ~0);
				}
			}
		}
		m_liOutPorts = liSetOutPort; // copy the names of ports that are really set
	}

}

bool VdfnHistoryProjectNode::GetNeedsReEvaluation() const
{
	// For the ReEvalNode: if  there are still some unprocessed measures, we
	// need another evaluation run
	return ( m_nUnprocessedMeasures != 0 );
}
/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/


