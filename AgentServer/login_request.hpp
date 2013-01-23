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

#ifndef __GAME_SERVER_LOGIN_REQUEST_HPP__
#define __GAME_SERVER_LOGIN_REQUEST_HPP__

#include <stdint.h>
#include <string>

struct LoginRequest
{
    uint32_t ID;
    uint32_t AccountID;
    std::string usr;
    std::string pw;
    std::string IP;
    uint8_t access_lv;
    uint8_t sec_lv;
    uint32_t silk;

    inline bool operator == (const LoginRequest &req) const
    {
        return ID == req.ID && usr == req.usr && pw == req.pw;
    }
};

#endif
