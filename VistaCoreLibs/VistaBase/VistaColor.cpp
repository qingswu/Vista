#include "VistaColor.h"

//// Constructors

VistaColor::VistaColor(
	int iColor /*= WHITE */, EFormat eFormat /*= FORMAT_RGB*/ )
{
	SetValues(iColor, eFormat);
}

VistaColor::VistaColor(int iR, int iG, int iB, int iA /*=255*/ )
{
	SetValues(iR, iG, iB, iA);
}

VistaColor::VistaColor(float fR, float fG, float fB, float fA /*=1.f*/ )
{
	SetValues(fR, fG, fB, fA);
}

VistaColor::VistaColor(const float afValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	SetValues(afValues, eFormat);
}

VistaColor::VistaColor(
	const double adValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	SetValues(adValues, eFormat);
}

VistaColor::VistaColor(const int aiValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	SetValues(aiValues, eFormat);
}

VistaColor VistaColor::CreateFromHSL(float fH, float fS, float fL)
{
	VistaColor oColor;
	oColor.SetHSLValues(fH, fS, fL);
	return oColor;
}

VistaColor VistaColor::CreateFromHSL(const float a3fValues[3])
{
	VistaColor oColor;
	oColor.SetHSLValues(a3fValues);
	return oColor;
}

VistaColor VistaColor::CreateFromHSL(const double a3dValues[3])
{
	VistaColor oColor;
	oColor.SetHSLValues(a3dValues);
	return oColor;
}


VistaColor VistaColor::CreateFromHSV(float fH, float fS, float fV)
{
	VistaColor oColor;
	oColor.SetHSVValues(fH, fS, fV);
	return oColor;
}

VistaColor VistaColor::CreateFromHSV(const float a3fValues[3])
{
	VistaColor oColor;
	oColor.SetHSVValues(a3fValues);
	return oColor;
}

VistaColor VistaColor::CreateFromHSV(const double a3dValues[3])
{
	VistaColor oColor;
	oColor.SetHSVValues(a3dValues);
	return oColor;
}

//// Setters

void VistaColor::SetValues(int iR, int iG, int iB, int iA /*= 255*/)
{
	m_a4fValues[0] = iR / 255.f;
	m_a4fValues[1] = iG / 255.f;
	m_a4fValues[2] = iB / 255.f;
	m_a4fValues[3] = iA / 255.f;
}

void VistaColor::SetValues(float fR, float fG, float fB, float fA /*= 1.f */)
{
	m_a4fValues[0] = fR;
	m_a4fValues[1] = fG;
	m_a4fValues[2] = fB;
	m_a4fValues[3] = fA;
}

void VistaColor::SetValues(
	const float afValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	m_a4fValues[0] = afValues[0];
	m_a4fValues[1] = afValues[1];
	m_a4fValues[2] = afValues[2];

	switch (eFormat)
	{
	case RGB:
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[3] = afValues[3];
		break;
	}
}

void VistaColor::SetValues(
	const double adValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	m_a4fValues[0] = static_cast<float>(adValues[0]);
	m_a4fValues[1] = static_cast<float>(adValues[1]);	
	m_a4fValues[2] = static_cast<float>(adValues[2]);

	switch (eFormat)
	{
	case RGB:
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[3] = static_cast<float>(adValues[3]);
		break;
	}
}

void VistaColor::SetValues(int iColor, EFormat eFormat /*= FORMAT_RGB*/ )
{
	switch (eFormat)
	{
	case RGB:
		m_a4fValues[0] = ((iColor & 0xFF0000) >> 16) / 255.0f;
		m_a4fValues[1] = ((iColor & 0x00FF00) >> 8) / 255.0f;
		m_a4fValues[2] = ( iColor & 0x0000FF) / 255.0f;
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[0] = ((iColor & 0xFF000000) >> 24) / 255.0f;
		m_a4fValues[1] = ((iColor & 0x00FF0000) >> 16) / 255.0f;
		m_a4fValues[2] = ((iColor & 0x0000FF00) >> 8) / 255.0f;
		m_a4fValues[3] =  (iColor & 0x000000FF) / 255.0f;
		break;
	}
}

void VistaColor::SetValues(
	const int aiValues[], EFormat eFormat /*= FORMAT_RGB*/)
{
	m_a4fValues[0] = aiValues[0] / 255.f;
	m_a4fValues[1] = aiValues[1] / 255.f;
	m_a4fValues[2] = aiValues[2] / 255.f;

	switch (eFormat)
	{
	case RGB:
		m_a4fValues[3] = 1.f;
		break;
	case RGBA:
		m_a4fValues[3] = aiValues[3] / 255.f;
		break;
	}
}

void VistaColor::SetHSLValues(float fH, float fS, float fL)
{
	float a3fHSL[3] = { fH, fS, fL };
	SetHSLValues(a3fHSL);
}

void VistaColor::SetHSLValues(const float a3fValues[3])
{
	HSLtoRGB(a3fValues, m_a4fValues);
	m_a4fValues[3] = 1.f;
}

void VistaColor::SetHSLValues(const double a3dValues[3])
{
	float a3fHSL[3] = {
		static_cast<float>(a3dValues[0]),
		static_cast<float>(a3dValues[1]),
		static_cast<float>(a3dValues[2]) };
	HSLtoRGB(a3fHSL, m_a4fValues);
	m_a4fValues[3] = 1.f;
}

void VistaColor::SetHSVValues(float fH, float fS, float fV)
{
	float a3fHSV[3] = { fH, fS, fV };
	SetHSVValues(a3fHSV);
}

void VistaColor::SetHSVValues(const float a3fValues[3])
{
	HSVtoRGB(a3fValues, m_a4fValues);
	m_a4fValues[3] = 1.f;
}

void VistaColor::SetHSVValues(const double a3dValues[3])
{
	float a3fHSV[3] = {
		static_cast<float>(a3dValues[0]),
		static_cast<float>(a3dValues[1]),
		static_cast<float>(a3dValues[2]) };
	HSVtoRGB(a3fHSV, m_a4fValues);
	m_a4fValues[3] = 1.f;
}

void VistaColor::SetRed(float fR)
{
	m_a4fValues[0] = fR;
}

void VistaColor::SetGreen(float fG)
{
	m_a4fValues[1] = fG;
}

void VistaColor::SetBlue(float fB)
{
	m_a4fValues[2] = fB;
}

void VistaColor::SetAlpha(float fA)
{
	m_a4fValues[3] = fA;
}

void VistaColor::SetHue(float fHue)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[0] = fHue;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetHSLSaturation(float fSaturation)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[1] = fSaturation;
	HSLtoRGB(a3fHSL, m_a4fValues);
}

void VistaColor::SetLightness(float fLightness)
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3fHSL[2] = fLightness;
	HSLtoRGB(a3fHSL, m_a4fValues);
}


void VistaColor::SetHSVSaturation(float fSaturation)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[1] = fSaturation;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

void VistaColor::SetValue(float fValue)
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3fHSV[2] = fValue;
	HSVtoRGB(a3fHSV, m_a4fValues);
}

//// Getters

void VistaColor::GetValues(
	int aiValues[], EFormat eFormat /*= FORMAT_RGB*/) const
{
	aiValues[0] = static_cast<int>(m_a4fValues[0] * 255.f);
	aiValues[1] = static_cast<int>(m_a4fValues[1] * 255.f);
	aiValues[2] = static_cast<int>(m_a4fValues[2] * 255.f);
	if (eFormat == RGBA)
	{
		aiValues[3] = static_cast<int>(m_a4fValues[3] * 255.f);
	}
}

void VistaColor::GetValues( 
	float afValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	afValues[0] = m_a4fValues[0];
	afValues[1] = m_a4fValues[1];
	afValues[2] = m_a4fValues[2];

	if (eFormat == RGBA)
	{
		afValues[3] = m_a4fValues[3];
	}
}

void VistaColor::GetValues( 
	double adValues[], EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	adValues[0] = m_a4fValues[0];
	adValues[1] = m_a4fValues[1];
	adValues[2] = m_a4fValues[2];
	if (eFormat == RGBA)
	{
		adValues[3] = m_a4fValues[3];
	}
}

int VistaColor::GetValues( EFormat eFormat /*= FORMAT_RGB*/ ) const
{
	switch (eFormat)
	{
	case RGB:
		return (static_cast<int>(m_a4fValues[0] * 255.f) << 16)
			+ (static_cast<int>(m_a4fValues[1] * 255.f) << 8)
			+ static_cast<int>(m_a4fValues[2] * 255.f);
	case RGBA:
		return (static_cast<int>(m_a4fValues[0] * 255.f) << 24)
			+ (static_cast<int>(m_a4fValues[1] * 255.f) << 16)
			+ (static_cast<int>(m_a4fValues[2] * 255.f) << 8)
			+ static_cast<int>(m_a4fValues[3] * 255.f);
	default:
		return 0;
	}
}

void VistaColor::GetHSLValues(float a3fValues[3]) const
{
	RGBtoHSL(m_a4fValues, a3fValues);
}

void VistaColor::GetHSLValues(double a3dValues[3]) const
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	a3dValues[0] = a3fHSL[0];
	a3dValues[1] = a3fHSL[1];
	a3dValues[2] = a3fHSL[2];
}

void VistaColor::GetHSVValues(float a3fValues[3]) const
{
	RGBtoHSV(m_a4fValues, a3fValues);
}

void VistaColor::GetHSVValues(double a3dValues[3]) const
{
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	a3dValues[0] = a3fHSV[0];
	a3dValues[1] = a3fHSV[1];
	a3dValues[2] = a3fHSV[2];
}

float VistaColor::GetRed() const
{
	return m_a4fValues[0];
}

float VistaColor::GetGreen() const
{
	return m_a4fValues[1];
}

float VistaColor::GetBlue() const
{
	return m_a4fValues[2];
}

float VistaColor::GetAlpha() const
{
	return m_a4fValues[3];
}

float VistaColor::GetHue() const
{
	float a3fHSL[3];
	RGBtoHSL(m_a4fValues, a3fHSL);
	return a3fHSL[0];
}

float VistaColor::GetHSLSaturation() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	float fL = (fMax + fMin) / 2.f; // Lightness

	float fS;

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		fS = 0.f;
	}
	else //Chromatic data...
	{
		if (fL < .5f)
			fS = fDelta / (fMax + fMin); // Saturation
		else
			fS = fDelta / (2.f - fMax - fMin); // Saturation
	}

	return fS;
}

float VistaColor::GetLightness() const
{
	float fMin = std::min(std::min(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Min. value of RGB
	float fMax = std::max(std::max(
		m_a4fValues[0], m_a4fValues[1]), m_a4fValues[2]); // Max. value of RGB

	return (fMax + fMin) / 2.f;
}

float VistaColor::GetHSVSaturation() const
{
	// @todo: implement direct calculation of the saturation
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[1];
}

float VistaColor::GetValue() const
{
	// @todo: implement direct calculation of the value
	float a3fHSV[3];
	RGBtoHSV(m_a4fValues, a3fHSV);
	return a3fHSV[2];
}

//// Operations

VistaColor VistaColor::Mix( 
	const VistaColor& v3End, float fFraction /*= .f5*/) const
{
	float fOwn = 1.0f - fFraction;
	return VistaColor( fOwn * m_a4fValues[0] + fFraction * v3End.m_a4fValues[0],
		fOwn * m_a4fValues[1] + fFraction * v3End.m_a4fValues[1],
		fOwn * m_a4fValues[2] + fFraction * v3End.m_a4fValues[2],
		fOwn * m_a4fValues[3] + fFraction * v3End.m_a4fValues[3]);
}

void VistaColor::ClampValues()
{
	for (int i = 0; i < 4; ++i)
	{
		if (m_a4fValues[i] < 0.f) m_a4fValues[i] = 0.f;
		if (m_a4fValues[i] > 1.f) m_a4fValues[i] = 1.f;
	}
}

VistaColor& VistaColor::AddRGB(const VistaColor &oOther)
{
	m_a4fValues[0] += oOther.m_a4fValues[0];
	m_a4fValues[1] += oOther.m_a4fValues[1];
	m_a4fValues[2] += oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::SubtractRGB(const VistaColor &oOther)
{
	m_a4fValues[0] -= oOther.m_a4fValues[0];
	m_a4fValues[1] -= oOther.m_a4fValues[1];
	m_a4fValues[2] -= oOther.m_a4fValues[2];
	return (*this);
}

VistaColor& VistaColor::MultiplyRGB(const float fScale)
{
	m_a4fValues[0] *= fScale;
	m_a4fValues[1] *= fScale;
	m_a4fValues[2] *= fScale;
	return (*this);
}

VistaColor& VistaColor::DivideRGB(const float fScale)
{
	m_a4fValues[0] /= fScale;
	m_a4fValues[1] /= fScale;
	m_a4fValues[2] /= fScale;
	return (*this);
}

//// static methods

float VistaColor::HueToRGB(float f1, float f2, float fHue)
{
	if (fHue < 0.f)
		fHue += 1.f;
	else if (fHue > 1.f)
		fHue -= 1.f;

	float fReturn;

	if ((6.f * fHue) < 1.f)
		fReturn = f1 + (f2 - f1) * 6.f * fHue;
	else if ((2.f * fHue) < 1.f)
		fReturn = f2;
	else if ((3.f * fHue) < 2.f)
		fReturn = f1 + (f2 - f1) * ((2.f / 3.f) - fHue) * 6.f;
	else
		fReturn = f1;

	return fReturn;
}

void VistaColor::HSLtoRGB(const float a3fHSL[3], float a3fRGB[3])
{
	float fH, fS, fL;
	fH = a3fHSL[0];
	fS = a3fHSL[1];
	fL = a3fHSL[2];

	if (fS == 0.0)
	{
		// saturation == 0 so use set rgb values to luminance value
		a3fRGB[0] = fL;
		a3fRGB[1] = fL;
		a3fRGB[2] = fL;
	}
	else
	{
		float f2;

		if (fL < .5f)
		{
			f2 = fL * (1.f + fS);
		}
		else
		{
			f2 = (fL + fS) - (fS * fL);
		}

		float f1 = 2.f * fL - f2;

		a3fRGB[0] = HueToRGB(f1, f2, fH + (1.f/3.f));
		a3fRGB[1] = HueToRGB(f1, f2, fH);
		a3fRGB[2] = HueToRGB(f1, f2, fH - (1.f/3.f));
	}
}

void VistaColor::RGBtoHSL(const float a3fRGB[3], float a3fHSL[3])
{
	float fR, fG, fB;
	fR = a3fRGB[0];
	fG = a3fRGB[1];
	fB = a3fRGB[2];

	float fMin = std::min(std::min(fR, fG), fB);    //Min. value of RGB
	float fMax = std::max(std::max(fR, fG), fB);    //Max. value of RGB
	float fDelta = fMax - fMin;             //Delta RGB value

	a3fHSL[2] = (fMax + fMin) / 2.f; // Luminance

	if (fDelta == 0.f)		//This is a gray, no chroma...
	{
		a3fHSL[0] = 0.f;	// Hue
		a3fHSL[1] = 0.f;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (a3fHSL[2] < .5f)
			a3fHSL[1] = fDelta / (fMax + fMin); // Saturation
		else
			a3fHSL[1] = fDelta / (2.f - fMax - fMin); // Saturation

		float deltaR = (((fMax - fR) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaG = (((fMax - fG) / 6.f) + (fDelta / 2.f)) / fDelta;
		float deltaB = (((fMax - fB) / 6.f) + (fDelta / 2.f)) / fDelta;

		if (fR == fMax )
			a3fHSL[0] = deltaB - deltaG; // Hue
		else if (fG == fMax)
			a3fHSL[0] = (1.f / 3.f) + deltaR - deltaB; // Hue
		else if (fB == fMax)
			a3fHSL[0] = (2.f / 3.f) + deltaG - deltaR; // Hue

		if (a3fHSL[0] < 0.f)
			a3fHSL[0] += 1.f; // Hue
		else if (a3fHSL[0] > 1.f)
			a3fHSL[0] -= 1.f; // Hue
	}
}

void VistaColor::HSLtoHSV(const float a3fHSL[3], float a3fHSV[3])
{
	a3fHSV[0] = a3fHSL[0];
	float fL = a3fHSL[2] * 2.f;
	float fS = a3fHSL[1] * ((fL <= 1.f) ? fL : 2.f - fL);
	a3fHSV[2] = (fL + fS) / 2.f;
	if (fL + fS > 0.f)
	{
		a3fHSV[1] = (2.f * fS) / (fL + fS);
	}
	else
	{
		a3fHSV[1] = 0.f;
	}
}

void VistaColor::HSVtoHSL(const float a3fHSV[3], float a3fHSL[3])
{
	a3fHSL[0] = a3fHSV[0];
	float fL = (2.f - a3fHSV[1]) * a3fHSV[2];
	a3fHSL[1] = a3fHSV[1] * a3fHSV[2];
	if (fL > 0.f)
	{
		a3fHSL[1] /= (fL <= 1.f) ? fL : (2.f - fL);
	}
	else
	{
		a3fHSL[1] = 0.f;
	}
	a3fHSL[2] = fL / 2.f;
}

void VistaColor::HSVtoRGB(const float a3fHSV[3], float a3fRGB[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	HSVtoHSL(a3fHSV, a3fHSL);
	HSLtoRGB(a3fHSL, a3fRGB);
}

void VistaColor::RGBtoHSV(const float a3fRGB[3], float a3fHSV[3])
{
	// @todo: implement direct conversion
	float a3fHSL[3];
	RGBtoHSL(a3fRGB, a3fHSL);
	HSLtoHSV(a3fHSL, a3fHSV);
}
