/*********************************************************************************
 *
 * This file is part of eSRO.
 *
 * eSRO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * eSRO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright ⓒ 2013  Rafael Dominguez (npcdoom)
 *
 *********************************************************************************/

#ifndef __SLES_MASTERY_TREE_HPP__
#define __SLES_MASTERY_TREE_HPP__

#include "mastery.hpp"
#include <map>

namespace Exception
{

class InvalidMastery
{};

class MasteryLimit
{};

}

/**
 *
 *  CHARACTER MASTERY TREE OBJECT
 *
 *  CHINESE RACE MASTERY LIMIT IS 330
 *  EUROPEAN RACE MASTERY LIMIT IS CHARACTER_LEVEL * 2
 *
 **/

class MasteryTree
{
public:

    typedef std::map<uint32_t,Mastery>::value_type value_type;
	typedef std::map<uint32_t,Mastery>::iterator iterator;
	typedef std::map<uint32_t,Mastery>::const_iterator const_iterator;

	MasteryTree (const RACE race = RACE_NONE, uint8_t CharLevel = 0);

	void SetCharacterLevel (uint8_t lvl);

	uint8_t GetCharacterLevel () const;

	int GetMasteryLimit () const;

	int GetMasteryTotal () const;

	const std::map<uint32_t,Mastery>& GetMasteryList () const;

	bool IsMax () const;

	iterator find (const uint32_t ID);

	const_iterator find (const uint32_t ID) const;

	iterator begin ();
	const_iterator begin () const;

	iterator end ();
	const_iterator end () const;

	void insert (const Mastery &mast);

	void erase (const uint32_t ID);

	void erase (iterator iter);

	/**
	 *
	 *  RETURN THE 2 HIGHEST MASTERY IN THE TREE
	 *
	 **/

	std::pair<Mastery,Mastery> GetHighestMastery () const;

	static MasteryTree Create (const RACE race);

private:

	RACE m_Race;
	uint8_t m_CharLevel;
	std::map<uint32_t,Mastery> m_mastery_list;
};

#endif //__SLES_MASTERY_TREE_HPP__
