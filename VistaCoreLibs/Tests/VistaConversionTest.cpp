/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2014 RWTH Aachen University               */
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


/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/

#include <gtest/gtest.h>

#include <VistaTestingUtils/VistaTestingCompare.h>
#include <VistaTestingUtils/VistaTestingRandom.h>

#include <VistaAspects/VistaConversion.h>

namespace
{
	static const int S_nNumIterations = 100;
	static const char S_acSeparators[] = { ',', ';', '|', ':' };
	static const float S_nStringFloatErrorTolerance = 0.0001f;
}


template<typename T>
inline void TestStringConversion()
{
	SCOPED_TRACE( "StringConversion<" + VistaConversion::GetTypeName<T>() + ">" );

	for( int i = 0; i < S_nNumIterations; ++i )
	{
		SCOPED_TRACE( "Iteration: " + VistaConversion::ToString( i ) );
		char cSeparator = S_acSeparators[ VistaTesting::RandomInt(3) ];
		T oOrig = VistaTesting::GenerateRandom<T>();
		T oNew;
		std::string sValue = VistaConversion::ToString( oOrig, cSeparator );
		ASSERT_TRUE( VistaConversion::FromString( sValue, oNew, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( oNew, oOrig, S_nStringFloatErrorTolerance ) );
	}
}


template<typename T>
inline void TestStringConversionContainers()
{
	SCOPED_TRACE( "StringListConversion<" + VistaConversion::GetTypeName<T>() + ">" );

	for( int i = 0; i < S_nNumIterations; ++i )
	{
		SCOPED_TRACE( "Iteration: " + VistaConversion::ToString( i ) );
		char cSeparator = S_acSeparators[ VistaTesting::RandomInt(3) ];
		int nSize = VistaTesting::RandomInt( 20 );
		std::list<T> liTest, liTestRes;
		std::vector<T> vecTest, vecTestRes;
		std::deque<T> deqTest, deqTestRes;
		std::stack<T> staTest, staTestRes;
		std::queue<T> queTest, queTestRes;
		T* aTest = new T[nSize];
		T* aTestRes = new T[nSize];
		for( int i = 0; i < nSize; ++i )
		{
			T oTemp = VistaTesting::GenerateRandom<T>();
			liTest.push_back( oTemp );
			vecTest.push_back( oTemp );
			deqTest.push_back( oTemp );
			staTest.push( oTemp );
			queTest.push( oTemp );
			aTest[i] = oTemp;
		}

		std::string sListValue = VistaConversion::ToString( liTest, cSeparator );
		std::string sVecValue = VistaConversion::ToString( vecTest, cSeparator );
		std::string sDeqValue = VistaConversion::ToString( deqTest, cSeparator );
		std::string sStackValue = VistaConversion::ToString( staTest, cSeparator );
		std::string sQueueValue = VistaConversion::ToString( queTest, cSeparator );
		std::string sArrayValue = VistaConversion::ArrayToString( aTest, nSize, cSeparator );

		ASSERT_EQ( sListValue, sVecValue );
		ASSERT_EQ( sListValue, sDeqValue );
		ASSERT_EQ( sListValue, sStackValue );
		ASSERT_EQ( sListValue, sQueueValue );
		ASSERT_EQ( sListValue, sArrayValue );

		SCOPED_TRACE( "Value: " + sListValue );

		ASSERT_TRUE( VistaConversion::FromString( sListValue, liTestRes, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( liTest, liTestRes, S_nStringFloatErrorTolerance ) );
		
		ASSERT_TRUE( VistaConversion::FromString( sVecValue, vecTestRes, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( vecTest, vecTestRes, S_nStringFloatErrorTolerance ) );
		
		ASSERT_TRUE( VistaConversion::FromString( sDeqValue, deqTestRes, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( deqTest, deqTestRes, S_nStringFloatErrorTolerance ) );
		
		ASSERT_TRUE( VistaConversion::FromString( sStackValue, staTestRes, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( staTest, staTestRes, S_nStringFloatErrorTolerance ) );
		
		ASSERT_TRUE( VistaConversion::FromString( sQueueValue, queTestRes, cSeparator ) );
		ASSERT_TRUE( VistaTesting::Compare( queTest, queTestRes, S_nStringFloatErrorTolerance ) );
		
		ASSERT_TRUE( VistaConversion::ArrayFromString( sArrayValue, aTestRes, nSize, cSeparator ) );
		ASSERT_TRUE( VistaTesting::CompareArray( aTest, aTestRes, nSize, S_nStringFloatErrorTolerance ) );	

		delete[] aTest;
		delete[] aTestRes;
	}
}

/*============================================================================*/
/* TESTS                                                                      */
/*============================================================================*/


TEST( VistaConversionTest, StringConversions_float )
{
	TestStringConversion<float>();
	TestStringConversionContainers<float>();
}

TEST( VistaConversionTest, StringConversions_double )
{
	TestStringConversion<double>();
	TestStringConversionContainers<double>();
}

TEST( VistaConversionTest, StringConversions_int )
{
	TestStringConversion<int>();
	TestStringConversionContainers<int>();
}

TEST( VistaConversionTest, StringConversions_long )
{
	TestStringConversion<long>();
	TestStringConversionContainers<long>();
}

TEST( VistaConversionTest, StringConversions_short )
{
	TestStringConversion<short>();
	TestStringConversionContainers<short>();
}

TEST( VistaConversionTest, StringConversions_unsigned_int )
{
	TestStringConversion<unsigned int>();
	TestStringConversionContainers<unsigned int>();
}

TEST( VistaConversionTest, StringConversions_bool )
{
	TestStringConversion<bool>();
	TestStringConversionContainers<bool>();
}

// Chars and strings can't be efficiently stored in containers,
// since they themselve can represent whitespaces or separators
TEST( VistaConversionTest, StringConversions_char )
{
	TestStringConversion<char>();
}

TEST( VistaConversionTest, StringConversions_string )
{
	TestStringConversion<std::string>();
}


// MathTypes are lists already, so they can't be stored in
// containers this way
TEST( VistaConversionTest, StringConversions_VistaVector3D )
{
	TestStringConversion<VistaVector3D>();
}

TEST( VistaConversionTest, StringConversions_VistaTransformMatrix )
{
	TestStringConversion<VistaTransformMatrix>();
}

TEST( VistaConversionTest, StringConversions_VistaQuaternion )
{
	TestStringConversion<VistaQuaternion>();
}

int main( int argc, char** argv )
{
	::testing::InitGoogleTest(&argc, argv);
	unsigned int nSeed = VistaTesting::InitializeRandomSeed();
	SCOPED_TRACE( "RandomSeed: " + VistaConversion::ToString( nSeed ) );
	return RUN_ALL_TESTS();
} 
