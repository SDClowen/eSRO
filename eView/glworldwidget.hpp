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

#ifndef GLWORLDWIDGET_HPP
#define GLWORLDWIDGET_HPP

#include <QGLWidget>
#include <boost/shared_ptr.hpp>

#include <navigation_mesh.hpp>

class Zone;
class MapManager;
class ModelManager;
class TextureManager;

class GLWorldWidget : public QGLWidget
{
    Q_OBJECT

public:

    GLWorldWidget (const boost::shared_ptr<ModelManager> &modelMgr, const boost::shared_ptr<TextureManager> &texMgr,
        QWidget *parent = NULL);

    QSize minimumSizeHint () const;

    QSize sizeHint () const;

    void setCurrentZone (const boost::shared_ptr<Zone> &zone);

    boost::shared_ptr<Zone> getCurrentZone () const;

    void setRenderNavMesh (bool render);

    void setRenderAABBs (bool render);

    void setRenderCollisionMesh (bool render);

    void resolveMotion ();

public slots:

signals:

protected:

    void initializeGL ();

    void paintGL ();

    void resizeGL (int width, int height);

    void wheelEvent (QWheelEvent *event);

    void keyPressEvent (QKeyEvent *event);

protected:

    void RenderZone (const boost::shared_ptr<Zone> &zone);

    void RenderNavigationEntity (const NavigationEntity &entity) const;

    void RenderNavigationCell (const NavigationCell &cell) const;

private:

    bool m_RenderNavMesh;
    bool m_RenderAABB;
    bool m_RenderCollisionMesh;
    bool m_RenderMotion;

    boost::shared_ptr<ModelManager> m_ModelMgr;
    boost::shared_ptr<TextureManager> m_TextureMgr;

    boost::shared_ptr<Zone> m_CurZone;
    Line m_MotionPath;
    Coord m_EndPos;
};

#endif // GLWORLDWIDGET_HPP
