/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2009 RWTH Aachen University               */
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
// $Id: VistaDeSerializer.h 4719 2009-09-10 09:29:58Z tbeer $
#ifndef _VISTAAUTOBUFFERPOOL_H_
#define _VISTAAUTOBUFFERPOOL_H_

#include "VistaAutoBuffer.h"
#include "VistaBaseConfig.h"
/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTABASEAPI VistaAutoBufferPool
{
public:
	VistaAutoBufferPool( size_t nNumBufsPreAlloced, size_t nBufDefSize, bool resizeOnNeed = true );
	~VistaAutoBufferPool();

	VistaAutoWriteBuffer GetBufferWrite();
	size_t GetCurrentNumOfBuffers() const;

private:
	std::vector<VistaAutoWriteBuffer> m_pool;
	std::vector<VistaAutoWriteBuffer>::const_iterator SearchNextAvailable() const;
	bool m_bAdaptSize;
};

#endif /* _VISTAAUTOBUFFERPOOL_H_ */


/*============================================================================*/
/* END OF FILE                                                                */
/*============================================================================*/