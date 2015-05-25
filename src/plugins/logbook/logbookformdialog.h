#ifndef LOGBOOKFORMDIALOG_H
#define LOGBOOKFORMDIALOG_H

#include <QWidget>

class QValidator;
class QTimer;
class QKeyEvent;
class QSettings;

namespace Logbook {
namespace Internal {

namespace Ui {
    class LogbookFormDialog;
}
class LogbookWindow;
class QsoEntry;
class ProfileData;

class LogbookFormDialog
        : public QWidget
{
    Q_OBJECT

public:
    LogbookFormDialog(QWidget* parent, LogbookWindow* window);
    virtual ~LogbookFormDialog();
    QMap<QString, QString>* m_data;

    void setName(QString);
    void setProfile(const ProfileData*);

signals:
    void AddQso();
    void lookupCallsign(QString);

public slots:
    void validateInput();

private slots:
    void on_pushButtonSubmitLogbookForm_clicked();
    void handleTimer();
    void startTimer();
    void stopTimer();
    void updateFrequency();
    void triggerLookup();
    void tableViewDoubleClicked();

private:
    Ui::LogbookFormDialog* m_ui;
    LogbookWindow* m_window;
    QValidator* m_rstValidator;
    QsoEntry* m_qsoEntry;
    QStringList m_bands;
    QStringList m_modes;
    QTimer* m_qtimer;
    QSettings* m_settings;
    const ProfileData* m_profile;

    void loadDefaults();
    void convertInputToUppercase();
    void setupWidgets();
    void clearForm();
    bool eventFilter(QObject*, QEvent*);
    bool isFormEmpty();
};

}
}

#endif // LOGBOOKFORMDIALOG_H
