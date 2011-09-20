/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2010 RWTH Aachen University               */
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

#include "VistaDTrackDriver.h"

#include <VistaDeviceDriversBase/VistaDeviceDriversOut.h>


#include <VistaDeviceDriversBase/DriverAspects/VistaDriverConnectionAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverSensorMappingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverMeasureHistoryAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverLoggingAspect.h>
#include <VistaDeviceDriversBase/DriverAspects/VistaDriverProtocolAspect.h>


#include <VistaInterProcComm/Connections/VistaConnection.h>
#include <VistaInterProcComm/Connections/VistaByteBufferDeSerializer.h>
#include <VistaInterProcComm/Connections/VistaCSVDeSerializer.h>

#include <VistaAspects/VistaAspectsUtils.h>
#include <VistaBase/VistaTimeUtils.h>

#include <string>
#include <iostream>
#include <assert.h>


using namespace std;

namespace
{
	unsigned int DATACON = 0;
	unsigned int CTRLCON = 1;
}

class IVistaDTrackProtocol
{
public:
	virtual bool SendAttachString(VistaConnection *pCon) const = 0;
	virtual bool SendDetachString(VistaConnection *pCon) const = 0;
	virtual bool SendEnableString(VistaConnection *pCon) const = 0;
	virtual bool SendDisableString(VistaConnection *pCon) const = 0;
	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const = 0;
};

class VistaDTrack1Protocol : public IVistaDTrackProtocol
{
public:
	virtual bool SendAttachString(VistaConnection *pCon) const
	{
		pCon->WriteRawBuffer("dtrack 10 3\0",13);
		VistaTimeUtils::Sleep(100);

		return true;
	}

	virtual bool SendDetachString(VistaConnection *pCon) const
	{
			// stop tracking data output
		pCon->WriteRawBuffer("dtrack 32\0",11);
		pCon->WaitForSendFinish();

		// stop measurement
		pCon->WriteRawBuffer("dtrack 10 0\0",13);
		pCon->WaitForSendFinish();

		// wait a bit just for the sake of it
		VistaTimeUtils::Sleep(100);

		return true;
	}

	virtual bool SendEnableString(VistaConnection *pCon) const
	{
		if(pCon->WriteRawBuffer("dtrack 31\0", 10)==10)
		{
			VistaTimeUtils::Sleep(100);
			return true;
		}
		return false;
	}

	virtual bool SendDisableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack 32\0", 10)==10);
		VistaTimeUtils::Sleep(100);
		return true;
	}

	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const
	{
		// not supported by dtrack1
		return false;
	}
};

class VistaDTrack2Protocol : public IVistaDTrackProtocol
{
public:
	virtual bool SendAttachString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 init\0", 13) == 13);
	}

	virtual bool SendDetachString(VistaConnection *pCon) const
	{
		if( SendDisableString(pCon) )
			pCon->WaitForSendFinish(); // no information given
		return true;
	}

	virtual bool SendEnableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 tracking start\0", 23) == 23);
	}

	virtual bool SendDisableString(VistaConnection *pCon) const
	{
		return (pCon->WriteRawBuffer("dtrack2 tracking stop\0", 22) == 22);
	}

	virtual bool SendStatusString(VistaConnection *pCon, bool &bIdle) const
	{
		if(pCon->WriteRawBuffer( "dtrack2 get status active\0", 26) == 26)
		{
			std::string strAnswer;
			pCon->ReadDelimitedString( strAnswer ); // read until 0x00 or failure

			// should evaluate the answer, how?
			return true;
		}
		return false;
	}
};

class VistaDTrackProtocolAspect : public IVistaDriverProtocolAspect
{
public:
	VistaDTrackProtocolAspect( VistaDTrackDriver *pDriver )
		: m_pDriver( pDriver ),
		  m_pProtocol(NULL)
	{
	}

	bool SetProtocol( const _cVersionTag &oTag )
	{
		if(m_pDriver)
		{
			if(VistaAspectsComparisonStuff::StringEquals( oTag.m_strProtocolName, "dtrack1", false ))
			{
				return ((m_pProtocol = new VistaDTrack1Protocol) != NULL);
			}
			else if(VistaAspectsComparisonStuff::StringEquals( oTag.m_strProtocolName, "dtrack2", false ))
			{
				return ((m_pProtocol = new VistaDTrack2Protocol) != NULL);
			}
		}

		return false;
	}

	IVistaDTrackProtocol *GetProtocol() const
	{
		return m_pProtocol;
	}

private:
	VistaDTrackDriver *m_pDriver;
	IVistaDTrackProtocol *m_pProtocol;
};
/*============================================================================*/
/* MACROS AND DEFINES, CONSTANTS AND STATICS, FUNCTION-PROTOTYPES             */
/*============================================================================*/


// ############################################################################
// DECODER API
// ############################################################################

static bool ReadNDoubles(IVistaDeSerializer *pDeSer,
			unsigned int nCount,
			VistaSensorMeasure::MEASUREVEC &vecOut,
			unsigned int &nOffset)
{
	VistaDTrackMeasures::sGenericMeasure *dField = (VistaDTrackMeasures::sGenericMeasure*)&vecOut[0];
	for(unsigned int n=0; n < nCount; ++n)
	{
		pDeSer->ReadDouble(dField->m_anField[nOffset++]);
	}
	return true;
}


class ILineDecode
{
public:
	ILineDecode(unsigned int nMarkerType)
		: m_nMarkerType(nMarkerType)
	{
	}

	virtual ~ILineDecode() {}

	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut,
			unsigned int nOffset) = 0;

	virtual int ReadMarkerType(VistaByteBufferDeSerializer*,
		 unsigned int &nMarkerType) = 0;

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut) = 0;

	std::vector<int>       m_vecBlockLength;
protected:
	unsigned int m_nMarkerType;
};


class FrDecode : public ILineDecode
{
public:
	FrDecode(unsigned int nMarkerType)
		: ILineDecode(nMarkerType)
	{}


	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer *pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut,
								unsigned int nOffset)
	{
		// fr is read off
		std::string strFrameCnt;
		pDeSer->ReadDelimitedString(strFrameCnt, 0x0d);
		VistaDTrackMeasures::sGlobalMeasure *m = (VistaDTrackMeasures::sGlobalMeasure*)&vecOut[0];

		m->m_nFrameCount = int(VistaAspectsConversionStuff::ConvertToDouble(strFrameCnt));
		return 1;
	}

	virtual int ReadMarkerType(VistaByteBufferDeSerializer*,
			unsigned int &nMarkerType)
	{
		nMarkerType = m_nMarkerType;
		return 0;
	}

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*,
		VistaSensorMeasure::MEASUREVEC &vecOut)
	{
		VistaDTrackMeasures::sGlobalMeasure *m = (VistaDTrackMeasures::sGlobalMeasure*)&vecOut[0];

		m->m_nFrameCount = -1;
		m->m_nTimeStamp  = 1.0;
		return 0;
	}
};

class GenDecode : public ILineDecode
{
public:
	GenDecode(unsigned int nSensorType,
			   const std::vector<int> &vecBlockLengths)
	: ILineDecode(nSensorType),
	  m_pFragDecode(new VistaCSVDeSerializer(' '))
	{
		 m_vecBlockLength = vecBlockLengths;
	}

	~GenDecode()
	{
		delete m_pFragDecode;
	}
	virtual int ReadMarkerType(VistaByteBufferDeSerializer *pDeSer,
			unsigned int &nMarkerType)
	{
		// read marker type has the task to read off the
		// number of samples in this record (hence the name ;)
		// if there are 0 markers in the set, there is no ' '
		// to delimit the end of this.
		// we should link the posix regexp here
		// but for now, we simply copy the read delimited string
		// from the serializer and pimp it to read ' ' OR '\r'
		// which marks the end of the record.
		/** @todo link posix and boost */
		std::string sString;

		char pcTmp[2];
		pcTmp[0] = 0x00;
		pcTmp[1] = 0x00;

		int iLength = 1;
		int iLen = 0; /**< measure length */
		do
		{
			int iRead=0;
			if((iRead=pDeSer->ReadRawBuffer((void*)&pcTmp[0], iLength))==iLength)
			{
				if(pcTmp[0] == ' ' || pcTmp[0] == '\r')
				{
					break; // leave loop
				}
				else
				{
					sString.append(string(&pcTmp[0]));
					++iLen;
				}
			}
			else
			{
				vdderr << "Should read: " << iLength
					<< ", but read: " << iRead << endl;
				break;
			}
		}
		while(true);


		// read off number of sensors read
		unsigned int nNum = VistaAspectsConversionStuff::ConvertToInt(sString);
		nMarkerType = m_nMarkerType;
		return nNum;

	}

	virtual int ReadAllBlocksWithOffset(VistaByteBufferDeSerializer *pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut,
								unsigned int nOffset)
	{
		std::string strBlock, strBeg;
		unsigned int nInitial = nOffset;
		char c;
		std::vector<int>::const_iterator cit = m_vecBlockLength.begin();
		++cit; // skip first token length
		for(; cit != m_vecBlockLength.end(); ++cit)
		{
			// prepare next block
			// read off '['
			pDeSer->ReadRawBuffer(&c, sizeof(char));
			//pDeSer->ReadDelimitedString(strBeg, '[');
			pDeSer->ReadDelimitedString(strBlock, ']');
			// load csv with strBlock
			m_pFragDecode->FillBuffer(strBlock);
			// read block-lengths of doubles
			ReadNDoubles(m_pFragDecode, (*cit), vecOut, nOffset);
		}
		return nOffset - nInitial; // return number of double read
	}

	virtual int ReadSingleBlock(VistaByteBufferDeSerializer*pDeSer,
		VistaSensorMeasure::MEASUREVEC &vecOut)
	{
		std::string strBlock, strBeg;
		pDeSer->ReadDelimitedString(strBeg, '[');
		pDeSer->ReadDelimitedString(strBlock, ']');
		// load csv with strBlock
		m_pFragDecode->FillBuffer(strBlock);
		// read block-lengths of doubles

		unsigned int nOffset = 0;
		ReadNDoubles(m_pFragDecode, (unsigned int)(double(vecOut.size())/sizeof(double)), vecOut, nOffset);

		return nOffset;
	}


private:
	VistaCSVDeSerializer *m_pFragDecode;

};

// ############################################################################
// ############################################################################


// ############################################################################
// ATTACH/DETACH SEQUENCE WHEN CONNECTED
// ############################################################################

class VistaDTrackAttachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaDTrackAttachSequence(VistaDTrackProtocolAspect *pProtocol,
		                       bool bNotAttachOnly = false)
		: m_bSendStartTracking(bNotAttachOnly),
		  m_pProtocol(pProtocol)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		if(!pCon->GetIsOpen())
			if(pCon->Open() == false)
				return false;


		// start tracking
		if(m_bSendStartTracking)
		{
			return (m_pProtocol->GetProtocol()->SendAttachString(pCon));
			//pCon->WriteRawBuffer("dtrack 31\0",11);
		}
		return true;
	}

	bool GetSendStartTracking() const { return m_bSendStartTracking; }
	void SetSendStartTracking(bool bSend) { m_bSendStartTracking = bSend; }

private:
	bool m_bSendStartTracking;
	VistaDTrackProtocolAspect *m_pProtocol;
};

class VistaDTrackDetachSequence : public VistaDriverConnectionAspect::IVistaConnectionSequence
{
public:
	VistaDTrackDetachSequence(VistaDTrackProtocolAspect *pProtocol,
		                       bool bNotDetachOnly = false)
		: m_bSendStopTracking(bNotDetachOnly),
		  m_pProtocol(pProtocol)
	{
	}

	virtual bool operator()(VistaConnection *pCon)
	{
		// nothing to do, we expect this connection to be open
		if(!pCon->GetIsOpen())
			return false;

		if(m_bSendStopTracking)
		{
			m_pProtocol->GetProtocol()->SendDetachString(pCon);
		}
		pCon->Close();
		return !pCon->GetIsOpen();
	}

	bool GetSendStopTracking() const { return m_bSendStopTracking; }
	void SetSendStopTracking(bool bSend) { m_bSendStopTracking = bSend; }
private:
	bool m_bSendStopTracking;
	VistaDTrackProtocolAspect *m_pProtocol;
};


// ############################################################################
// CREATION METHOD
// ############################################################################


VistaDTrackCreationMethod::VistaDTrackCreationMethod(IVistaTranscoderFactoryFactory *metaFac)
: IVistaDriverCreationMethod(metaFac)
{
	RegisterSensorType( "STICK",
		sizeof(VistaDTrackMeasures::sStickMeasure),
		60,
		metaFac->CreateFactoryForType("STICK"));

	RegisterSensorType( "BODY",
		sizeof(VistaDTrackMeasures::sBodyMeasure),
		60,
		metaFac->CreateFactoryForType("BODY"));

	RegisterSensorType( "MARKER",
		sizeof(VistaDTrackMeasures::sMarkerMeasure),
		60,
		metaFac->CreateFactoryForType("MARKER"));

	RegisterSensorType( "GLOBAL",
		sizeof(VistaDTrackMeasures::sGlobalMeasure),
		60,
		metaFac->CreateFactoryForType("GLOBAL"));

	//RegisterSensorType( "MEASURE",
	//	sizeof(_sMeasureMeasure),
	//	60,
	//	metaFac->CreateFactoryForType("MEASURE"));
}

IVistaDeviceDriver *VistaDTrackCreationMethod::CreateDriver()
{
	return new VistaDTrackDriver(this);
}
/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/
VistaDTrackDriver::VistaDTrackDriver(IVistaDriverCreationMethod *crm)
: IVistaDeviceDriver(crm),
  m_pConnection(new VistaDriverConnectionAspect),
  m_pProtocol( NULL ),
  m_pSensors(new VistaDriverSensorMappingAspect(crm)),
  m_vecPacketBuffer(8192),
  m_pDeSerializer(new VistaByteBufferDeSerializer),
  m_pLine(new VistaByteBufferDeSerializer),
  m_nMarkerType(~0),
  m_nGlobalType(~0),
  m_bAttachOnly(false)
{
	SetUpdateType(IVistaDeviceDriver::UPDATE_CONNECTION_THREADED);
	m_pProtocol = new VistaDTrackProtocolAspect(this);
	RegisterAspect( m_pProtocol );

	// prepare connections
	m_pConnection->SetConnection(DATACON, NULL, "DATACONNECTION", true);
	m_pConnection->SetConnection(CTRLCON, NULL, "CONTROLCONNECTION", true);

	m_pConnection->SetAttachSequence(CTRLCON, new VistaDTrackAttachSequence(m_pProtocol, m_bAttachOnly));
	m_pConnection->SetDetachSequence(CTRLCON, new VistaDTrackDetachSequence(m_pProtocol, m_bAttachOnly));

	RegisterAspect(m_pConnection);

	// register sensor type STICK measuring 18 values
	unsigned int nStickType  =  m_pSensors->RegisterType("STICK");
	if( nStickType == ~0 )
		nStickType = m_pSensors->GetTypeId("STICK");

	// register sensor type BODY measuring 17 values (same as stick without button)
	unsigned int nBodyType   =  m_pSensors->RegisterType("BODY");
	if( nBodyType == ~0 )
		nBodyType = m_pSensors->GetTypeId("BODY");

	// register sensort type MARKER catching id,qu and 3dof
	m_nMarkerType =  m_pSensors->RegisterType("MARKER");
	if( m_nMarkerType == ~0 )
		m_nMarkerType = m_pSensors->GetTypeId("MARKER");


	// register type global, catching frame count and (optional) clock
	m_nGlobalType =  m_pSensors->RegisterType("GLOBAL");
	if( m_nGlobalType == ~0 )
		m_nGlobalType = m_pSensors->GetTypeId("GLOBAL");

	unsigned int nMeasureType = m_pSensors->RegisterType("MEASURE");
	if( nMeasureType == ~0 )
		nMeasureType = m_pSensors->GetTypeId("MEASURE");

	RegisterAspect(m_pSensors);

	// create fr decoder
	m_mapDecoder.insert(DECODEMAP::value_type("fr", new FrDecode(m_nGlobalType)));
	// create ts decoder
	m_mapDecoder.insert(DECODEMAP::value_type("ts", new FrDecode(m_nGlobalType)));

	std::vector<int> vTmp;
	vTmp.push_back(2); // 2 doubles for the header
	vTmp.push_back(6); // 6 for the pos / euler frame
	vTmp.push_back(9); // 9 for the rot matrix

	// 6d: normal body
	// [id qu][sx sy sz ex ey ez][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	m_mapDecoder.insert(DECODEMAP::value_type("6d", new GenDecode(nBodyType, vTmp)));

	// 6df: flystick
	// [id qu bt][sx sy sz ex ey ez][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	vTmp[0] = 3; // 3 for the header, the rest is unaltered
	m_mapDecoder.insert(DECODEMAP::value_type("6df", new GenDecode(nStickType, vTmp)));


	// 6dmt, measuring tool
	// [id qu bt][sx sy sz][b0 b1 b2 b3 b4 b5 b6 b7 b8]
	vTmp[1] = 3; // same here for the pos /euler frame
	m_mapDecoder.insert(DECODEMAP::value_type("6dmt", new GenDecode(nMeasureType, vTmp)));

	// 3d: single marker,
	// [id qu][sx sy sz]
	vTmp.clear(); // only two blocks
	vTmp.push_back(2); // 2 first
	vTmp.push_back(3); // then 3 doubles
	m_mapDecoder.insert(DECODEMAP::value_type("3d", new GenDecode(m_nMarkerType, vTmp)));

}

VistaDTrackDriver::~VistaDTrackDriver()
{
	UnregisterAspect(m_pConnection, false);
	delete m_pConnection;

	UnregisterAspect(m_pSensors, false);
	delete m_pSensors;

	UnregisterAspect(m_pProtocol, false);
	delete m_pProtocol;


	// generically delete all sensors
	DeleteAllSensors();


	delete m_pDeSerializer;
	delete m_pLine;

	for(DECODEMAP::iterator it = m_mapDecoder.begin();
		it != m_mapDecoder.end(); ++it)
	{
		delete (*it).second;
	}
}

bool VistaDTrackDriver::DoSensorUpdate(VistaType::microtime dTs)
{
// 	cout << "VistaDTrackDriver::DoSensorUpdate("
//              << dTs << ")" << endl;

	bool bTimeout = false;
	int nRet = m_pConnection->GetCommand(DATACON, (void*)&m_vecPacketBuffer[0], 8192, 5000, &bTimeout);

	if(nRet == -1)
	{
		vddout << "GetCommandFailed()\n";
		return false;
	}

	if(bTimeout)
	{
		vddout << "Timeout()\n";
		return false;
	}

 	//cout << "Read [" << nRet << "] bytes from connection\n";

	(*m_pDeSerializer).SetBuffer((const char*)&m_vecPacketBuffer[0], nRet, false);

	VistaDriverLoggingAspect *pLog
		= static_cast<VistaDriverLoggingAspect*>(
			GetAspectById(VistaDriverLoggingAspect::GetAspectId()));

	//std::vector<double> vTmp(8192);
	bool bRet = false;
	while((*m_pDeSerializer).GetTailSize())
	{
		std::string strLine;
		(*m_pDeSerializer).ReadDelimitedString(strLine, 0x0a);

		if(pLog)
		{
			pLog->Log(dTs, this, strLine);
		}

		//cout << "L: " << strLine << endl;
		(*m_pLine).SetBuffer(strLine.c_str(), (int)strLine.size(), false);

		std::string strType;
		(*m_pLine).ReadDelimitedString(strType, ' ');
		// lookup decoder
		DECODEMAP::iterator it = m_mapDecoder.find(strType);
		if( it != m_mapDecoder.end() )
		{
			// determine number of values to read
			unsigned int nMarkerType = ~0;

			// we read off the first part of the sentence
			// the decoder will store its type information
			// in nMarkerType, if the record contains more than
			// one entry, this value is returned as nNumValues
			int nNumValues = (*(*it).second).ReadMarkerType(m_pLine, nMarkerType);

			if(nMarkerType == m_nGlobalType)
			{
				// this is kinda hacked
				unsigned int nSensorId = ~0;
				// the art protocol does not assign ids to global information
				// like frame count and timestamp, even more unfortunate,
				// the order of the sentences in one record is not guaranteed
				// although it looks as if fr and ts always proceed
				// a record. However, we do want to record that data
				// as a "virtual" sensor, where we map fr count to be sensor 0
				// and ts sensor 1, so the user can decide on using this
				// sensor as input type "GLOBAL"
				if(strType == "fr")
					nSensorId = 0;
				else if(strType == "ts")
					nSensorId = 1;

				unsigned int nMappedId = m_pSensors->GetSensorId(nMarkerType, nSensorId);
				VistaDeviceSensor *pSen = GetSensorByIndex(nMappedId);
				if(pSen)
				{
					m_pHistoryAspect->MeasureStart(pSen, dTs);
					VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
					assert(pM);

					unsigned int nIdx = 0;
					int nSize = (*(*it).second).ReadAllBlocksWithOffset(m_pLine, (*pM).m_vecMeasures, nIdx);
					m_pHistoryAspect->MeasureStop(pSen);
					pSen->SetUpdateTimeStamp(dTs);
					bRet = true;
				}
				// the else case is not critical, as up to now,
				// all global sentences are fed in one block which
				// is clearly separated by the 0xd 0xa ending
			}
			else
			{
				// so far, all other records contain at least a prefix
				// with [id qu ...], we read off this information
				// to a temporary buffer, as we need to know where to
				// store the upcoming information
				VistaSensorMeasure::MEASUREVEC vTmp((*it).second->m_vecBlockLength[0]*sizeof(double));
				unsigned int nMarkerIndex = 0;
				for(int n=0; n < nNumValues; ++n)
				{
					//cout << "Trying to Read sensor [" << n << "]\n";
					// figure out which sensor to use:
					if((*(*it).second).ReadSingleBlock(m_pLine, vTmp) == vTmp.size()/sizeof(double))
					{
						// vTmp[0]: id
						// vTmp[1]: qu
						// vTmp[2]: bt (if (*it).second->m_vecBlockLength() == 3)

						// determine type related index
						VistaDeviceSensor *pSen = NULL;

						if(nMarkerType == m_nMarkerType)
						{
							// markers are special as they have no real id!
							// so we map the markers as they come to the markers
							// as given in the configuration file
							unsigned int nNum = m_pSensors->GetNumRegisteredSensorsForType(nMarkerType);
							if(nMarkerIndex < nNum)
							{
								// ok, we still have a marker left to read
								unsigned int nSensorId = m_pSensors->GetSensorId(nMarkerType, nMarkerIndex);

								// increase the marker index
								nMarkerIndex = (++nMarkerIndex) % (m_pSensors->GetNumRegisteredSensorsForType(nMarkerType)+1);

								// retrieve sensor
								pSen = GetSensorByIndex(nSensorId);
							} // else: we have no more virtual marker left to read
						}
						else
						{
							// no marker, retrieve sensor naturally
							VistaDTrackMeasures::sGenericMeasure *m = (VistaDTrackMeasures::sGenericMeasure*)&vTmp[0];

							unsigned int nSensorId = m_pSensors->GetSensorId(nMarkerType, (unsigned int)m->m_anField[0]);
							// retrieve from internal map
							pSen = GetSensorByIndex(nSensorId);
						}

						// markers are counted 'as seen'
						// so we map them to the index % as many as are registered


						// we will store vTmp[0] and vTmp[1] in the resulting measure
						// of the sensor later on,

						//cout << "Sensor(" << pSen << "; " << nMarkerType << ", " << vTmp[0] << ", " << nMarkerIndex << ")";
						if(pSen)
						{
							 //cout << " -- FOUND\n";

							m_pHistoryAspect->MeasureStart(pSen, dTs);
							VistaSensorMeasure *pM = m_pHistoryAspect->GetCurrentSlot(pSen);
							assert(pM);
							unsigned int nIdx = (unsigned int)(double(vTmp.size())/sizeof(double));
							double *dField = (double*)&vTmp[0];
							VistaDTrackMeasures::sGenericMeasure *m = (VistaDTrackMeasures::sGenericMeasure*)&(*pM).m_vecMeasures[0];

							for(unsigned int l=0; l < nIdx; ++l)
								m->m_anField[l] = dField[l];

							int nSize = (*(*it).second).ReadAllBlocksWithOffset(m_pLine,
															   (*pM).m_vecMeasures,
															   nIdx);

//							for(int i=0; i < (*pM).m_vecMeasures.size(); ++i)
//								std::cout << (*pM).m_vecMeasures[i] << " ";
//							std::cout  << std::endl;

							// we skip two eventually pending whitespace or 0x0d
							// in order to relax the parsing constraint for the
							// next block
//							char c;
//							m_pLine->ReadRawBuffer(&c, sizeof(char));
							m_pHistoryAspect->MeasureStop(pSen);
							pSen->SetUpdateTimeStamp(dTs);
							bRet = true;
						}
						else
						{
							//cout << " -- NOT FOUND\n";

							// we could not find the sensor for this packet,
							// maybe it is not configured, so we simply read
							// the rest of the string off and continue
							VistaSensorMeasure::MEASUREVEC vSkip(32*sizeof(double));
							unsigned int nIdx = 0;
							int nSize = (*(*it).second).ReadAllBlocksWithOffset(m_pLine, vSkip, nIdx);
							char c;
							m_pLine->ReadRawBuffer(&c, sizeof(char));
						}
					}
				}
			}
		}

		//cout << "(TS) : " << (*m_pDeSerializer).GetTailSize() << endl;
		if((*m_pDeSerializer).GetTailSize() == 1)
		{
			//cout << "(TS == 1)\n";
			// the dtrack2 protocol seems to be buggy sometimes
			// as sometimes it is forgotten to remove the trailing
			// '\0' when sending the dtrack ascii packet, so this parser
			// gets confused. However, when the tailsize is 1, no real
			// value is encoded in the size-1-string, so we simply skip it here
			// the 'old' dtrack does not contain this.
			char c;
			(*m_pDeSerializer).ReadRawBuffer(&c, sizeof(char));
		}
	}

	return bRet;
}

bool VistaDTrackDriver::PhysicalEnable(bool bEnable)
{
	if(!m_bAttachOnly) // will not send enable/disable only on specific request
	{
		if(bEnable)
		{
			return m_pProtocol->GetProtocol()->SendEnableString(m_pConnection->GetConnection(1));

		}
		else
		{
			return m_pProtocol->GetProtocol()->SendDisableString(m_pConnection->GetConnection(1));
		}
	}
	return true;
}

bool VistaDTrackDriver::GetAttachOnly() const
{
	return m_bAttachOnly;
}

void VistaDTrackDriver::SetAttachOnly(bool bAttach)
{
	VistaDTrackAttachSequence *pAttach =
		dynamic_cast<VistaDTrackAttachSequence*>(m_pConnection->GetAttachSequence(1));

	if(pAttach)
		pAttach->SetSendStartTracking(!bAttach);

	VistaDTrackDetachSequence *pDetach =
		dynamic_cast<VistaDTrackDetachSequence*>(m_pConnection->GetDetachSequence(1));

	if(pDetach)
		pDetach->SetSendStopTracking(!bAttach);

	m_bAttachOnly = bAttach;
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/
