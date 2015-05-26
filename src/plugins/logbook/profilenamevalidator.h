#ifndef PROFILENAMEVALIDATOR_H
#define PROFILENAMEVALIDATOR_H

#include <QValidator>

namespace Logbook {
namespace Internal {

class ProfileNameValidator
        : public QValidator
{
    Q_OBJECT

public:
    ProfileNameValidator(QObject* parent = 0);
    ~ProfileNameValidator();

    void clear();
    void setNames(QStringList);

public:
    QValidator::State validate(QString& input, int& pos) const;

private:
    QStringList m_names;
};

}
}

#endif // PROFILENAMEVALIDATOR_H
