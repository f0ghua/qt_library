#include "QAppLogging.h"
#include "windows.h"

#include <QMutex>
#include <fstream>

#define LOG_INTKEY "appCore"

QAPP_LOGGING_CATEGORY(AppCore,            LOG_INTKEY)
QAPP_LOGGING_CATEGORY(AppCoreTrace,       LOG_INTKEY QAL_TAG_TRACE)

QAtomicPointer<QAppLogging> QAppLogging::s_instance = 0;

static QtMessageHandler g_oldMsgHandle;

static void msgHandler(QtMsgType type,
                    const QMessageLogContext &context,
                    const QString &message)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    do {
        int destOption = QAppLogging::instance()->outputDest();
        const QString &logFileName = QAppLogging::instance()->logFileName();
        if (destOption == QAppLogging::eDestNone) {
            break;
        }

        QString logMessage = qFormatLogMessage(type, context, message);
        logMessage.append(QLatin1Char('\n'));

        if (destOption & QAppLogging::eDestSystem) {
            OutputDebugString(reinterpret_cast<const wchar_t *>(logMessage.utf16()));
        }

        if (destOption & QAppLogging::eDestFile) {
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

QAppLogging::QAppLogging()
    : m_outputDest(eDestSystem)
    , m_logFileName("log.txt")
{

}

void QAppLogging::installHandler()
{
    g_oldMsgHandle = qInstallMessageHandler(msgHandler);
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");
}

void QAppLogging::registerCategory(const char *category, QtMsgType severityLevel)
{
    Q_UNUSED(severityLevel);
    QAppCategoryOptions options(category, true);
    _registeredCategories << options;
}

QStringList QAppLogging::registeredCategories()
{
    QStringList sl;
    foreach (auto options, _registeredCategories) {
        sl.append(options.name);
    }
    return sl;
}

void QAppLogging::setCategoryLoggingOn(const QString &category, bool enable)
{
    QList<QAppCategoryOptions>::iterator it;
    QList<QAppCategoryOptions>::iterator end = _registeredCategories.end();
    for (it = _registeredCategories.begin(); it != end; it++) {
        if (it->name == category) {
            it->isEnable = enable;
            break;
        }
    }

    return;
}

bool QAppLogging::categoryLoggingOn(const QString &category)
{
    bool enable = false;
    foreach (auto options, _registeredCategories) {
        if (options.name == category) {
            enable = options.isEnable;
            break;
        }
    }

    return enable;
}

void QAppLogging::setFilterRulesByLevel(LogLevel severityLevel)
{
    QString filterRules;

    filterRules += QString("*") + QAL_TAG_TAIL + ".debug=false\n";
    filterRules += QString("*") + QAL_TAG_TAIL + ".info=false\n";
    filterRules += QString("*") + QAL_TAG_TAIL + ".warning=false\n";
    filterRules += QString("*") + QAL_TAG_TAIL + ".critical=false\n";
    filterRules += QString("*") + QAL_TAG_TAIL + ".fatal=false\n";

    foreach (auto options, _registeredCategories) {
        QString &category = options.name;
        bool isCategoryEnable = options.isEnable;
        if (!isCategoryEnable) {
            continue;
        }

        if (severityLevel <= TraceLevel) {
            filterRules += category;
            filterRules += ".debug=true\n";
        }
        if (severityLevel <= DebugLevel) {
            if (!category.contains(QAL_TAG_TRACE QAL_TAG_TAIL)) {
                filterRules += category;
                filterRules += ".debug=true\n";
            }
        }
        if (severityLevel <= InfoLevel) {
            filterRules += category;
            filterRules += ".info=true\n";
        }
        if (severityLevel <= WarnLevel) {
            filterRules += category;
            filterRules += ".warning=true\n";
        }
        if (severityLevel <= ErrorLevel) {
            filterRules += category;
            filterRules += ".critical=true\n";
        }
        if (severityLevel <= FatalLevel) {
            filterRules += category;
            filterRules += ".fatal=true\n";
        }
    }

    qDebug() << "Filter rules" << filterRules;
    QLoggingCategory::setFilterRules(filterRules);
}

