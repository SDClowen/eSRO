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

#ifndef HOST_INFO_HPP
#define HOST_INFO_HPP

#include <stdint.h>
#include <vector>
#include <string>

struct HostDivisionInfo
{
    uint8_t locale;
    uint32_t version;
    std::string port;
    std::string div;
    std::vector<std::string> hosts;
};

struct DivisionInfo
{
    std::string name;               ///DIVISION NAME
    std::vector<std::string> gws;   ///GATEWAY LIST
};

struct HostInfo
{
    uint8_t locale;
    uint32_t version;
    std::string port;
    std::vector<DivisionInfo> divs;  ///DIVISION LIST
};

#endif // HOST_INFO_HPP
