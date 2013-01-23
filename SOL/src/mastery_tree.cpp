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

#include "mastery_tree.hpp"

MasteryTree::MasteryTree (const RACE race, uint8_t CharLevel)
	: m_Race(race), m_CharLevel(CharLevel)
{
}

void MasteryTree::SetCharacterLevel (uint8_t lvl)
{
	m_CharLevel = lvl;
}

uint8_t MasteryTree::GetCharacterLevel () const
{
    return m_CharLevel;
}

int MasteryTree::GetMasteryLimit () const
{
	int limit = 0;

    switch(m_Race)
    {
    case RACE_EU:
		limit = 2*m_CharLevel;
        break;
    case RACE_CH:
		limit = 330;
		break;
    default:
        break;
	}

	return limit;
}

int MasteryTree::GetMasteryTotal () const
{
    int total = 0;

    for (const_iterator i = begin(); i != end(); ++i)
        total += i->second.Level();

    return total;
}

const std::map<uint32_t,Mastery>& MasteryTree::GetMasteryList () const
{
	return m_mastery_list;
}

bool MasteryTree::IsMax () const
{
	return GetMasteryTotal() == GetMasteryLimit();
}

MasteryTree::iterator MasteryTree::find (const uint32_t ID)
{
    return m_mastery_list.find(ID);
}

MasteryTree::const_iterator MasteryTree::find (const uint32_t ID) const
{
    return m_mastery_list.find(ID);
}

MasteryTree::iterator MasteryTree::begin ()
{
	return m_mastery_list.begin();
}

MasteryTree::const_iterator MasteryTree::begin () const
{
	return m_mastery_list.begin();
}

MasteryTree::iterator MasteryTree::end ()
{
	return m_mastery_list.end();
}

MasteryTree::const_iterator MasteryTree::end () const
{
	return m_mastery_list.end();
}

void MasteryTree::insert (const Mastery &mastery)
{
    m_mastery_list.insert(std::make_pair(mastery.ID(),mastery));
}

void MasteryTree::erase (const uint32_t ID)
{
    m_mastery_list.erase(ID);
}

void MasteryTree::erase (iterator i)
{
    m_mastery_list.erase(i);
}

std::pair<Mastery,Mastery> MasteryTree::GetHighestMastery () const
{
    std::pair<Mastery,Mastery> t;

    for (const_iterator i = begin(); i != end(); ++i)
    {
        if (i->second.Level() > t.first.Level())
        {
            if (t.first.Level() > t.second.Level())
                t.second = t.first;

            t.first = i->second;
        }
        else if (i->second.Level() > t.second.Level())
            t.second = i->second;
    }

    return t;
};

MasteryTree MasteryTree::Create (const RACE race)
{
    MasteryTree tr(race);

    if (race == RACE_EU)
	{
		tr.m_mastery_list.insert(std::make_pair(513,Mastery(513)));
		tr.m_mastery_list.insert(std::make_pair(514,Mastery(514)));
		tr.m_mastery_list.insert(std::make_pair(515,Mastery(515)));
		tr.m_mastery_list.insert(std::make_pair(516,Mastery(516)));
		tr.m_mastery_list.insert(std::make_pair(517,Mastery(517)));
		tr.m_mastery_list.insert(std::make_pair(518,Mastery(518)));

	}
	else if ( race == RACE_CH )
	{
	    tr.m_mastery_list.insert(std::make_pair(257,Mastery(257)));
		tr.m_mastery_list.insert(std::make_pair(258,Mastery(258)));
		tr.m_mastery_list.insert(std::make_pair(259,Mastery(259)));
        tr.m_mastery_list.insert(std::make_pair(273,Mastery(273)));
        tr.m_mastery_list.insert(std::make_pair(274,Mastery(274)));
        tr.m_mastery_list.insert(std::make_pair(275,Mastery(275)));
        tr.m_mastery_list.insert(std::make_pair(276,Mastery(276)));
	}

	return tr;
}
