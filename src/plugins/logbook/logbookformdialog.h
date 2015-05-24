#ifndef LOGBOOKFORMDIALOG_H
#define LOGBOOKFORMDIALOG_H

#include <QWidget>

class QValidator;
class QTimer;

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
    void handleTimer();

private:
    Ui::LogbookFormDialog* m_ui;
    LogbookWindow* m_window;
    QValidator* m_rstValidator;
    QsoEntry* m_qsoEntry;
    QStringList m_bands;
    QStringList m_modes;
    QTimer* m_qtimer;

    void loadDefaults();
    void convertInputToUppercase();
    void setupWidgets();
};

}
}

#endif // LOGBOOKFORMDIALOG_H
