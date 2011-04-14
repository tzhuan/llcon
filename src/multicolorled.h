/******************************************************************************\
 * Copyright (c) 2004-2011
 *
 * Author(s):
 *  Volker Fischer
 *
 * Description:
 *
 * SetLight():
 *  0: Green
 *  1: Yellow
 *  2: Red
 *
 ******************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
\******************************************************************************/

#if !defined ( _MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_ )
#define _MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_

#include <qlabel.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qtreewidget.h>
#include <qicon.h>
#include "global.h"


/* Definitions ****************************************************************/
#define DEFAULT_UPDATE_TIME             300

// the red and yellow light should be on at least this interval
#define MIN_TIME_FOR_RED_LIGHT          100


/* Classes ********************************************************************/
class CMultiColorLED : public QLabel
{
    Q_OBJECT

public:
    CMultiColorLED ( QWidget* parent = 0, Qt::WindowFlags f = 0 );

    void Reset();
    void SetUpdateTime ( const int iNUTi );
    void SetLight ( const int iNewStatus );

protected:
    enum ELightColor
    {
        RL_DISABLED,
        RL_GREY,
        RL_GREEN,
        RL_YELLOW,
        RL_RED
    };
    ELightColor eColorFlag;

    virtual void changeEvent ( QEvent* curEvent );
    void UpdateColor();

    QPixmap BitmCubeDisabled;
    QPixmap BitmCubeGrey;
    QPixmap BitmCubeGreen;
    QPixmap BitmCubeYellow;
    QPixmap BitmCubeRed;

    QTimer  TimerRedLight;
    QTimer  TimerGreenLight;
    QTimer  TimerYellowLight;

    int     iUpdateTime;

    bool    bFlagRedLi;
    bool    bFlagGreenLi;
    bool    bFlagYellowLi;

protected slots:
    void         OnTimerRedLight();
    void         OnTimerGreenLight();
    void         OnTimerYellowLight();
    virtual void OnNewPixmap ( const QPixmap& newPixmap ) { setPixmap ( newPixmap ); }

signals:
    void newPixmap ( const QPixmap& newPixmap );
};


class CMultColLEDListViewItem : public CMultiColorLED
{
    Q_OBJECT

public:
    CMultColLEDListViewItem ( const int iNewCol )
        : pListViewItem ( NULL ), iColumn ( iNewCol ) {}

    void SetListViewItemPointer ( QTreeWidgetItem* pNewListViewItem )
    {
        pListViewItem = pNewListViewItem;
    }

protected slots:
    virtual void OnNewPixmap ( const QPixmap& newPixmap )
    {
        if ( pListViewItem != NULL )
        {
            pListViewItem->setIcon ( iColumn, QIcon ( newPixmap ) );
        }
    }

protected:
    QTreeWidgetItem* pListViewItem;
    int              iColumn;
};


class CServerListViewItem : public QTreeWidgetItem
{
public:
    CServerListViewItem ( QTreeWidget* parent )
        : QTreeWidgetItem ( parent ), LED ( 2 )
    {
        LED.SetListViewItemPointer ( this );
    }

    void SetLight ( int iNewStatus )
    {
        LED.SetLight ( iNewStatus );
    }

protected:
    CMultColLEDListViewItem LED;
};


class CConnectionServerListViewItem : public QTreeWidgetItem
{
public:
    CConnectionServerListViewItem ( QTreeWidget* parent )
        : QTreeWidgetItem ( parent ), LED ( 4 )
    {
        LED.SetListViewItemPointer ( this );
    }

    void Reset() { LED.Reset(); }
    void SetUpdateTime ( const int iNUTi ) { LED.SetUpdateTime ( iNUTi ); }
    void SetLight ( int iNewStatus ) { LED.SetLight ( iNewStatus ); }

protected:
    CMultColLEDListViewItem LED;
};

#endif // _MULTCOLORLED_H__FD6B49B5_87DF_48DD_A873_804E1606C2AC__INCLUDED_
