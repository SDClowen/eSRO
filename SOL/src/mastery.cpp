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

#include "mastery.hpp"

Mastery::Mastery(uint32_t id ,uint8_t lv)
    : m_ID(id),m_Level(lv)
{
}

uint32_t Mastery::ID () const
{
    return m_ID;
}

void Mastery::Level (const uint8_t lvl)
{
    m_Level = lvl;
}

uint8_t Mastery::Level () const
{
    return m_Level;
}

bool Mastery::operator < (const Mastery& mastery) const
{
    return m_ID < mastery.m_ID;
}

Mastery& Mastery::operator++ ()
{
    if (m_Level < 255)
        ++m_Level;

    return *this;
}

Mastery Mastery::operator++ (int)
{
    Mastery tmp(m_ID,m_Level);

    ++(*this);

    return tmp;
}

Mastery& Mastery::operator-- ()
{
    if (m_Level > 0)
        --m_Level;

    return *this;
}

Mastery Mastery::operator-- (int)
{
    Mastery tmp(m_ID,m_Level);

    --(*this);

    return tmp;
}
