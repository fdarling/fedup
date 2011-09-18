#ifndef FEDUP_LOADICON_H
#define FEDUP_LOADICON_H

#include <QIcon>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace fedup {

QIcon LoadIcon(const QString &filename);

} // namespace fedup

#endif
