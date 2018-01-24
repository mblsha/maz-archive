/****************************************************************************
** Form interface generated from reading ui file './alarm.ui'
**
** Created: Втр Июн 11 12:58:09 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ALARM_H
#define ALARM_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QFrame;
class QLabel;
class QPushButton;
class QSlider;

class Alarm : public QDialog
{ 
    Q_OBJECT

public:
    Alarm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Alarm();

    QLabel* TextLabel1;
    QSlider* alarmVolume;
    QLabel* pixmap;
    QLabel* TextLabel2;
    QLabel* alarmMessage;
    QFrame* Line1;
    QPushButton* done;


protected:
    QVBoxLayout* AlarmLayout;
    QHBoxLayout* Layout1;
    QHBoxLayout* Layout9;
    QGridLayout* Layout7;
    QVBoxLayout* Layout8;
    QHBoxLayout* Layout6;
    QHBoxLayout* Layout10;
};

#endif // ALARM_H
