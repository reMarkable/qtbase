/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QEVDEVMOUSEHANDLER_H
#define QEVDEVMOUSEHANDLER_H

#include <QObject>
#include <QString>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSocketNotifier;

class QEvdevMouseHandler : public QObject
{
    Q_OBJECT
public:
    static QEvdevMouseHandler *createLinuxInputMouseHandler(const QString &key, const QString &specification);
    ~QEvdevMouseHandler();

private slots:
    void readMouseData();

private:
    QEvdevMouseHandler(int deviceDescriptor, bool compression, bool clamp, bool smooth, int jitterLimit, int xoffset, int yoffset);

    void sendMouseEvent();

    QSocketNotifier *m_notify;
    int m_x, m_y;
    int m_prevx, m_prevy;
    int m_fd;
    bool m_compression;
    bool m_clamp;
    bool m_smooth;
    int m_xoffset, m_yoffset;
    Qt::MouseButtons m_buttons;
    int m_smoothx, m_smoothy;
    int m_jitterLimitSquared;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QEVDEVMOUSEHANDLER_H
