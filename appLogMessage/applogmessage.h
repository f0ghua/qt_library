#ifndef APPLOGMESSAGE_H
#define APPLOGMESSAGE_H

#include <QString>

class AppLogMessage
{
    Q_DISABLE_COPY(AppLogMessage)

public :
    enum LogDest {
        eDestNone       = 0x00,
        eDestSystem     = 0x01,
        eDestFile       = 0x02
    };

    static AppLogMessage *instance()
    {
        AppLogMessage *inst = s_instance.loadAcquire();
        if (!inst) {
            inst = new AppLogMessage();
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

private:
    AppLogMessage();

    static QAtomicPointer<AppLogMessage> s_instance;
    int m_outputDest;
    QString m_logFileName;
};

#endif // APPLOGMESSAGE_H
