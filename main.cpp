#include "mainwindow.h"

#include <QApplication>
#include <QLoggingCategory>
#include <iostream>


void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   Q_UNUSED(context);

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1] ").arg(dt);

   switch (type)
   {
      case QtDebugMsg:
         txt += QString("{Debug} \t\t %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} \t %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} \t %1").arg(msg);
         break;
      case QtInfoMsg:
         txt += QString("{Info} \t %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} \t\t %1").arg(msg);
         abort();
         break;
   }

   QFile outFile("./logs/LogFile.log");
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);

   QTextStream textStream(&outFile);
   textStream << txt << Qt::endl;
}


int main(int argc, char *argv[])
{
    QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    #ifndef DEBUG
    //qInstallMessageHandler(customMessageHandler);
    #endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
