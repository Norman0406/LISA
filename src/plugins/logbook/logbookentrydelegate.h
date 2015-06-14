#ifndef LOGBOOKENTRYDELEGATE_H
#define LOGBOOKENTRYDELEGATE_H

#include <QStyledItemDelegate>

namespace Logbook {
namespace Internal {

namespace Ui {
class LogbookEntryWidget;
}

class LogbookEntryDelegate
        : public QStyledItemDelegate
{
public:
    LogbookEntryDelegate(Ui::LogbookEntryWidget*, QObject* parent = 0);

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel* model, const QModelIndex &index) const;

private:
    Ui::LogbookEntryWidget* m_uiData;
};

}
}

#endif // LOGBOOKENTRYDELEGATE_H
