#ifndef LINEEDITFREQUENCY_H
#define LINEEDITFREQUENCY_H

#include <QLineEdit>

namespace Logbook {
namespace Internal {

class LineEditFrequency
        : public QLineEdit
{
    Q_OBJECT

public:
    LineEditFrequency(QWidget* parent = 0);

    void setFrequency(double);
    double getFrequency() const;

signals:
    void frequencyChanged(double);

private slots:
    void valueChanged(const QString&);

protected:
    void keyPressEvent(QKeyEvent*);

private:
    double m_frequency;
    QString m_prevText;
};

}
}

#endif // LINEEDITFREQUENCY_H
