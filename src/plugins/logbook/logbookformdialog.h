#ifndef LOGBOOKFORMDIALOG_H
#define LOGBOOKFORMDIALOG_H

#include <QWidget>

class QValidator;

namespace Logbook {
namespace Internal {

namespace Ui {
    class LogbookFormDialog;
}
class LogbookWindow;
class QsoEntry;

class LogbookFormDialog
        : public QWidget
{
public:
    LogbookFormDialog(QWidget* parent, LogbookWindow* window);
    virtual ~LogbookFormDialog();
    QMap<QString, QString>* m_data;

signals:
    void AddQso();

public slots:
    void GetQsoData();
    void validateInput();

private slots:
    void on_pushButtonSubmitLogbookForm_clicked();

private:
    Ui::LogbookFormDialog* m_ui;
    LogbookWindow* m_window;
    QValidator* m_rstValidator;
    QsoEntry* m_qsoEntry;
    QStringList m_bands;
    QStringList m_modes;

    void loadDefaults();
    void convertInputToUppercase();
    void setupComboBox();
};

}
}

#endif // LOGBOOKFORMDIALOG_H
