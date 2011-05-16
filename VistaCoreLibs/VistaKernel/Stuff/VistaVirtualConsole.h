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

#ifndef _VISTAVIRTUALCONSOLE_H
#define _VISTAVIRTUALCONSOLE_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>
#include <VistaKernel/DisplayManager/VistaSceneOverlay.h>
#include <VistaKernel/InteractionManager/VistaKeyboardSystemControl.h>
#include <VistaKernel/GraphicsManager/VistaGeometry.h> // for color

#include <string>
#include <vector>
#include <deque>

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class VistaEventManager;
class VistaCommandEvent;
class VistaDisplayManager;
class VistaKeyboardSystemControl;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/

class VISTAKERNELAPI VistaVirtualConsole : public IVistaSceneOverlay
{
public:
	enum
	{
		FLG_NONE = 0,         //**< default */
		FLG_ADAPTONRESIZE = 1 //**< adapt font size when window resizes */
	};

	VistaVirtualConsole(VistaEventManager *pMgr,
						 VistaDisplayManager *pDm,
						 VistaKeyboardSystemControl *pCtrl,
						int iCmdToken,
						int nFlags = FLG_NONE,
						int cActivationKey = VISTA_KEY_ESC, //<** KEY_ESC **/
						int iInitialWidth = 640,
						int iInitialHeight = 480,
						const std::string &strViewportName = "");
	virtual ~VistaVirtualConsole();

	/**
	 * OpenGL callback. Do not call this on your own, as this calls GL
	 */
	virtual bool Do ();


	/**
	 * Adds lines to the output buffer. '\t' is escaped by 4 spaces,
	 * '\n' is escaped to single lines. This routine is rather slow,
	 * uses some memcopies and may not handle special characters well.
	 * But it is <b>the</b> method for putting something on screen.
	 * Stick to ASCII characters here.
	 */
	bool Output( const std::string &strOutput,
			const VistaColorRGB &oColor = VistaColorRGB::WHITE );

	/**
	 * adds a string to the input line, null terminated, plz.
	 */
	bool Input(const unsigned char *s);


	enum eCursorDir
	{
		E_CURSOR_RIGHT=0,
		E_CURSOR_LEFT
	};

	enum eHistoryDir
	{
		E_HISTORY_UP=0,
		E_HISTORY_DOWN
	};

	enum eOutputDir
	{
		E_OUTPUT_PAGE_UP=0,
		E_OUTPUT_PAGE_DOWN,
	};

	/**
	 * move cursor into the direction as indicated by dir.
	 */
	bool MoveCursor(eCursorDir dir);

	/**
	 * move in history. The current input line is forgotten and
	 * replaced by the command line history indicated by dir
	 */
	bool MoveInHistory(eHistoryDir dir);

	/**
	 * @todo implement MoveInOutput
	 */
	//bool MoveInOutput( eOutputDir dir );

	/**
	 * programmatic delete
	 */
	bool Delete();

	/**
	 * programmatic backspace
	 */
	bool Backspace();

	/**
	 * Set enable / disable state for the gl callback
	 */
	void SetEnabled(bool bEnabled);

	/**
	 * retrieve enable/disable state for this console
	 */
	bool GetEnabled() const;


	/**
	 * change viewport size, call this in case you want to re-scale the
	 * fontsize or adapt to a new vp size manually.
	 */
	bool UpdateViewportChange(int iWidth, int iHeight);

	/**
	 * query the flags given for this console
	 */
	int GetFlags() const;
	void SetFlags( int nFlags );

	/**
	 * query the current string in the input line.
	 */
	const std::string& GetInputLine() const;

	/**
	 * programmatially process key input
	 * @param iKeyCode the ANSI key code to be processed
	 * @param nModState the state of shift/ctrl/alt
			  see vista keyboard handling for details.
	 */
	bool HandleKeyPress( int iKeyCode, int nModState, bool bIsKeyRepeat = false );

	/**
	 * return the determined activation key of this console.
	 */
	int  GetActivationKey() const;


	// coloring
	const VistaColorRGB& GetTextColor() const;
	void SetTextColor( const VistaColorRGB &oColor );

	const VistaColorRGB& GetCursorColor() const;
	void SetCursorColor( const VistaColorRGB &oColor );

	const VistaColorRGB& GetBackgroundColor() const;
	void SetBackgroundColor( const VistaColorRGB &oColor );

	/**
	 *  1: opaque
	 *  0: transparent
	 */
	float GetConsoleTransparency() const;

	/**
	 *  1: opaque
	 *  0: transparent
	 */
	void SetConsoleTransparency( float fTrans );

private:
	class ConsoleKeySink;
	class ToggleConsoleCommand;
	class DisplayChangeObserver;

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// ViSTA stuff

	// event stuff
	VistaEventManager *m_pEvMgr;
	VistaCommandEvent *m_pCmdEvent;

	// display stuff
	DisplayChangeObserver *m_pDispObserver;
	VistaDisplayManager   *m_pDspMgr;

	// key processing
	VistaKeyboardSystemControl                      *m_pCtrl;
	ConsoleKeySink                                  *m_pKeySink;
	VistaKeyboardSystemControl::IVistaDirectKeySink *m_pOldSink;
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// state handling
	bool m_bEnabled,
		 m_bInitDone;
	int  m_nActivationKey; /**< the enable / disable key for the console */
	int  m_nFlags;

	VistaColorRGB m_BackgroudColor,
				   m_TextColor,
				   m_CursorColor;

	// resources
	// GL stuff
	double   m_nCharacterWidth,  /**< width */
			 m_nCharacterHeight; /**< height */
	unsigned int   m_glFontHandle;     /**< texture resource */
	float    m_nTransparency;    /**< 1: totally opaque, 0: fully transparent */

	bool CreateGLConsole();
	bool CreateGLFont();

	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// processing API
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	bool InputChar(char c);
	bool EndOfInput();

	void DetermineInputLineSize();
	void DrawContents();
	void DrawInputLine();

	void DebugPrint() const;

	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// why is this no std::string?
	//typedef std::vector<unsigned char> LINE;
	typedef std::string LINE;

	struct SOutputLine
	{
		SOutputLine( const LINE &oLine, const VistaColorRGB &oColor )
			: m_strLine(oLine),
			m_oColor(oColor)
		{

		}

		LINE           m_strLine;
		VistaColorRGB m_oColor;
	};

	LINE m_strInputLine;
	LINE::iterator m_itCursor;

	typedef std::vector<LINE> HISTORY;
	HISTORY        m_vecHistory;
	HISTORY::const_iterator m_itHistory;

	typedef std::deque<SOutputLine> OUTPUTBUFFER;
	OUTPUTBUFFER  m_vecOutput;
	//OUTPUTBUFFER::const_iterator m_itDrawStart;


	int m_nVpTotalLines,
		m_nVpTotalCols,
		m_nInputLines,
		m_nInputCursorPos,
		m_nOutputCursorPos;
	double m_nCharWidth,
		   m_nCharHeight;

	void DrawString(const char *s,
					const int &nLength,
					const double &x, const double &y,
					const double &w, const double &h, const double &z);
	void DrawCharacter(const int &c,
					const double &x, const double &y,
					const double &w, const double &h, const double &z);

	/**
	 * @return the number of lines used for printing
	 */
	int DrawWrapString(const char *s,
						const int &nLength,
						const double &x, const double &y, const double &w,
						const double &h, const double &z, const int &wrap);

	double GetInputLineAreaHeight() const;
	double GetInputLineAreaHeightBack() const;
	double GetContentAreaHeight() const;
	double GetContentAreaHeightBack() const;
	// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
};

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAVIRTUALCONSOLE_H

