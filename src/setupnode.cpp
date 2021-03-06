#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <qevent.h>
#include <qnamespace.h>
#include <QMessageBox>
#include <QDebug>
#include "graphwidget.h"

#include "setupnode.h"



SetupNode::SetupNode(GraphWidget *graphWidget) : Node(graphWidget)
{
    QRect exposedRect(graphWidget->mapToScene(0,0).toPoint(),graphWidget->viewport()->rect().size());
    //QRect exposedRect(ui->graphicsView->mapToScene(0,0).toPoint(), ui->graphicsView->viewport()->rect().size());
    setPos(20,20);
    if (!FindNewVertPosition(-1))
        FindNewVertPosition(1);

}

QRectF SetupNode::boundingRect() const
{
int width = 40;
int height = 20;
qreal adjust = 2;
return QRectF( -width - adjust, -height - adjust, 2 * width + adjust, 2 * height + adjust);
}

bool SetupNode::AllowAttach(Node *) const
{
    return false;
}

void SetupNode::WriteHeader(QTextStream &h) const
{
    h << "void " << getName() << "(void);\n";
}

void SetupNode::WriteNodeInfo(QTextStream &s) const
{
    QString ps; ps.sprintf("!!%f!!%f\n",pos().rx(),pos().ry());
    s << "//!!fSetup!!" << getName() <<  ps;
    Node::WriteNodeInfo(s);
}

void SetupNode::FunctionData(QString &Return, QString &Parameters, QString &FunctionReturn, bool &HasBrackets) const
{
    Return = "void ";
    Parameters = "()";
    FunctionReturn = "";
    HasBrackets = true;
}

QString SetupNode::Regenerate() const
{
    QString s;
    QList<Node *> nodes;
    foreach (QGraphicsItem *item, scene()->items()) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }
    foreach (Node *node,nodes) {
          s += node->InitizationCode();
    }
    return s;
}

void SetupNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    QRectF r = paintSetup(painter, option);
    QString v;
        v.sprintf("\n%05.5f",getCurrent());
    painter->drawPath(shape());
    painter->drawText(r,Qt::AlignHCenter,getName());
}

void SetupNode::setName(const QString &value)
{
    if (value!="setup") {
        QMessageBox msgBox;
        msgBox.setText("setup function cannot be renamed");
        msgBox.setStandardButtons( QMessageBox::Ok );
        msgBox.exec();
    }
    Node::setName("setup");
}


QPainterPath SetupNode::shape() const
{
    QPainterPath path,epath;
    QRectF r= boundingRect();
    epath.addEllipse(r);
    r.setWidth(r.width()/2);

    //path.addRect(r);
    return epath.subtracted(path);


}
