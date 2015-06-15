#ifndef SPINBOXRST_H
#define SPINBOXRST_H

#include <QSpinBox>

namespace Logbook {
namespace Internal {

class SpinBoxRST
        : public QSpinBox
{
    Q_OBJECT

public:
    SpinBoxRST(QWidget* parent = 0);

    virtual QString textFromValue(int value) const;
};

}
}

#endif // SPINBOXRST_H
