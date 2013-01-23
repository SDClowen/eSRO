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

#ifndef __SR_GAME_SERVER_FORTRESS_WAR_MANAGER_HPP__
#define __SR_GAME_SERVER_FORTRESS_WAR_MANAGER_HPP__

#include <fortress_property.hpp>
#include <map>

enum FORTRESS_TYPE
{
    FORTRESS_NONE,
    FORTRESS_JANGAN,
    FORTRESS_DOWNHANG,
    FORTRESS_HOTAN,
    FORTRESS_CONSTANTINOPLE,
    FORTRESS_SAMARKAND,
    FORTRESS_BIJEOKDAN
};

class FortressManager
{
public:

    FortressManager ();

    bool Initialize ();

    void Release ();

    void addFortress (const FortressProperty &property);

    /**
     *
     *  Check if the fortress is implemented.
     *  @name Name of the fortress to check.
     *
     **/

    bool isFortressActive (const std::string &name) const;

private:

    std::map<std::string,uint32_t> m_NameMapping;
    std::map<uint32_t,FortressProperty> m_Fortresses;
};

#endif
