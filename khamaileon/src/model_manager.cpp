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

#include "model_manager.hpp"
#include "model.hpp"
#include "strutil.hpp"
#include <algorithm>
#include <fstream>
#include <vector>
#include <stdlib.h>

ModelManager::ModelManager (const std::string &path)
    : m_Path(path)
{
    if (*m_Path.rbegin() != '/')
        m_Path.append("/");
}

bool ModelManager::Initialize()
{
    std::string objname = m_Path + "navmesh/object.ifo";

    std::ifstream objfile(objname.c_str());

    if (!objfile)
        return 0;

    std::string line;

    getline(objfile,line);

    if (line != "JMXVOBJI1000")
        return false;

    if (objfile.fail())
        return false;

    getline(objfile,line);

    int count = atoi(line.c_str());

    if (objfile.fail())
        return false;

    ObjectInfo obj;
    std::vector<std::string> fields;

    while (!objfile.eof() && count)
    {
        getline(objfile,line);

        fields = split(line,' ');

        if (fields.size() >= 3)
        {
            obj.refID = atoi(fields[0].c_str());
            ///fields[1] is some flag stuff still unknown
            
            obj.refModel = fields[2];

            for (size_t i = 3; i < fields.size(); ++i)
                obj.refModel += " " + fields[i];

            for (size_t i = 0; i < obj.refModel.size(); ++i)
            {
                if (obj.refModel[i] == '\\')
                    obj.refModel[i] = '/';
            }

            obj.refModel.erase(std::remove(obj.refModel.begin(),obj.refModel.end(),'"'),obj.refModel.end());

            m_Objects.insert(std::make_pair(obj.refID,obj));
        }

        fields.clear();
        --count;
    }

    if (count)
        return false;

    return true;
}

void ModelManager::LoadModels()
{
    for (std::map<uint32_t,ObjectInfo>::const_iterator it = m_Objects.begin(); it != m_Objects.end(); ++it)
        LoadModel(it->second.refID,it->second.refModel);
}

bool ModelManager::LoadModel (uint32_t refModelID)
{
    std::map<uint32_t,ObjectInfo>::const_iterator it = m_Objects.find(refModelID);

    if (it == m_Objects.end())
        return false;

    return LoadModel(refModelID,it->second.refModel);
}

bool ModelManager::LoadModel (const uint32_t refModelID, const std::string &entry)
{
    boost::shared_ptr<Model> model(new Model);

    if (!model)
        return false;

    if (!model->Read(m_Path+entry))
        return false;

    /// READ THE COLLISION MESH DATA
    if (model->hasCollisionMesh())
    {
        if (!model->ReadMeshs(m_Path))
            return false;
    }

    m_Models.insert(std::make_pair(refModelID,model));

    return true;
}

ObjectInfo ModelManager::FindModelInfo (const uint32_t refModelID) const
{
    ObjectInfo info;
    std::map<uint32_t,ObjectInfo>::const_iterator it = m_Objects.find(refModelID);

    if (it != m_Objects.end())
        info = it->second;

    return info;
}

boost::shared_ptr<Model> ModelManager::FindModel (const uint32_t refModelID) const
{
    boost::shared_ptr<Model> model;

    std::map<uint32_t,model_ptr>::const_iterator it = m_Models.find(refModelID);

    if (it != m_Models.end())
        model = it->second;

    return model;
}

size_t ModelManager::size() const
{
    return m_Models.size();
}

const std::map<uint32_t,ModelManager::model_ptr>& ModelManager::GetModels () const
{
    return m_Models;
}
