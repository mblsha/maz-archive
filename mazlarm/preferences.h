/****************************************************************************
** Form interface generated from reading ui file './preferences.ui'
**
** Created: Втр Июн 11 12:58:08 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;
class QSlider;
class QSpinBox;
class QTabWidget;
class QWidget;

class Preferences : public QDialog
{ 
    Q_OBJECT

public:
    Preferences( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Preferences();

    QTabWidget* TabWidget2;
    QWidget* tab;
    QSlider* alarmVolume;
    QMultiLineEdit* alarmMessage;
    QLabel* volumeLabel;
    QLabel* TextLabel4;
    QWidget* tab_2;
    QLineEdit* alarmFile;
    QPushButton* alarmBrowse;
    QLabel* fileLabel;
    QLabel* timeLabel;
    QSpinBox* hour;
    QLabel* TextLabel3;
    QSpinBox* minute;
    QPushButton* alarmTest;
    QCheckBox* fileIsApplication;
    QWidget* tab_3;
    QLabel* TextLabel1;
    QComboBox* fmodOutput;
    QComboBox* fmodDriver;
    QLabel* TextLabel2;
    QFrame* Line3;
    QPushButton* ok;
    QPushButton* cancel;


public slots:
    virtual void browseFile();

protected slots:
    virtual void testAlarm();

protected:
    QVBoxLayout* PreferencesLayout;
    QGridLayout* tabLayout;
    QVBoxLayout* Layout15;
    QGridLayout* tabLayout_2;
    QGridLayout* tabLayout_3;
    QHBoxLayout* Layout2;
};

#endif // PREFERENCES_H
