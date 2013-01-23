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

#include "glworldwidget.hpp"
#include "zone.hpp"

#include <model.hpp>
#include <model_mesh_collision.hpp>
#include <model_mesh.hpp>
#include <model_manager.hpp>
#include <navigation_mesh.hpp>
#include <texture_manager.hpp>

#include <boost/bind.hpp>
#include <QtGui>
#include <QtOpenGL>
#include <QDebug>
#include <QFile>
#include <iostream>

GLWorldWidget::GLWorldWidget (const boost::shared_ptr<ModelManager> &modelMgr,
    const boost::shared_ptr<TextureManager> &texMgr, QWidget *parent)
    :   QGLWidget(QGLFormat(QGL::SampleBuffers),parent),
        m_RenderNavMesh(false),
        m_RenderAABB(false),
        m_RenderCollisionMesh(false),
        m_RenderMotion(false),
        m_ModelMgr(modelMgr),
        m_TextureMgr(texMgr)
{
    srand(time(NULL));
}

QSize GLWorldWidget::minimumSizeHint() const
{
    return QSize(200,200);
}

QSize GLWorldWidget::sizeHint() const
{
    return QSize(400,400);
}

void GLWorldWidget::setCurrentZone (const boost::shared_ptr<Zone> &zone)
{
    m_CurZone = zone;

    updateGL();
}

boost::shared_ptr<Zone> GLWorldWidget::getCurrentZone() const
{
    return m_CurZone;
}

void GLWorldWidget::setRenderNavMesh (bool render)
{
    if (m_CurZone)
    {
        m_RenderNavMesh = render;

        updateGL();
    }
}

void GLWorldWidget::setRenderAABBs (bool render)
{
    if (m_CurZone)
    {
        m_RenderAABB = render;

        updateGL();
    }
}

void GLWorldWidget::setRenderCollisionMesh (bool render)
{
    if (m_CurZone)
    {
        m_RenderCollisionMesh = render;

        updateGL();
    }
}

void GLWorldWidget::resolveMotion()
{
    if (m_CurZone)
    {
        m_RenderMotion = true;

//		Coord beg(984.898193,1916.132812,-0.433914,m_CurZone->ID());
//		Coord end(984.711304,1911.136353,-0.433914,m_CurZone->ID());

		Coord beg(565.034790,1058.567505,575.964722,m_CurZone->ID());
		Coord end(560.036621,1058.702637,575.964722,m_CurZone->ID());

//        beg.set_z(m_CurZone->GetHeightAt(beg.get_x(),beg.get_y()));
//        end.set_z(m_CurZone->GetHeightAt(end.get_x(),end.get_y()));

        m_MotionPath = Line(beg.get_x(),beg.get_y(),beg.get_z(),end.get_x(),end.get_y(),end.get_z());

        m_CurZone->ResolveMotion(beg,end,m_ModelMgr,&m_EndPos);

        qDebug() << "EndPos = " << m_EndPos.get_x() << "," << m_EndPos.get_y() << "," << m_EndPos.get_z() << " - " << m_EndPos.get_zone();

        updateGL();
    }
}

void GLWorldWidget::initializeGL()
{
    glClearDepth(1.0);
    glClearColor(0, 0, 0, 0);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.f, 1.f, 1.f, 30000.f);
}

void GLWorldWidget::paintGL()
{
    static float HEIGHT_RATIO = 1.5f;
    static float scaleValue = 0.75f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float height = 0;

    if (m_CurZone)
        height = m_CurZone->GetHeightAt(1712,1714);

    // This Determines The Camera's Position And View
    //          Position        View       Up Vector
    gluLookAt(2000, 900+height, 2000, 20, 100 + height, 20, 0, 1, 0);
    glScalef(scaleValue, scaleValue * HEIGHT_RATIO, scaleValue);

    if (m_CurZone)
        RenderZone(m_CurZone);
}

void GLWorldWidget::RenderZone (const boost::shared_ptr<Zone> &zone)
{
    enum { CELL_SIZE = NavigationMesh::TEXTURE_SIZE };

    boost::shared_ptr<NavigationMesh> mesh = zone->getNavigationMesh();

    if (!mesh)
        return;

    glPushMatrix();

//    glEnable(GL_TEXTURE_2D);
//    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
//
//    for (int x = 0; x < CELL_SIZE; ++x)
//    {
//        for (int z = 0; z < CELL_SIZE; ++z)
//        {
//            glBindTexture(GL_TEXTURE_2D,m_TextureMgr->Find(mesh->getTextureAt(x,z)));
//            glBegin(GL_QUADS);
//                glColor3f(1,1,1);
//                glTexCoord2f(0.0f, 0.0f);
//                glVertex3f((x + 0) * 20.0f, mesh->getHeightAt(x - 0, z + 0), (z + 0) * 20.0f);
//                glTexCoord2f(1.0f, 0.0f);
//                glVertex3f((x + 1) * 20.0f, mesh->getHeightAt(x + 1, z + 0), (z + 0) * 20.0f);
//                glTexCoord2f(1.0f, 1.0f);
//                glVertex3f((x + 1) * 20.0f, mesh->getHeightAt(x + 1, z + 1), (z + 1) * 20.0f);
//                glTexCoord2f(0.0f, 1.0f);
//                glVertex3f((x + 0) * 20.0f, mesh->getHeightAt(x - 0, z + 1), (z + 1) * 20.0f);
//            glEnd();
//        }
//    }
//
//    glDisable(GL_TEXTURE_2D);

    if (m_RenderNavMesh)
    {
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        mesh->visitCells(boost::bind(&GLWorldWidget::RenderNavigationCell,this,_1));

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    if (m_RenderMotion)
    {
        glColor3f(0.5f,0.5f,0.5f);

        glBegin(GL_LINES);
            glVertex3f(m_MotionPath.x1,m_MotionPath.z1+2,m_MotionPath.y1);
            glVertex3f(m_MotionPath.x2,m_MotionPath.z2+2,m_MotionPath.y2);
        glEnd();

        glPointSize(2.0f);

        glBegin(GL_POINTS);
            glVertex3f(m_MotionPath.x1,m_MotionPath.z1+2,m_MotionPath.y1);
            glVertex3f(m_MotionPath.x2,m_MotionPath.z2+2,m_MotionPath.y2);
        glEnd();

        glPointSize(4.0f);

        glBegin(GL_POINTS);
            glColor3f(1.0f,1.0f,1.0f);
            glVertex3f(m_EndPos.get_x(),m_EndPos.get_z()+2,m_EndPos.get_y());
        glEnd();

        glPointSize(1.0f);
    }

    mesh->visitEntities(boost::bind(&GLWorldWidget::RenderNavigationEntity,this,_1));

    glPopMatrix();
}

void GLWorldWidget::RenderNavigationEntity (const NavigationEntity &entity) const
{
    boost::shared_ptr<Model> model = m_ModelMgr->FindModel(entity.refID);

    if (model)
    {
        glPushMatrix();
        glTranslatef(entity.x,entity.y,entity.z);
        glRotatef(entity.angle*(180/3.14159),0,1,0);

        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

        uint8_t color[3] = { rand()%255,rand()%255,rand()%255 };

        if (m_RenderAABB || m_RenderCollisionMesh)
            glColor3ub(color[0],color[1],color[2]);

        if (m_RenderAABB)
        {
            /// DRAW BOUNDING BOX
            glBegin(GL_QUADS);

                glVertex3f(model->minX,model->minY,model->minZ);
                glVertex3f(model->maxX,model->minY,model->minZ);
                glVertex3f(model->maxX,model->minY,model->maxZ);
                glVertex3f(model->minX,model->minY,model->maxZ);

                glVertex3f(model->minX,model->maxY,model->minZ);
                glVertex3f(model->maxX,model->maxY,model->minZ);
                glVertex3f(model->maxX,model->maxY,model->maxZ);
                glVertex3f(model->minX,model->maxY,model->maxZ);

                glVertex3f(model->minX,model->minY,model->minZ);
                glVertex3f(model->minX,model->minY,model->maxZ);
                glVertex3f(model->minX,model->maxY,model->maxZ);
                glVertex3f(model->minX,model->maxY,model->minZ);

                glVertex3f(model->maxX,model->minY,model->minZ);
                glVertex3f(model->maxX,model->minY,model->maxZ);
                glVertex3f(model->maxX,model->maxY,model->maxZ);
                glVertex3f(model->maxX,model->maxY,model->minZ);

                glVertex3f(model->minX,model->minY,model->minZ);
                glVertex3f(model->maxX,model->minY,model->minZ);
                glVertex3f(model->maxX,model->maxY,model->minZ);
                glVertex3f(model->minX,model->maxY,model->minZ);

                glVertex3f(model->minX,model->minY,model->maxZ);
                glVertex3f(model->maxX,model->minY,model->maxZ);
                glVertex3f(model->maxX,model->maxY,model->maxZ);
                glVertex3f(model->minX,model->maxY,model->maxZ);

            glEnd();
        }

        if (m_RenderCollisionMesh && model->hasCollisionMesh())
        {
            const CollisionMesh *mesh = model->getCollisionMesh()->getCollisionMesh();

            if (mesh)
            {
                std::vector<CollisionVertex> vertices = mesh->getVertices();
                std::vector<CollisionCell> cells = mesh->getCells();

                std::vector<CollisionVertex>::const_iterator vit;

                for (size_t i = 0; i < cells.size(); ++i)
                {
                    glColor3ub(color[0],color[1],color[2]);

                    /// DRAW EDGES
                    std::vector<CollisionLink>::const_iterator lit;

                    glBegin(GL_LINES);
                        for (lit = cells[i].Links.begin(); lit != cells[i].Links.end(); ++lit)
                        {
                            switch(lit->flag)
                            {
                            case CollisionLink::LINK_OUTER_BORDER_WALL:
                                glColor3ub(92,51,23);
                                break;
                            case CollisionLink::LINK_OUTER_BORDER_PASSABLE:
                                glColor3ub(255,0,0);
                                break;
                            case CollisionLink::LINK_INNER_BORDER:
                                glColor3ub(0,255,0);
                                break;
                            case CollisionLink::LINK_INNER_WALL:
                                glColor3ub(255,255,255);
                                break;
                            case CollisionLink::LINK_AERIAL_OUTER_BORDER_WALL:
                                glColor3ub(0,255,255);
                                break;
                            case CollisionLink::LINK_AERIAL_INNER_BORDER_PASSABLE:
                                glColor3ub(255,255,0);
                                break;
                            case 8:
//                                qDebug() << "Link with flag 8";
                                glColor3ub(76,32,120);
                                break;
                            default:
                                qDebug() << "Unknown Type = " << lit->flag;
                                glColor3ub(color[0],color[1],color[2]);
                                break;
                            }

                            vit = vertices.begin()+lit->vertexSrc;

                            glVertex3f(vit->x,vit->z,vit->y);

                            vit = vertices.begin()+lit->vertexDest;

                            glVertex3f(vit->x,vit->z,vit->y);
                        }
                    glEnd();
                }
            }
        }

        glPopMatrix();
    }
}

void GLWorldWidget::RenderNavigationCell (const NavigationCell &cell) const
{
    float h1 = m_CurZone->GetHeightAt(cell.rect.minX,cell.rect.minY);
    float h2 = m_CurZone->GetHeightAt(cell.rect.maxX,cell.rect.minY);
    float h3 = m_CurZone->GetHeightAt(cell.rect.maxX,cell.rect.maxY);
    float h4 = m_CurZone->GetHeightAt(cell.rect.minX,cell.rect.maxY);

    uint8_t color[3] = { 0,0,200 };

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColor4ub(color[0],color[1],color[2], 128);

    /// DRAW CELL AREA

    glBegin(GL_QUADS);
        glVertex3f(cell.rect.minX, h1, cell.rect.minY);
        glVertex3f(cell.rect.maxX, h2, cell.rect.minY);
        glVertex3f(cell.rect.maxX, h3, cell.rect.maxY);
        glVertex3f(cell.rect.minX, h4, cell.rect.maxY);
    glEnd();


    float h5 = 0;

    glBegin(GL_LINES);
        for (size_t i = 0; i < cell.links.size(); ++i)
        {
            switch(cell.links[i].type)
            {
            case NavigationLink::LINK_WALL:
                glColor4ub(255,0,255,255);
                break;
            case NavigationLink::LINK_ZONE:
                glColor4ub(255,255,0,255);
                break;
            default:
                glColor4ub(color[0],color[1],color[2], 200);
                break;
            }

            h5 = m_CurZone->GetHeightAt(cell.links[i].edge.x1,cell.links[i].edge.y1);

            glVertex3f(cell.links[i].edge.x1,h5,cell.links[i].edge.y1);

            h5 = m_CurZone->GetHeightAt(cell.links[i].edge.x2,cell.links[i].edge.y2);

            glVertex3f(cell.links[i].edge.x2,h5,cell.links[i].edge.y2);
        }
    glEnd();

    /// DRAW NPC NAVIGATION POINTS (VERTICES AND MIDPOINTS)

    glColor4ub(255,0,0,255);
    glPointSize(2);

    glBegin(GL_POINTS);
        glVertex3f(cell.rect.minX, h1+2, cell.rect.minY);
        glVertex3f(cell.rect.maxX, h2+2, cell.rect.minY);
        glVertex3f(cell.rect.maxX, h3+2, cell.rect.maxY);
        glVertex3f(cell.rect.minX, h4+2, cell.rect.maxY);

        glVertex3f((cell.rect.minX+cell.rect.maxX)/2,(h1+h2)/2,cell.rect.minY);
        glVertex3f((cell.rect.minX+cell.rect.maxX)/2,(h3+h4)/2,cell.rect.maxY);
        glVertex3f(cell.rect.minX,(h1+h4)/2,(cell.rect.minY+cell.rect.maxY)/2);
        glVertex3f(cell.rect.maxX,(h3+h2)/2,(cell.rect.minY+cell.rect.maxY)/2);
    glEnd();

    glPointSize(1);
}

void GLWorldWidget::resizeGL(int width, int height)
{
    // Prevent divide by zero (in the gluPerspective call)
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height); // Reset current viewport

    glMatrixMode(GL_PROJECTION); // Select projection matrix
    glLoadIdentity(); // Reset projection matrix

    gluPerspective(45.0f, static_cast<GLfloat>(width)/height, 0.1f, 20000.0f); // Calculate aspect ratio

    glMatrixMode(GL_MODELVIEW); // Select modelview matrix
    glLoadIdentity(); // Reset modelview matrix
}

void GLWorldWidget::wheelEvent(QWheelEvent *event)
{
//    if (event->delta() > 0)
//        m_Z += 50;
//    else
//        m_Z -= 50;
//
//    updateGL();
//    event->accept();
}

void GLWorldWidget::keyPressEvent(QKeyEvent *event)
{
//    bool updateScene = true;
//
//    switch(event->key())
//    {
//    case Qt::Key_Up:
//        m_RotX += 5;
//        break;
//    case Qt::Key_Down:
//        m_RotX -= 5;
//        break;
//    case Qt::Key_Left:
//        m_RotY -= 5;
//        break;
//    case Qt::Key_Right:
//        m_RotY += 5;
//        break;
//    default:
//        updateScene = false;
//        break;
//    }
//
//    if (updateScene)
//        updateGL();
}
