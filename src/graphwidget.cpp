/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "nodefactory.h"
#include "mainwindow.h"
#include <math.h>
#include <QGraphicsSceneEvent>
#include <QKeyEvent>
class MyQGraphicsScene : public QGraphicsScene
{
    GraphWidget *widget;
public:
    MyQGraphicsScene(GraphWidget *w,QWidget *parent) :QGraphicsScene(parent) { widget = w; }
    void mousePressEvent(QGraphicsSceneMouseEvent *event)
    {
            QGraphicsScene::mousePressEvent(event);
            if (!event->isAccepted())
                widget->mousePressEvent(event);
    }

};

//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0)
{
    MyQGraphicsScene *scene = new MyQGraphicsScene(this,parent);//this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    Shift = false;
    //scene->setSceneRect(-parent->width(),- parent->height(),parent->width(),parent->height());
    scene->setSceneRect(0,0,parent->width(),parent->height());
    setScene(scene);
    setCacheMode(CacheNone);//CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.95), qreal(0.95));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Fuzzy Builder"));
}
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    /*case Qt::Key_Up:
        centerNode->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        centerNode->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        centerNode->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        centerNode->moveBy(20, 0);
        break;*/
    case Qt::Key_Delete:
    {
        QList<Node *> nodes;
        foreach (QGraphicsItem *item, scene()->items()) {
            if (Node *node = qgraphicsitem_cast<Node *>(item)){
                if (node->getSelected())
                {
                    node->DeleteAllEdges();
                    scene()->items().removeOne(item);
                    delete node;
                }
            }
        }
    }
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
   case Qt::Key_Shift:
        Shift = true;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void GraphWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() ==Qt::Key_Shift)
         Shift = false;
    QGraphicsView::keyPressEvent(event);
}
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
return;
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes)
        node->calculateForces();

    bool itemsMoved = false;
    foreach (Node *node, nodes) {
        if (node->advance())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}

void GraphWidget::mousePressEvent(QGraphicsSceneMouseEvent */*event*/)
{
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item)){
            node->setSelected(false);
            node->update();
        }
    }

}

QString GraphWidget::SuggestName(LOGICTYPE t) const
{
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    QString name;
        switch(t)
        {
        case fSETUP:
            name = "setup";
            return name;
            break;
        case fIN:
            name = "In";
            break;
        case fOUT:
            name = "Out";
            break;
        case fAND:
            name = "And";
            break;
        case fOR:
            name = "Or";
            break;
        case fNOT:
            name = "Not";
            break;
        case fFUZZY:
            name = "Fuzzy";
            break;
        case fPID:
            name = "Pid";
            break;
        case fTIMER:
            name = "Timer";
            break;
        case fDEFINE:
            name = "PIN";
        }
        int count = 1;
        QString result;
        result = name;
        result += QString::number(count);
        bool restart = true;
        while(restart) {
            restart = false;
            foreach (Node *node, nodes)
            {
                if (node->getName()==result)
                {
                    count++;
                    result = name;
                    result += QString::number(count);
                    restart = true;
                    break;
                }
            }
        }
        return result;

}


#ifndef QT_NO_WHEELEVENT

void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, -event->delta() / 240.0));
}

#endif


void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
    painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
    painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
/*    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
    */
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::simulate()
{
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    foreach (Node *node, nodes) {
        if (node->GetLogicType()!=fOUT)
                node->Simulate();
    }
    foreach (Node *node, nodes) {
        if (node->GetLogicType()==fOUT)
                node->Simulate();
    }
    foreach (Node *node, nodes) {
            node->update();
    }

}
void GraphWidget::ReadSource(QTextStream &h)
{
    NodeFactory::ReadSource(this,h);
}

Node *GraphWidget::FindNode(QString &name)
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))  {
            if (node->getName()==name)
                return node;
        }
    }
    return 0;
}

void GraphWidget::WriteSource(QTextStream &textstreamHeader, QTextStream &textstreamsrc)
{
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }



    if (verbose)    {
        StartComment(textstreamsrc);
        textstreamsrc << "Header Section\n";
        EndComment(textstreamsrc);
        StartComment(textstreamsrc);
        textstreamsrc << "Includes Section\n";
        EndComment(textstreamsrc);
    }
    foreach (Node *node, nodes) {
        node->setHeaderBeenWritten(false);
    }
    foreach (Node *node, nodes) {
        node->WriteIncludes(textstreamHeader);// will only write out if it hasn't been written yet
        node->setHeaderBeenWritten(true);
    }
    if (verbose) {
        StartComment(textstreamsrc);
        textstreamsrc << "End Includes Section\n";
        EndComment(textstreamsrc);

        StartComment(textstreamsrc);
        textstreamsrc << "Prototypes Section\n";
        EndComment(textstreamsrc);
    }
    foreach (Node *node, nodes)
        node->WriteHeader(textstreamHeader);
    if (verbose)    {
        StartComment(textstreamsrc);
        textstreamsrc << "End Prototypes Section\n";
        EndComment(textstreamsrc);
        StartComment(textstreamsrc);
        textstreamsrc << "Start Defines Section\n";
        EndComment(textstreamsrc);
    }
    foreach (Node *node, nodes) {
        node->setSourceBeenWritten(false);
    }


    foreach(Node *node,nodes)
        if (node->GetLogicType()==fDEFINE)
            node->WriteSource(textstreamsrc);
    if (verbose) {
        StartComment(textstreamsrc);
        textstreamsrc << "End Defines Section\n";
        EndComment(textstreamsrc);

        StartComment(textstreamsrc);
        textstreamsrc << "End Header Section\n";
        EndComment(textstreamsrc);

    }



    {
        bool setupdone = false;

    foreach (Node *node,nodes) {
        if (node->GetLogicType()==fSETUP)   {
            StartComment(textstreamsrc);
            textstreamsrc << "//the setup routine runs once when you press reset\n";
            EndComment(textstreamsrc);
            node->WriteSource(textstreamsrc);
            setupdone = true;
            break; // only one setup
        }
    }
    if (!setupdone) {
        StartComment(textstreamsrc);
        textstreamsrc << "//the setup routine runs once when you press reset\n";
        EndComment(textstreamsrc);
        textstreamsrc << "void setup()\n{\n";
        foreach (Node *node,nodes)
            textstreamsrc << node->InitizationCode();
        textstreamsrc << "}\n\n";
    }
    }
    StartComment(textstreamsrc);
    textstreamsrc << "// the loop routine runs over and over again forever\n";
    EndComment(textstreamsrc);
    textstreamsrc << "void loop() {\n";
    textstreamsrc << "//each output is called and it calls any inputs connected to it\n";
    foreach (Node *node, nodes) {
        if (node->GetLogicType()==fOUT)
            textstreamsrc << "  " << node->getName() << "();\n";
    }
    textstreamsrc << "}\n\n\n";
    if (verbose)    {
    StartComment(textstreamsrc);
    textstreamsrc<< "These are the Logic Blocks written by the Fuzzybuilder\n";
    EndComment(textstreamsrc);
    }
    foreach (Node *node, nodes) {
        if (node->GetLogicType()==fOUT)
            node->WriteSource(textstreamsrc); // the output nodes write out any connections they have to them recursively
    }
    bool notice = true;

    foreach (Node *node, nodes) {
        if (!node->getSourceBeenWritten())
        {
            if (notice) {
                notice = false;
                textstreamsrc << "\n\n";
                StartComment(textstreamsrc);
                textstreamsrc << "// the following Logic Block(s) are not used\n//But are available for use with the FuzzyBuilder\n";
                EndComment(textstreamsrc);
            }
            node->WriteSource(textstreamsrc);
        }
    }

    StartComment(textstreamsrc);
    textstreamsrc << "//FuzzyBuilder Layout Section\n";
    EndComment(textstreamsrc);

    foreach(Node *node,nodes)   {
        node->WriteNodeInfo(textstreamsrc);
    }
    foreach(Node *node,nodes)   {
        node->WriteEdges(textstreamsrc);
    }

    if (verbose)    {
        StartComment(textstreamsrc);
        textstreamsrc << "End FuzzyBuilder Layout Section\n";
        EndComment(textstreamsrc);


        StartComment(textstreamsrc);
        textstreamsrc << "FuzzyBuilder Group Section\n";
        EndComment(textstreamsrc);
    }
    wp->WriteGroups(textstreamsrc);
    if (verbose) {
        StartComment(textstreamsrc);
        textstreamsrc << "End FuzzyBuilder Group Section\n";
        EndComment(textstreamsrc);
    }

}


void GraphWidget::shuffle()
{
    foreach (QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

