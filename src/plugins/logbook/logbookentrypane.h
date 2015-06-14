#ifndef LOGBOOKENTRYPANE_H
#define LOGBOOKENTRYPANE_H

#include <coreplugin/ioutputpane.h>
#include <QDataWidgetMapper>
#include <QPushButton>
#include <QSqlRelationalTableModel>
#include <QStandardItemModel>
#include <QComboBox>
#include "profiledata.h"

namespace Logbook {
namespace Internal {

namespace Ui {
class LogbookEntryWidget;
}

class Database;
class LogbookEntryDelegate;
class LogbookMode;

class LogbookEntryPane
        : public Core::IOutputPane
{
    Q_OBJECT

public:
    LogbookEntryPane(LogbookMode*, QWidget* parent);
    virtual ~LogbookEntryPane();

    QWidget *outputWidget(QWidget* );
    QList<QWidget*> toolBarWidgets() const;

    QString displayName() const { return tr("Logbook Entry"); }
    int priorityInStatusBar() const { return 1; }

    void visibilityChanged(bool visible);

    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;

    void clearModel();

public slots:
    void setModel(QSqlRelationalTableModel*);
    void updateProfiles(const QList<ProfileData>&);
    void rowSelected(int);

private slots:
    void addQso();
    void newQso();
    void clearQso();
    void deleteQso();
    void selectedProfileChanged(int);
    void updateDateTime();
    void dirty();
    void convertInputToUppercase();
    void frequencyChanged(QString);
    void bandChanged(QString);

private:
    void initUiContents();
    bool checkDirty();
    void resetDirtyFlag();
    void setProfile(const ProfileData*);
    bool inRange(const double& value, const double min, const double max) const;
    QString bandFromFrequency(double) const;
    double frequencyFromBand(QString) const;

    LogbookMode* m_mode;
    QSqlRelationalTableModel* m_model;
    QStandardItemModel* m_editModel;
    int m_selectedRow;
    Ui::LogbookEntryWidget* m_ui;
    QWidget* m_widget;
    QDataWidgetMapper* m_mapper;
    LogbookEntryDelegate* m_itemDelegate;
    QList<QUuid> m_profileUuids;
    QUuid m_selectedProfile;
    const ProfileData* m_profile;
    QTimer* m_dateTimer;
    bool m_isDirty;
    bool m_dateTimerUpdate;

    QStringList m_bands;
    QStringList m_modes;

    bool m_hasPrevData;
    bool m_bandChanged;

    // toolbar
    QList<QWidget*> m_toolbarWidgets;
    QComboBox* m_profileList;
    QPushButton* m_buttonAdd;
    QPushButton* m_buttonClear;
    QPushButton* m_buttonDelete;
};

}
}

#endif // LOGBOOKENTRYPANE_H
