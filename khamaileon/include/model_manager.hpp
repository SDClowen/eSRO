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

#ifndef MODEL_MANAGER_HPP
#define MODEL_MANAGER_HPP

#include <map>
#include <string>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

class Model;

struct ObjectInfo
{
    uint32_t refID;
    std::string refModel;
};

class ModelManager
{
public:

    typedef boost::shared_ptr<Model> model_ptr;

    ModelManager (const std::string &path);

    bool Initialize ();

    void LoadModels ();

    bool LoadModel (const uint32_t refModelID);

    ObjectInfo FindModelInfo (const uint32_t refModelID) const;

    boost::shared_ptr<Model> FindModel (const uint32_t refModelID) const;

    size_t size () const;

    const std::map<uint32_t,model_ptr>& GetModels () const;

protected:

    bool LoadModel (const uint32_t refModelID,const std::string &entry);

private:

    std::string m_Path;
    std::map<uint32_t,ObjectInfo> m_Objects;
    std::map<uint32_t,model_ptr> m_Models;
};

#endif // MODEL_MANAGER_HPP
