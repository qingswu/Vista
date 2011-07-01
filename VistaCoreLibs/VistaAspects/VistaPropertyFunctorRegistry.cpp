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

#include "VistaPropertyFunctorRegistry.h"
#include "VistaPropertyFunctor.h"

#include <algorithm>
#include <iostream>

#include "VistaAspectsOut.h"

/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/

/*============================================================================*/
/* CONSTRUCTORS / DESTRUCTOR                                                  */
/*============================================================================*/

class CleanupHelper
{
	public:
	CleanupHelper()
	{
		m_pSingleton = VistaPropertyFunctorRegistry::GetSingleton();
		VistaPropertyFunctorRegistry::refup(m_pSingleton);
	}

	virtual ~CleanupHelper()
	{
		bool bForce=false;
		if(m_pSingleton->getcount() > 1)
		{
			if(m_pSingleton->GetTalkativeFlag())
			{
				vasperr << "CCleanupHelper::~CCleanupHelper() --"
						  << "refcount > 1! (" << m_pSingleton->getcount()
						  << ")\n";
			}
			bForce = true;
		}
		VistaPropertyFunctorRegistry::refdown(m_pSingleton);

		if(bForce)
			delete m_pSingleton;

	}
	void dummy() { int i=0; ++i; }

	VistaPropertyFunctorRegistry *m_pSingleton;
};

//static CCleanupHelper SclHlp;


VistaPropertyFunctorRegistry *VistaPropertyFunctorRegistry::m_pSingleton
	= NULL;


VistaPropertyFunctorRegistry *VistaPropertyFunctorRegistry::GetSingleton()
{
	if(!m_pSingleton)
	{
		m_pSingleton = new VistaPropertyFunctorRegistry();
	}
	return m_pSingleton;
}

VistaPropertyFunctorRegistry::~VistaPropertyFunctorRegistry()
{
	for(GETFMAP::iterator it = m_mpGetters.begin();
		it != m_mpGetters.end(); ++it)
		{
			delete (*it).second; // free memory for functor
		}

	for(SETFMAP::iterator it1 = m_mpSetters.begin();
		it1 != m_mpSetters.end(); ++it1)
		{
			delete (*it1).second; // free memory for functor
		}

}

VistaPropertyFunctorRegistry::VistaPropertyFunctorRegistry()
: m_bTalkative(false)
{
//	SclHlp.dummy(); // reference to trick clever compilers.
}


/*============================================================================*/
/* IMPLEMENTATION                                                             */
/*============================================================================*/

bool VistaPropertyFunctorRegistry::RegisterGetter(const std::string &sPropertyName,
					const std::string &sClassType,
					IVistaPropertyGetFunctor *pFunctor,
					bool bForce)
{
	// first lookup key in attrib to class map
	STSTMAP::iterator cmp = m_mpGProps.find(sPropertyName);
	if(cmp == m_mpGProps.end())
	{
		// ok, attrib name is NOT registered
		// so register attrib name for the specific class
		std::set<std::string> sTmp;
		sTmp.insert(sClassType);

		// register set
		m_mpGProps.insert(STSTMAP::value_type(sPropertyName, sTmp));
	}
	else
	{
		// ok, we already registered this property
		// what might be missing is that it was for this class
		std::set<std::string> sTmp = (*cmp).second;

		// we add the class type to the current set
		sTmp.insert(sClassType);
		m_mpGProps.erase(cmp);
		m_mpGProps.insert(STSTMAP::value_type(sPropertyName, sTmp));
	}

	// ok, lookup set in getter-map
	GETFMAP::const_iterator cit = m_mpGetters.find(STRPAIR(sPropertyName, sClassType));
	if(cit == m_mpGetters.end())
	{
		// ok register functor for this class
		m_mpGetters.insert(GETFMAP::value_type(STRPAIR(sPropertyName, sClassType), pFunctor));
		m_mpGetterCache.clear(); // clear cache map
		return true;
	}
	else
	{
		// already registered, force insertion
		if(bForce)
		{
			m_mpGetters[STRPAIR(sPropertyName, sClassType)] = pFunctor;
			m_mpGetterCache.clear(); // clear cache map
			return true;
		}
		else
		{
			if(m_bTalkative)
			{
				// utter a warning
				vasperr << "VistaPropertyFunctorRegistry::RegisterGetter("
						  << sPropertyName << ", "
						  << sClassType << ", "
						  << pFunctor << ", "
						  << (bForce ? "TRUE" : "FALSE")
						  << ") -- functor for prop and class already registered\n";
			}
		}
	}

	return false;
}

bool VistaPropertyFunctorRegistry::UnregisterGetter(
					  const std::string &sPropName,
					  const std::string &sClassType)
{
	STSTMAP::iterator cmp = m_mpGProps.find(sPropName);
	if(cmp == m_mpGProps.end())
	{
		// ok, attrib name is NOT registered
		return true; // unregistered ;)
	}

	// find classtype in functor map
	GETFMAP::iterator fit = m_mpGetters.find(STRPAIR(sPropName, sClassType));
	if(fit == m_mpGetters.end())
	{
		// ok, no functor for this
		return true; // unregistered ;)
	}

	// ok, we have a functor for this, so remove it from map
	m_mpGetters.erase(fit);
	// do not forget to unregister from setmap
	std::set<std::string>::iterator sit = (*cmp).second.find(sClassType);
	if(sit != (*cmp).second.end())
	{

		if((*cmp).second.size() == 1)
		{
			// we can remove the set completely
			m_mpGProps.erase(cmp);
		}
		else
		{
			(*cmp).second.erase(sit);
		}

		if( m_bTalkative )
			vaspout << "VistaPropertyFunctorRegistry::UnregisterGetter("
					  << sPropName << ", "
					  << sClassType << ")"
					  << std::endl;

		m_mpGetterCache.clear(); // clear cache map
		return true;
	}

	return false;
}


IVistaPropertyGetFunctor *VistaPropertyFunctorRegistry::GetGetFunctor(
									const std::string &sPropName,
									const std::string &sClassType,
									const std::list<std::string> &rLiBaseCl)
{
	GETFMAP::const_iterator cp = m_mpGetterCache.find(STRPAIR(sPropName, sClassType));
	if(cp != m_mpGetterCache.end())
		return (*cp).second;


	// check for prop-name
	STSTMAP::const_iterator pit = m_mpGProps.find(sPropName);
	if(pit == m_mpGProps.end())
	{
		if(m_bTalkative)
		{
			// we can abort the search here
			vasperr << "VistaPropertyFunctorRegistry::GetGetFunctor("
					  << sPropName << ", "
					  << sClassType
					  << ") -- no prop for this name defined\n";
		}
		return NULL;
	}

	// ok, search the set for the least known symbol
	std::string sTmp;
	for(std::list<std::string>::const_reverse_iterator lcit = rLiBaseCl.rbegin();
		lcit != rLiBaseCl.rend(); ++lcit)
		{
			if((*pit).second.find(*lcit) != (*pit).second.end())
			{
				// ok, we found a matching symbol
				sTmp = *lcit;
				break; // leave loop on first match
			}
		}
	if(!sTmp.empty())
	{
		// ok symbol found, try lookup
		GETFMAP::const_iterator cit = m_mpGetters.find(STRPAIR(sPropName,sTmp));
		if(cit == m_mpGetters.end())
		{
			if(m_bTalkative)
			{
				// weird error
				vasperr << "VistaPropertyFunctorRegistry::GetGetFunctor("
					  << sPropName << ", "
					  << sClassType
					  << ") -- no functor for class (should not happen!)\n";
			}
			return NULL;
		}

		m_mpGetterCache.insert(GETFMAP::value_type(STRPAIR(sPropName, sClassType), (*cit).second));

		return (*cit).second;
	}

/*	STRPAIR p(sPropName, sClassType);
	GETFMAP::const_iterator cit = m_mpGetters.find(p);
	if( cit != m_mpGetters.end())
	{
		// already registered
		return (*cit).second;
	}
	*/
	return NULL;
}


bool VistaPropertyFunctorRegistry::RegisterSetter(const std::string &sPropertyName,
					const std::string &sClassType,
					IVistaPropertySetFunctor *pFunctor,
					bool bForce)
{
		// first lookup key in attrib to class map
	STSTMAP::iterator cmp = m_mpSProps.find(sPropertyName);
	if(cmp == m_mpSProps.end())
	{
		// ok, attrib name is NOT registered
		// so register attrib name for the specific class
		std::set<std::string> sTmp;
		sTmp.insert(sClassType);

		// register set
		m_mpSProps.insert(STSTMAP::value_type(sPropertyName, sTmp));
	}
	else
	{
		// ok, we already registered this property
		// what might be missing is that it was for this class
		std::set<std::string> sTmp = (*cmp).second;

		// we add the class type to the current set
		sTmp.insert(sClassType);
		//m_mpSProps[sPropertyName] = sTmp;

		m_mpSProps.erase(cmp);
		m_mpSProps.insert(STSTMAP::value_type(sPropertyName, sTmp));
	}

	// ok, lookup set in getter-map
	SETFMAP::const_iterator cit = m_mpSetters.find(STRPAIR(sPropertyName,sClassType));
	if(cit == m_mpSetters.end())
	{
		// ok register functor for this class
		m_mpSetters.insert(SETFMAP::value_type(STRPAIR(sPropertyName, sClassType), pFunctor));
		m_mpSetterCache.clear(); // clear cache map
		return true;
	}
	else
	{
		// already registered, force insertion
		if(bForce)
		{
			m_mpSetters[STRPAIR(sPropertyName, sClassType)] = pFunctor;
			m_mpSetterCache.clear(); // clear cache map
			return true;
		}
		else
		{
			if(m_bTalkative)
			{
				// utter a warning
				vasperr << "VistaPropertyFunctorRegistry::RegisterSetter("
						  << sPropertyName << ", "
						  << sClassType << ", "
						  << pFunctor << ", "
						  << (bForce ? "TRUE" : "FALSE")
						  << ") -- functor for prop and class already registered\n";
			}
		}
	}

	return false;
}

bool VistaPropertyFunctorRegistry::UnregisterSetter(const std::string &sPropName,
					  const std::string &sClassType)
{
	STSTMAP::iterator cmp = m_mpSProps.find(sPropName);
	if(cmp == m_mpSProps.end())
	{
		// ok, attrib name is NOT registered
		return true; // unregistered ;)
	}

	// find classtype in functor map
	SETFMAP::iterator fit = m_mpSetters.find(STRPAIR(sPropName, sClassType));
	if(fit == m_mpSetters.end())
	{
		// ok, no functor for this
		return true; // unregistered ;)
	}

	// ok, we have a functor for this, so remove it from map
	m_mpSetters.erase(fit);
	// do not forget to unregister from setmap
	std::set<std::string>::iterator sit = (*cmp).second.find(sClassType);
	if(sit != (*cmp).second.end())
	{
		if((*cmp).second.size() == 1)
		{
			// we can remove the set completely
			m_mpSProps.erase(cmp);
		}
		else
		{
			(*cmp).second.erase(sit);
		}
		m_mpSetterCache.clear(); // clear cache map
		if( m_bTalkative )
		{
			vaspout << "VistaPropertyFunctorRegistry::UnregisterSetter("
		          << sPropName << ", "
		          << sClassType << ")"
		          << std::endl;
		}
		return true;
	}
	return false;
}

IVistaPropertySetFunctor *VistaPropertyFunctorRegistry::GetSetFunctor(const std::string &sPropName,
									const std::string &sClassType,
										const std::list<std::string> &rLiBaseCl)
{
	// check for chached value
	SETFMAP::const_iterator ct = m_mpSetterCache.find(STRPAIR(sPropName, sClassType));
	if(ct != m_mpSetterCache.end())
		return (*ct).second; // return cached value

	// we did not find it, so try to resolve the symbol pair

	// check for prop-name
	STSTMAP::const_iterator pit = m_mpSProps.find(sPropName);
	if(pit == m_mpSProps.end())
	{
		if(m_bTalkative)
		{
			// we can abort the search here
			vasperr << "VistaPropertyFunctorRegistry::GetSetFunctor("
					  << sPropName << ", "
					  << sClassType
					  << ") -- no prop for this name defined\n";
		}
		return NULL;
	}

	// ok, search the set for the least known symbol
	std::string sTmp;
	//for(std::list<std::string>::const_iterator lcit = rLiBaseCl.begin(); lcit != rLiBaseCl.end(); ++lcit)
	for(std::list<std::string>::const_reverse_iterator lcit = rLiBaseCl.rbegin();
		lcit != rLiBaseCl.rend(); ++lcit)
		{
			if((*pit).second.find(*lcit) != (*pit).second.end())
			{
				// ok, we found a matching symbol
				sTmp = *lcit;
				break; // leave loop on fist match
			}
		}
	if(!sTmp.empty())
	{
		// ok symbol found, try lookup
		SETFMAP::const_iterator cit = m_mpSetters.find(STRPAIR(sPropName,sTmp));
		if(cit == m_mpSetters.end())
		{
			if(m_bTalkative)
			{
				// weird error
				vasperr << "VistaPropertyFunctorRegistry::GetSetFunctor("
					  << sPropName << ", "
					  << sClassType
					  << ") -- no functor for class (should not happen!)\n";
			}
			return NULL;
		}

		// we found a setter, so add to cache
		m_mpSetterCache.insert(SETFMAP::value_type(STRPAIR(sPropName, sClassType), (*cit).second));

		return (*cit).second;
	}

	return NULL;
}

std::list<IVistaPropertyGetFunctor*> VistaPropertyFunctorRegistry::GetGetterByClass(const std::string &strExactMatch) const
{
	std::list<IVistaPropertyGetFunctor*> liRet;

	for( GETFMAP::const_iterator cit = m_mpGetters.begin(); cit != m_mpGetters.end(); ++cit )
	{
		if( (*cit).first.second == strExactMatch )
			liRet.push_back( (*cit).second );
	}

	return liRet;
}

std::list<IVistaPropertySetFunctor*> VistaPropertyFunctorRegistry::GetSetterByClass(const std::string &strExactMatch) const
{
	std::list<IVistaPropertySetFunctor*> liRet;

	for( SETFMAP::const_iterator cit = m_mpSetters.begin(); cit != m_mpSetters.end(); ++cit )
	{
		if( (*cit).first.second == strExactMatch )
			liRet.push_back( (*cit).second );
	}

	return liRet;

}

int VistaPropertyFunctorRegistry::GetGetterSymbolSet(
						std::set<std::string> &setStore,
						const std::list<std::string> &rLiBaseCl) const
{
	for(GETFMAP::const_iterator cit = m_mpGetters.begin();
	   cit != m_mpGetters.end(); ++cit)
	   {
			std::list<std::string>::const_iterator lit =
					   find(rLiBaseCl.begin(),rLiBaseCl.end(),
								 (*cit).first.second);
			if(lit != rLiBaseCl.end())
			{
				// ok, symbol found
				setStore.insert((*cit).first.first);
			}
	   }
	return (int)setStore.size();
}

int VistaPropertyFunctorRegistry::GetSetterSymbolSet(
					   std::set<std::string> &setStore,
					   const std::list<std::string> &rLiBaseCl) const
{
	for(SETFMAP::const_iterator cit = m_mpSetters.begin();
	   cit != m_mpSetters.end(); ++cit)
	   {
			std::list<std::string>::const_iterator lit =
					   find(rLiBaseCl.begin(),rLiBaseCl.end(),
								 (*cit).first.second);
			if(lit != rLiBaseCl.end())
			{
				// ok, symbol found
				setStore.insert((*cit).first.first);
			}
	   }
	return (int)setStore.size();
}

int VistaPropertyFunctorRegistry::GetSymbolSet(
					   std::set<std::string> &setStore,
					   const std::list<std::string> &rLiBaseCl) const
{
	GetGetterSymbolSet(setStore, rLiBaseCl);
	GetSetterSymbolSet(setStore, rLiBaseCl);
	return (int)setStore.size();
}

bool VistaPropertyFunctorRegistry::GetTalkativeFlag() const
{
	return m_bTalkative;
}

void VistaPropertyFunctorRegistry::SetTalkativeFlag(bool bTalkative)
{
	m_bTalkative = bTalkative;
}

typedef std::set<std::string> STRINGSET;
typedef std::multimap<std::string, STRINGSET> STRINGMAP;

template<class T>
class _printElement //: public std::unary_function<std::map<std::pair<std::string, std::string>, typename T*>::value_type, void>
{
public:
	_printElement( VistaAspectsOut &out )
	: m_out(out) {}

	VistaAspectsOut &m_out;

	typedef std::pair<std::string, std::string> STRINGPAIR;
	typedef std::map<STRINGPAIR, T*> MAPTYPE;
	void operator()( const typename MAPTYPE::value_type &value )
	{
		m_out.out() << "[" << value.first.first << " ; " << value.first.second << "] -- "
				    << (value.second ? value.second->GetNameForNameable() : "<NONE>?")
				    << std::endl;
	}


	void operator()( const typename STRINGMAP::value_type &value )
	{
		m_out.out() << "[" << value.first <<  "] -- " << std::endl;

		const STRINGSET &v = value.second;
		std::for_each( v.begin(), v.end(), _printElement<std::string>( m_out ) );
	}

	void operator() ( const std::string &element )
	{
		m_out.out() << "\t" << element << std::endl;
	}

};

void VistaPropertyFunctorRegistry::Show(VistaAspectsOut &out) const
{
	out.out() << "[GETTERS]" << std::endl;
	std::for_each( m_mpGetters.begin(), m_mpGetters.end(), _printElement<IVistaPropertyGetFunctor>(out) );

	out.out() << "[SETTERS]" << std::endl;
	std::for_each( m_mpSetters.begin(), m_mpSetters.end(), _printElement<IVistaPropertySetFunctor>(out) );

	out.out() << "[GetPMaps]" << std::endl;
	std::for_each( m_mpGProps.begin(), m_mpGProps.end(), _printElement<STRINGMAP>( out ) );

	out.out() << "[GetSMaps]" << std::endl;
	std::for_each( m_mpSProps.begin(), m_mpSProps.end(), _printElement<STRINGMAP>( out ) );
}

/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

