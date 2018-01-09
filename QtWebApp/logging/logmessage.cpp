/**
  @file
  @author Stefan Frings
*/

#include "logmessage.h"
#include <QThread>

using namespace stefanfrings;

LogMessage::LogMessage(const QtMsgType type, const QString& message, QHash<QString, QString>* logVars, QHash<QString, QString>* globalLogVars,
                       const QString &file, const QString &function, const int line)
{
    this->type=type;
    this->message=message;
    this->file=file;
    this->function=function;
    this->line=line;
    timestamp=QDateTime::currentDateTime();
    threadId=QThread::currentThreadId();

    // Copy the logVars if not null,
    // so that later changes in the original do not affect the copy
    if (logVars)
        this->logVars=*logVars;

    if (globalLogVars)
        this->globalLogVars=*globalLogVars;
}

QString LogMessage::toString(const QString& msgFormat, const QString& timestampFormat) const
{
    QString decorated=msgFormat+"\n";

    QString before = "\"";
    QString after = "|";
    QString msg = qPrintable(message);
    msg.replace(before, after);
    decorated.replace("<msg>", msg);

    if (decorated.contains("<timestamp>"))
    {
        decorated.replace("<timestamp>",timestamp.toString(timestampFormat));
    }

    QString typeNr;
    typeNr.setNum(type);
    decorated.replace("<typeNr>",typeNr);

    switch (type)
    {
        case QtDebugMsg:
            decorated.replace("<type>","DEBUG   ");
            break;
        case QtWarningMsg:
            decorated.replace("<type>","WARNING ");
            break;
        case QtCriticalMsg:
            decorated.replace("<type>","CRITICAL");
            break;
        case QtFatalMsg:
            decorated.replace("<type>","FATAL   ");
            break;
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            decorated.replace("<type>","INFO    ");
            break;
    #endif
        default:
            decorated.replace("<type>",typeNr);
    }

    decorated.replace("<file>",file);
    decorated.replace("<function>",function);
    decorated.replace("<line>",QString::number(line));

    QString threadId;
    threadId.setNum((unsigned long)QThread::currentThreadId());
    decorated.replace("<thread>",threadId);

    // Fill in variables
    if (!logVars.isEmpty())
    {
        QList<QString> keys=logVars.keys();
        foreach (QString key, keys)
        {
            decorated.replace("<"+key+">",logVars.value(key));
        }
    }

    if (!globalLogVars.isEmpty())
    {
        QList<QString> keys=globalLogVars.keys();
        foreach (QString key, keys)
        {
            decorated.replace("<"+key+">",globalLogVars.value(key));
        }
    }

    return decorated;
}

QtMsgType LogMessage::getType() const
{
    return type;
}
