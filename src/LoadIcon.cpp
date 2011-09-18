#include "LoadIcon.h"

#include <QString>
#include <QPixmap>
#include <QBitmap>
#include <QApplication>

namespace fedup {

QIcon LoadIcon(const QString &filename)
{
	// printf("%s\n", (qApp->applicationDirPath() + "/icons/" + filename).toAscii().data());
	// QPixmap pixmap(qApp->applicationDirPath() + "/icons/" + filename);
	QPixmap pixmap(":/icons/" + filename);
	pixmap.setMask(pixmap.createHeuristicMask());
	return QIcon(pixmap);
}

} // namespace fedup
