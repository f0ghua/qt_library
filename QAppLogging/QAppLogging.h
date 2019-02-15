#ifndef APPLOGMESSAGE_H
#define APPLOGMESSAGE_H

#include <QLoggingCategory>
#include <QStringList>

// Add global logging categories (not class specific)
Q_DECLARE_LOGGING_CATEGORY(AppCore)
Q_DECLARE_LOGGING_CATEGORY(AppCoreTrace)

#define QLOG_ERROR()        qCCritical(AppCore)
#define QLOG_WARNING()      qCWarning(AppCore)
#define QLOG_INFO()         qCInfo(AppCore)
#define QLOG_DEBUG()        qCDebug(AppCore)
#define QLOG_TRACE()        qCDebug(AppCoreTrace)

#define QAPP_LOGGING_MARK        "9527"
#define QAPP_LOGGING_MARK_TRACE  "Trace" QAPP_LOGGING_MARK

//
// This is a QAPP specific replacement for Q_LOGGING_CATEGORY. It will register
// the category name into a global list. only 2 parameters support, because
// level has no usage here.
//
#define QAPP_LOGGING_CATEGORY(name, string) \
    static QAppLoggingCategory qAppCategory ## name (string); \
    Q_LOGGING_CATEGORY(name, string)

#define QLOG_DEBUG() qCDebug(AppCore)
#define QLOG_TRACE() qCDebug(AppCoreTrace)

struct QAppCategoryOptions {
    QAppCategoryOptions(QString category, bool enable) :
        name(category), isEnable(enable)
    { }

    QString name;
    bool isEnable;
};

class QAppLogging
{
    Q_DISABLE_COPY(QAppLogging)

public :
    enum LogDest {
        eDestNone       = 0x00,
        eDestSystem     = 0x01,
        eDestFile       = 0x02
    };

    enum LogLevel
    {
        TraceLevel      = 0,
        DebugLevel,
        InfoLevel,
        WarnLevel,
        ErrorLevel,
        FatalLevel,
        OffLevel
    };

    static QAppLogging *instance()
    {
        QAppLogging *inst = s_instance.loadAcquire();
        if (!inst) {
            inst = new QAppLogging();
            if (!s_instance.testAndSetRelease(0, inst)) {
                delete inst;
                inst = s_instance.loadAcquire();
            }
        }
        return inst;
    }
    static void installHandler();

    int outputDest() const {return m_outputDest;}
    QString logFileName() const {return m_logFileName;}
    void setOutputDest(int value) {m_outputDest = value;}
    void setLogFileName(const QString &fileName) {m_logFileName = fileName;}

    void registerCategory(const char *category, QtMsgType severityLevel = QtDebugMsg);
    QStringList registeredCategories(void);
    void setCategoryLoggingOn(const QString &category, bool enable);
    bool categoryLoggingOn(const QString &category);
    void setFilterRulesByLevel(LogLevel severityLevel);

private:
    QAppLogging();

    static QAtomicPointer<QAppLogging> s_instance;
    int m_outputDest;
    QString m_logFileName;

    QList<QAppCategoryOptions> _registeredCategories;
};

class QAppLoggingCategory
{
public:
    QAppLoggingCategory(const char *category)
    {
        QAppLogging::instance()->registerCategory(category);
    }
};

#endif // APPLOGMESSAGE_H
