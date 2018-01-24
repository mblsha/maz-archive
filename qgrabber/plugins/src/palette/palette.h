#ifndef PALETTE_H
#define PALETTE_H

#include <qwidget.h>
#include <allegro.h>

/*****************************************************************************
 *
 * Class PaletteView
 *
 *****************************************************************************/

class PaletteView : public QWidget
{
	Q_OBJECT

public:
	PaletteView (QWidget *parent = 0, const char *name = 0);
	~PaletteView ();

private:
	QPixmap *pix;

public slots:
	void setPalette (RGB *pal);

protected:
	void paintEvent (QPaintEvent *p);
};

#endif
