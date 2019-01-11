#include "applogmessage.h"
#include "windows.h"

#include <QMutex>
#include <fstream>

QAtomicPointer<AppLogMessage> AppLogMessage::s_instance = 0;

static QtMessageHandler g_oldMsgHandle;

static void msgHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &message)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    do {
        int destOption = AppLogMessage::instance()->outputDest();
        const QString &logFileName = AppLogMessage::instance()->logFileName();
        if (destOption == AppLogMessage::eDestNone) {
            break;
        }

        QString logMessage = qFormatLogMessage(type, context, message);
        logMessage.append(QLatin1Char('\n'));

        if (destOption & AppLogMessage::eDestSystem) {
            OutputDebugString(reinterpret_cast<const wchar_t *>(logMessage.utf16()));
        }

        if (destOption & AppLogMessage::eDestFile) {
            static std::ofstream logFile(logFileName.toLocal8Bit().constData());
            if (logFile) {
                logFile << qPrintable(logMessage);
            }
        }
    } while(0);

    if (type == QtFatalMsg) {
        abort();
    }
}

void AppLogMessage::installHandler()
{
    g_oldMsgHandle = qInstallMessageHandler(msgHandler);
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");
}

AppLogMessage::AppLogMessage()
    : m_outputDest(eDestSystem)
    , m_logFileName("log.txt")
{

}

