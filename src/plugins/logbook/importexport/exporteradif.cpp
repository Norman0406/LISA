#include "exporteradif.h"
#include <coreplugin/icore.h>
#include <app/app_version.h>
#include <QFileDialog>
#include <QDateTime>

using namespace Logbook::Internal;

ExporterAdif::ExporterAdif(QObject* parent)
    : Exporter(parent)
{
}

ExporterAdif::~ExporterAdif()
{
}

QString ExporterAdif::getTypeStatic()
{
    return QLatin1String("ADIF");
}

QString ExporterAdif::getType() const
{
    return ExporterAdif::getTypeStatic();
}

void ExporterAdif::iDoExport(QModelIndexList& selection) const
{
    QString filename = QFileDialog::getSaveFileName(Core::ICore::dialogParent(), tr("Export ADIF (%1 entries)").arg(selection.size()), QLatin1String(""), tr("ADIF Files (*.adx)"));
    if (!filename.isEmpty()) {
        QFile file(filename);
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QXmlStreamWriter xml(&file);
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement(QLatin1String("ADX"));

            // write header
            xml.writeStartElement(QLatin1String("HEADER"));
            xml.writeTextElement(QLatin1String("ADIF_VER"), QLatin1String("3.0.4"));
            xml.writeTextElement(QLatin1String("PROGRAMID"), QLatin1String("LISA"));
            xml.writeTextElement(QLatin1String("PROGRAMVERSION"), QLatin1String(Core::Constants::LISA_VERSION_LONG));

            QString timestamp = QDateTime::currentDateTimeUtc().toString(QLatin1String("yyyyMMdd hhmmss"));
            xml.writeTextElement(QLatin1String("CREATED_TIMESTAMP"), timestamp);

            xml.writeEndElement();

            // write records
            xml.writeStartElement(QLatin1String("RECORDS"));

            foreach (QModelIndex index, selection) {

                xml.writeStartElement(QLatin1String("RECORD"));
                const QAbstractItemModel* model = index.model();

                // write fields
                for (int i = 0; i < model->columnCount(); i++) {
                    QModelIndex currentIndex = model->index(index.row(), i);
                    writeAdifField(model->headerData(i, Qt::Horizontal).toString(), model->data(currentIndex), xml);
                }

                xml.writeEndElement();
            }

            xml.writeEndElement();
            xml.writeEndElement();
            xml.writeEndDocument();

            file.close();

            if (file.error()) {
                //QMessageBox::error(this, tr("Error"), tr("Error: Cannot write file:"));
            }
        }
        else {
            //QMessageBox::error(this, tr("Error"), tr("Error: Cannot write file:"));
        }
    }
}

void ExporterAdif::writeAdifField(QString name, QVariant data, QXmlStreamWriter& xml) const
{
    // UNDONE

    if (name == QLatin1String("Callsign"))
        xml.writeTextElement(QLatin1String("CALL"), data.toString());
    else if (name == QLatin1String("DateTime")) {
        QDateTime dateTime = data.toDateTime();
        xml.writeTextElement(QLatin1String("QSO_DATE"), dateTime.toString(QLatin1String("yyyyMMdd")));
        xml.writeTextElement(QLatin1String("TIME_ON"), dateTime.toString(QLatin1String("hhmmss")));
    }
    else if (name == QLatin1String("Name"))
        xml.writeTextElement(QLatin1String("NAME"), data.toString());
    else if (name == QLatin1String("Frequency"))
        xml.writeTextElement(QLatin1String("FREQ"), data.toString());
    else if (name == QLatin1String("Mode"))
        xml.writeTextElement(QLatin1String("MODE"), getModeEnum(data.toString()));
    else if (name == QLatin1String("Comment"))
        xml.writeTextElement(QLatin1String("COMMENT"), data.toString());
}

QString ExporterAdif::getModeEnum(const QString& mode) const
{
    // UNDONE
    return mode;
}

