#include "spinboxrst.h"

using namespace Logbook::Internal;

SpinBoxRST::SpinBoxRST(QWidget* parent)
    : QSpinBox(parent)
{
}

QString SpinBoxRST::textFromValue(int value) const
{
    return QString(QLatin1String("%1")).arg(value, 4, 10, QChar(QLatin1Char('0')));
}
