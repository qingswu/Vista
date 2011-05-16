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

#ifndef _VISTACYBERGLOVEDRIVER_H__
#define _VISTACYBERGLOVEDRIVER_H__

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include "../VistaDeviceDriversConfig.h"
#include "../VistaDeviceDriver.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

class VistaDriverConnectionAspect;
class VistaCyberGloveProtocol;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

/**
 * a driver for Immersion's CyberGlove. Should be pretty complete, according
 * to the manual of the device. Was only tested on an 18-sensor variant, but
 * should work with other models as well.
 * The connection is expected to be a serial line connection, as it twiddles
 * around with timeouts needed for serial line communications. Other types
 * of connections may not work properly.
 *
 *
 * @todo add more documentation, PLEAZE
 */
class VISTADEVICEDRIVERSAPI VistaCyberGloveDriver : public IVistaDeviceDriver
{
public:

	enum VCG_SAMPLE_FREQ
	{
		VCG_FREQ_1HZ    = 1,
		VCG_FREQ_2HZ    = 2,
		VCG_FREQ_10HZ   = 10,
		VCG_FREQ_15HZ   = 15,
		VCG_FREQ_30HZ   = 30,
		VCG_FREQ_45HZ   = 45,
		VCG_FREQ_60HZ   = 60,
		VCG_FREQ_75HZ   = 75,
		VCG_FREQ_100HZ  = 100,
		VCG_FREQ_MAX    = 0,
		VCG_FREQ_CUSTOM
	};

	enum VCG_ERROR_CODE
	{
		VCG_ERROR_NONE = 0,		// no error

		VCG_ERROR_UNKNOWN,		// unknown error
		VCG_ERROR_GLOVE,		// error code 'eg': Glove not plugged in.
		VCG_ERROR_SAMPLING		// error code 'es': Sampling rate too fast.
	};

	struct _sCyberGloveSample
	{
		char m_cRecord[35];

		int m_nSensorsInSample;
		int m_nSensorMask;

		bool m_bIncStatusByte;
		bool m_bIncTimestamp;

		VCG_ERROR_CODE m_eErrorCode;
	};

	VistaCyberGloveDriver();

	virtual ~VistaCyberGloveDriver();

	//*****************************
	// Public Configuration Methods
	//*****************************

	//--------------------
	// Light Configuration
	//--------------------

	/// Returns the current state of the small light on the wrist of the glove
	/// The State can either be set via software or using the switch on the wrist
	/// If in streaming mode this method always returns false if the status byte is not used
	/// This helps prevent synch problems
	bool GetLightState(void) const;

	/// Allows to set the state of the small light on the glove via software
	/// Using this command makes the light independent of the switch (see also Switch Configuration)
	bool SetLightState(bool bState);

	//---------------------
	// Switch Configuration
	//---------------------

	/// Tells the Cyberglove whether the light on the glove should be linked to the switch (bMode = true)
	/// or whether light and switch should be independent (bMode = false)
	/// Default Mode is bMode = true;
	bool SetSwitchLightMode(bool bMode);

	/// Returns the current switch-light mode (see above)
	bool GetSwitchLightMode(void);

	/// Returns the current state of the switch on the wrist of the glove
	/// The State can either be set via software or using the switch on the wrist
	/// If in streaming mode this method always returns false if the status byte is not used
	/// This helps prevent synch problems
	bool GetSwitchState(void);

	/// Allows to set the state of the switch on the glove via software
	/// This command can only be executed if streaming mode is disabled (sync problems)
	bool SetSwitchState(bool bState);

	//-----------------------
	// Format of Data-Packets
	//-----------------------

	/// Choose the format of a data packet
	/// Data Packet: <Sensor Values> [Timestamp] [Status]
	/// Use this method to include or exclude timestamp and status data
	/// The status byte contains information about the CyberGlove connection, the light
	/// and the switch state (see p. 50)
	bool SetPacketFormat(bool bStatus,bool bTimestamp);

	/// The sensor mask indicates which sensors of the glove are reported
	/// This mask is combined internally with the hardware sensor mask, so
	/// no invalid sensors can be enabled. Bit 0 stands for Sensor #1 and
	/// Bit 23 for Sensor #24 (see p.20)
	bool SetSensorMask(int nMask);

	/// Retrieves the current sensor mask from the CGIU
	/// If the CyberGlove is in use, the last value is returned
	long GetSensorMask();

	/// This method allows the user to include only the nNumSensors first sensor
	/// values in each data packet even if more sensors are enabled in the sensor mask
	bool SetNumberOfSensors(int nNumSensors);

	/// Returns the number of sensor values that are included in each data packet
	/// If the CGIU is in use, the last known value is returned
	int GetNumberOfSensors();

	/// Returns the number of sensors of the DataGlove
	/// This value never changes, so it only needs to be retrieved from the CGIU once
	int GetMaxNumberOfSensors();

	//-----------------------
	// Hardware Configuration
	//-----------------------

	/// Enabling digital filtering results in a more stable signal from the sensors
	/// It adds about 0.126ms of lag
	/// See also p.29
	bool SetDigitalFilter(bool bMode);

	/// Restarts the CGIU Firmware and resets all attributes to
	/// driver default values. A restart takes about 2 seconds.
	bool Reset();

	/// Sets CyberGlove configuration to driver default settings
	bool RestoreDefaultSettings();

	//-------------------
	// Sampling Frequency
	//-------------------

	/// Sets the sampling frequency to one of the predefined values
	/// VCG_FREQ_CUSTOM is not allowed, if you want to specify your own freuqency
	/// use SetSamplingFrequency(int w1, int w2)
	bool SetSamplingFrequency(VCG_SAMPLE_FREQ nFreq);

	/// If you need a sampling frequency that is not included in the list
	/// of predefined frequencies, you can can specify your own values for the counters
	/// w1 and w2. The CGIU counts from w1 to 0 and does so w2 times before the next data
	/// packet is sent out (see p. 47)
	bool SetSamplingFrequency(int w1, int w2);

	/// Returns the current Sampling Frequency
	/// The return value VCG_FREQ_CUSTOM means that the user specified their own
	/// values for the counters w1 and w2
	VCG_SAMPLE_FREQ GetSamplingFrequency(void) const;

	/// Returns the sampling period counter values w1 and w2 (see above)
	bool GetSamplingFrequency(int * w1, int * w2) const;

	//-----------------------------
	// Streaming Mode vs. Poll Mode
	//-----------------------------

	/// Sets the method used to retrieve data packets from the CGIU
	/// Normally Streaming mode is more efficient, because the streaming mode command
	/// needs only to be sent once.
	/// If errors occur it may be useful to switch to poll mode, where each data packet has
	/// to be "ordered" individually
	bool SetStreamingMode(bool bMode);

	//-----------------------------
	// Driver Creation
	//-----------------------------

	static IVistaDriverCreationMethod *GetDriverFactoryMethod();

	//-----------------------------
	// IVistaDeviceDriver API
	//-----------------------------
	bool Connect();

	unsigned int GetSensorMeasureSize() const
	{
		return sizeof(_sCyberGloveSample);
	}

protected:
	bool DoSensorUpdate(microtime dTs);

private:
	bool    m_bInitDone;			// Hardware init successful?
	bool    m_bStreamingMode;		// Using streaming or poll mode?
	bool    m_bTempStreamingMode;

	bool    m_bIncStatusByte;		// Include status byte in data packets?
	bool    m_bIncTimestamp;		// Include timestamp?
	bool    m_bSwitchLightMode;		// Switch controls light?
	bool    m_bFilterMode;			// Enable digital filter?

	int     m_nNumSensors;			// Number of sensor values in one data packet
	int     m_nSensorMask;			// Copy of current sensor mask
	int     m_nMaxSensors;			// Max number of sensors in glove

	VCG_SAMPLE_FREQ	m_nSampleFreq;	// Predefined sample frequency
	int     m_nSampleW1;			// Sample period counter 1
	int     m_nSampleW2;			// Sample period counter 2

	VistaDriverConnectionAspect *		m_pConnectionAspect;

#ifdef WIN32
	// tweak: methods needed to enabled connection triggered driver updates under windows
	void _preCommunicate() const;
	void _postCommunicate() const;
#endif

};

#endif //_VISTACYBERGLOVEDRIVER_H__
