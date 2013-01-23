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

#ifndef __SRDL_CHARACTER_QUERIES_HPP__
#define __SRDL_CHARACTER_QUERIES_HPP__

#include <autopot.hpp>
#include <coord.hpp>
#include <player_property.hpp>

#include <mysql++/mysql++.h>

#include <map>
#include <boost/shared_ptr.hpp>

class Player;

namespace DB {
namespace PLAYER {

class Check
{
public:

    bool operator() (mysqlpp::Connection &conn, const std::string& name);
};

class Create
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t AccountID, const boost::shared_ptr<Player> &player,
        const std::string &name);
};

class Select
{
public:

    std::vector<PlayerProperty> operator() (mysqlpp::Connection& conn, const uint32_t AccountID);
};

class Delete
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t ID);
};

/**
 *
 *  brief Update player actual position in world.
 *
 **/

class Position
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t CharID, const Coord &position);
};

class Angle
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint16_t angle);
};

class SP
{
public:

    void operator() (mysqlpp::Connection& conn, const uint32_t CharID, const uint32_t points);
};

class Exp
{
public:

    void operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player);
};

class Stats
{
public:

    void operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player);
};

class Lvl
{
public:

    void operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player);
};

class Gold
{
public:

    void operator() (mysqlpp::Connection& conn, const boost::shared_ptr<Player> player);
};

class GoldStored
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint64_t gold);
};

class ReturnPoint
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t teleport_id);
};

class HPMP
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t hp, const uint32_t mp);
};

class Bersek
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t count);
};

class Erase
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID);
};

class Restore
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID);
};

class State
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, bool state);
};

class GuildID
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t GuildID);
};

class StallAvatar
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t avatarID);
};

class Autopot
{
public:

    AutopotSetting operator() (mysqlpp::Connection &conn, const uint32_t CharID);

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const AutopotSetting &settings);
};

class InventoryCapacity
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t size);
};

class StorageCapacity
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint8_t size);
};

class Skin
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t CharID, const uint32_t refCharID,
        const uint8_t volume);
};

class Buffs
{
public:

    void operator() (mysqlpp::Connection &conn, const uint32_t charID, const uint32_t refSkillID, const uint32_t time);

    std::vector<std::pair<uint32_t,uint32_t> > operator() (mysqlpp::Connection &conn, const uint32_t charID);
};

class RemoveBuffs
{
public:

    /**
     *
     *  @brief Clean all persistent buff of the desired player.
     *
     *  @conn: active db connection.
     *  @charID: character ID.
     *
     **/

    void operator() (mysqlpp::Connection &conn, const uint32_t charID);
};

}   }

#endif //__SRDL_CHARACTER_QUERIES_HPP__
