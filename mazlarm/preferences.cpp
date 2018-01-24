#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './preferences.ui'
**
** Created: Втр Июн 11 12:59:30 2002
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "preferences.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"14 16 125 2",
"Qt c None",
".d c #1d3d21",
".c c #224a29",
".g c #24462a",
".q c #254c29",
".f c #25502c",
".b c #26512d",
".e c #27552f",
".D c #29532c",
".m c #29572f",
".a c #2a5031",
"#Y c #2b282c",
".h c #2d3730",
".p c #2e6034",
".l c #306537",
".C c #336537",
"#Q c #34333a",
".o c #346c3c",
".i c #346f3d",
".# c #35433c",
".P c #356939",
".k c #366f3f",
".z c #36703f",
"#Z c #37363e",
".r c #377641",
"#4 c #38353a",
".n c #387442",
".B c #397340",
".j c #397642",
"#P c #3a603f",
".v c #3a7744",
".A c #3c6843",
".E c #3c8147",
"#2 c #3d3c44",
".Y c #3d7a44",
".N c #3e7946",
".y c #3e7f47",
".w c #3f8149",
"#6 c #403f46",
".O c #407b45",
"#d c #407f47",
"#1 c #413e45",
"#5 c #414048",
".u c #41744a",
"#X c #424e47",
".G c #42834d",
".s c #428b4e",
"#J c #43874c",
".x c #44884e",
"#M c #458a4e",
"#R c #464f4d",
"#w c #468047",
"#l c #46884d",
".Q c #468e51",
"#3 c #48464e",
"#0 c #48464f",
".t c #489155",
".F c #489654",
"#v c #498e50",
".5 c #4a9354",
".H c #4a9556",
".6 c #4a9757",
"#K c #4b7e46",
".M c #4b9656",
"#D c #4c7a42",
"#f c #4c9656",
"#n c #4c9757",
".X c #4c9856",
"#c c #4c9857",
"#N c #4c9858",
"#k c #4c9958",
"#e c #4c9a58",
"#u c #4d9857",
"#B c #4d9957",
"#m c #4d9d5a",
"#L c #4e8147",
"#S c #4e8758",
"#I c #4e985a",
"#C c #4f9f5b",
"#O c #509c5a",
"#H c #509d5a",
"#U c #509f5c",
".I c #50a05c",
".L c #50a15c",
"#W c #528e5c",
"#T c #52a35e",
"#V c #52a45e",
".Z c #539856",
"#A c #549252",
".4 c #589a56",
"#G c #59884a",
".J c #5ca65e",
".K c #5da55d",
"#i c #5f5944",
".R c #60a05a",
".W c #639f58",
"#o c #657e42",
"#h c #665e44",
".7 c #6c8e4b",
"#g c #748545",
"#. c #756b44",
"#t c #78914c",
"#b c #78944e",
".9 c #7f7344",
"#j c #82914b",
".2 c #897c43",
".8 c #8a7c43",
"#E c #8a8642",
".S c #8d934a",
"#F c #908a44",
".V c #909149",
"## c #928343",
"#x c #938842",
".0 c #948e46",
"#p c #958643",
"#q c #968644",
".3 c #978c44",
".1 c #988943",
"#z c #998d44",
".T c #9a8b43",
".U c #9b8b43",
"#a c #9c8c43",
"#r c #9c8c44",
"#s c #9d8c44",
"#y c #9d8d44",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"Qt.#.a.b.c.dQtQt.e.e.f.g.hQt",
"Qt.i.j.k.l.mQtQtQt.n.o.p.qQt",
".r.s.t.u.v.w.x.x.y.z.A.B.C.D",
".E.FQt.G.H.I.J.K.L.M.NQt.O.P",
"QtQt.Q.M.R.S.T.U.V.W.X.YQtQt",
"QtQt.H.Z.0.1.U.U.2.3.4.5QtQt",
"Qt.6.M.7.U.8.9#.###a#b#c#dQt",
"Qt#e#f#g.U#a#h#i#a#a#j#k#lQt",
"Qt#m#n#o#a#a#p#q#r#s#t#u#vQt",
"QtQt#k#w#x#r#r#s#y#z#A#BQtQt",
"QtQt#C.M#D#E#y#y#F#G#c#HQtQt",
"QtQtQt#I#c#J#K#L#M#N#O#PQtQt",
"QtQt#Q#R#S#T.I#U#V#W#X#YQtQt",
"QtQt#Z#0#1QtQtQtQt#2#3#4QtQt",
"QtQtQt#5QtQtQtQtQtQt#6QtQtQt"};


/* 
 *  Constructs a Preferences which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Preferences::Preferences( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    if ( !name )
	setName( "Preferences" );
    resize( 401, 251 ); 
    setCaption( i18n( "Configure MAZlarm" ) );
    setIcon( image0 );
    PreferencesLayout = new QVBoxLayout( this, 11, 6, "PreferencesLayout"); 

    TabWidget2 = new QTabWidget( this, "TabWidget2" );

    tab = new QWidget( TabWidget2, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    alarmVolume = new QSlider( tab, "alarmVolume" );
    alarmVolume->setMouseTracking( FALSE );
    alarmVolume->setMaxValue( 255 );
    alarmVolume->setLineStep( 50 );
    alarmVolume->setPageStep( 25 );
    alarmVolume->setOrientation( QSlider::Horizontal );
    alarmVolume->setTickmarks( QSlider::Right );
    alarmVolume->setTickInterval( 25 );

    tabLayout->addWidget( alarmVolume, 0, 1 );

    alarmMessage = new QMultiLineEdit( tab, "alarmMessage" );

    tabLayout->addWidget( alarmMessage, 1, 1 );

    volumeLabel = new QLabel( tab, "volumeLabel" );
    volumeLabel->setText( i18n( "Volume:" ) );

    tabLayout->addWidget( volumeLabel, 0, 0 );

    Layout15 = new QVBoxLayout( 0, 0, 6, "Layout15"); 

    TextLabel4 = new QLabel( tab, "TextLabel4" );
    TextLabel4->setText( i18n( "Message:" ) );
    Layout15->addWidget( TextLabel4 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout15->addItem( spacer );

    tabLayout->addLayout( Layout15, 1, 0 );
    TabWidget2->insertTab( tab, i18n( "General" ) );

    tab_2 = new QWidget( TabWidget2, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    alarmFile = new QLineEdit( tab_2, "alarmFile" );

    tabLayout_2->addMultiCellWidget( alarmFile, 1, 1, 1, 4 );

    alarmBrowse = new QPushButton( tab_2, "alarmBrowse" );
    alarmBrowse->setText( i18n( "&Browse" ) );

    tabLayout_2->addWidget( alarmBrowse, 1, 5 );

    fileLabel = new QLabel( tab_2, "fileLabel" );
    fileLabel->setText( i18n( "File:" ) );

    tabLayout_2->addWidget( fileLabel, 1, 0 );

    timeLabel = new QLabel( tab_2, "timeLabel" );
    timeLabel->setText( i18n( "Time:" ) );

    tabLayout_2->addWidget( timeLabel, 0, 0 );

    hour = new QSpinBox( tab_2, "hour" );
    hour->setMinimumSize( QSize( 30, 0 ) );
    hour->setAutoMask( FALSE );
    hour->setMaxValue( 23 );
    hour->setLineStep( 1 );
    hour->setProperty( "frameShape", i18n( "WinPanel" ) );

    tabLayout_2->addWidget( hour, 0, 1 );

    TextLabel3 = new QLabel( tab_2, "TextLabel3" );
    TextLabel3->setText( i18n( ":" ) );

    tabLayout_2->addWidget( TextLabel3, 0, 2 );

    minute = new QSpinBox( tab_2, "minute" );
    minute->setMaxValue( 59 );
    minute->setLineStep( 5 );

    tabLayout_2->addWidget( minute, 0, 3 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    tabLayout_2->addItem( spacer_2, 0, 4 );

    alarmTest = new QPushButton( tab_2, "alarmTest" );
    alarmTest->setText( i18n( "&Test" ) );

    tabLayout_2->addWidget( alarmTest, 0, 5 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_2->addItem( spacer_3, 3, 3 );

    fileIsApplication = new QCheckBox( tab_2, "fileIsApplication" );
    fileIsApplication->setEnabled( TRUE );
    fileIsApplication->setText( i18n( "File is application" ) );

    tabLayout_2->addMultiCellWidget( fileIsApplication, 2, 2, 1, 3 );
    TabWidget2->insertTab( tab_2, i18n( "Alarm" ) );

    tab_3 = new QWidget( TabWidget2, "tab_3" );
    tabLayout_3 = new QGridLayout( tab_3, 1, 1, 11, 6, "tabLayout_3"); 

    TextLabel1 = new QLabel( tab_3, "TextLabel1" );
    TextLabel1->setText( i18n( "Output:" ) );

    tabLayout_3->addWidget( TextLabel1, 0, 0 );

    fmodOutput = new QComboBox( FALSE, tab_3, "fmodOutput" );
    fmodOutput->setEnabled( TRUE );
    fmodOutput->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)0, 0, 0, fmodOutput->sizePolicy().hasHeightForWidth() ) );

    tabLayout_3->addWidget( fmodOutput, 0, 1 );

    fmodDriver = new QComboBox( FALSE, tab_3, "fmodDriver" );
    fmodDriver->setEnabled( TRUE );
    fmodDriver->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, 0, 0, fmodDriver->sizePolicy().hasHeightForWidth() ) );

    tabLayout_3->addWidget( fmodDriver, 1, 1 );

    TextLabel2 = new QLabel( tab_3, "TextLabel2" );
    TextLabel2->setText( i18n( "Driver:" ) );

    tabLayout_3->addWidget( TextLabel2, 1, 0 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    tabLayout_3->addItem( spacer_4, 2, 0 );
    TabWidget2->insertTab( tab_3, i18n( "FMOD" ) );
    PreferencesLayout->addWidget( TabWidget2 );

    Line3 = new QFrame( this, "Line3" );
    Line3->setProperty( "frameShape", (int)QFrame::HLine );
    Line3->setFrameShadow( QFrame::Sunken );
    Line3->setFrameShape( QFrame::HLine );
    PreferencesLayout->addWidget( Line3 );

    Layout2 = new QHBoxLayout( 0, 0, 6, "Layout2"); 
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout2->addItem( spacer_5 );

    ok = new QPushButton( this, "ok" );
    ok->setText( i18n( "&OK" ) );
    ok->setDefault( TRUE );
    Layout2->addWidget( ok );

    cancel = new QPushButton( this, "cancel" );
    cancel->setText( i18n( "&Cancel" ) );
    Layout2->addWidget( cancel );
    PreferencesLayout->addLayout( Layout2 );

    // signals and slots connections
    connect( alarmBrowse, SIGNAL( clicked() ), this, SLOT( browseFile() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( alarmTest, SIGNAL( clicked() ), this, SLOT( testAlarm() ) );

    // tab order
    setTabOrder( hour, minute );
    setTabOrder( minute, ok );
    setTabOrder( ok, alarmFile );
    setTabOrder( alarmFile, alarmBrowse );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Preferences::~Preferences()
{
    // no need to delete child widgets, Qt does it all for us
}

void Preferences::browseFile()
{
    qWarning( "Preferences::browseFile(): Not implemented yet!" );
}

void Preferences::testAlarm()
{
    qWarning( "Preferences::testAlarm(): Not implemented yet!" );
}

#include "preferences.moc"
