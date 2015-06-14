#include "logbookentrydelegate.h"
#include "ui_logbookentrywidget.h"
#include <QStyledItemDelegate>
#include <QDateTimeEdit>

using namespace Logbook::Internal;

LogbookEntryDelegate::LogbookEntryDelegate(Ui::LogbookEntryWidget* ui, QObject* parent)
    : QStyledItemDelegate(parent),
      m_uiData(ui)
{
}

void LogbookEntryDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (index.column() == 1) {
        QDateTimeEdit* dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        Q_ASSERT(dateTimeEdit);

        QDateTime dateTime = index.data().toDateTime();
        if (dateTimeEdit == m_uiData->dateTimeEdit_QSO_Date) {
            dateTimeEdit->setDate(dateTime.date());
        }
        else if (dateTimeEdit == m_uiData->dateTimeEdit_QSO_UTC) {
            dateTimeEdit->setTime(dateTime.time());
        }
    }
    else {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void LogbookEntryDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (index.column() == 1) {
        QDateTimeEdit* dateTimeEdit = qobject_cast<QDateTimeEdit*>(editor);
        Q_ASSERT(dateTimeEdit);

        if (dateTimeEdit == m_uiData->dateTimeEdit_QSO_Date) {
            QDate date = dateTimeEdit->date();
            QTime time = m_uiData->dateTimeEdit_QSO_UTC->time();

            QDateTime combined(date, time);
            model->setData(index, combined);
        }
        else if (dateTimeEdit == m_uiData->dateTimeEdit_QSO_UTC) {
            // do nothing
        }
    }
    else {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}
