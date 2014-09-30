/***************************************************************************
 *   Copyright (C) 2009 by Docteur Pierre-Marie Desombre.   *
 *   pm.desombre@medsyn.fr  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "parseCCAM.h"
#include "parseconstants.h"

#include <common/constants.h>
#include <common/icore.h>
#include <common/settings.h>

//#include <catdoc/catdoclib/xls.h>

#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QUuid>
#include <QDate>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>

using namespace Common;
using namespace Constants;
using namespace ParseConstants;
using namespace CCAMfindSpace;

static inline Common::Settings *settings(){return Common::ICore::instance()->settings();}

ParseCcam::ParseCcam(QObject * parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    #ifdef Q_OS_LINUX
    m_codec = settings()->value(CODEC_LINUX).toString();
    #elif defined Q_OS_WIN32
    m_codec = settings()->value(CODEC_WINDOWS).toString();
    #else
        QMessageBox::warning(0,"Warning","OS non support&eacute;.",QMessageBox::Ok);
    #endif
    ccamButton->setEnabled(false);
    ccamButton->hide();
    ngapButton->setEnabled(false);
    ngapButton->hide();
    databaseButton->setEnabled(false);
    pathLabel->setText("");
    activityLabel->setText("");
    m_model = new QStandardItemModel(this);
    qDebug() << __FILE__ << QString::number(__LINE__) << " parseCCAM " ;
    m_ccamBaseThread = new FillCcamBase(0);
    connect(searchButton,SIGNAL(pressed()),this,SLOT(searchCsvFile()));
    connect(quitButton,SIGNAL(pressed()),this,SLOT(close()));
    //connect(ccamButton,SIGNAL(pressed()),this,SLOT(writeCsv()));
    //connect(ngapButton,SIGNAL(pressed()),this,SLOT(writeMedicalProcedureCsv()));
    connect(databaseButton,SIGNAL(pressed()),this,SLOT(fillCcamDatabase()));
    connect(m_ccamBaseThread,SIGNAL(finished()),this,SLOT(ccambasethreadIsFinished())); 
    connect(m_ccamBaseThread,SIGNAL(outThread(const QString&)),this,SLOT(getDataFromThread(const QString&)));
}

ParseCcam::~ParseCcam(){}

void ParseCcam::searchCsvFile()
{
    m_ameli = new FindFile(this,XLS);
    m_soffice = new FindFile(this,SOFFICE);
    QString fileFound;
    QString pathFileFound;
    QString dirPathFileFound;
    
    if (m_ameli->dialog()->exec()==QDialog::Accepted)
    {
    	  fileFound = m_ameli->fileRecord();
    	  pathFileFound = m_ameli->findFilePath();
    	  qDebug() << __FILE__ << QString::number(__LINE__) << "pathFileFound"  << pathFileFound;
    	  dirPathFileFound = pathFileFound;
    	  dirPathFileFound.remove(fileFound);
    	  //qDebug() << __FILE__ << QString::number(__LINE__) << "dirPathFileFound = " << dirPathFileFound;
        }
        qDebug() << __FILE__ << QString::number(__LINE__) << "pathFileFound"  << pathFileFound;
        qDebug() << __FILE__ << QString::number(__LINE__) << "dirPathFileFound = " << dirPathFileFound;
    QString xlsFile = pathFileFound;
    m_csvFile = xlsFile;
    //QFile file(xlsFile);
    QFileInfo fileinfo(xlsFile);
    if (!fileinfo.exists())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << xlsFile + " has not been found" ;
          return;
        }
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    /*QWidget *w = new QWidget(this);
    w = QApplication::activeWindow();
    w->showMinimized();*/
    /*QProcess * p = new QProcess(this);
    QString soffice;    
    #ifdef Q_OS_LINUX
    qWarning() << __FILE__ << QString::number(__LINE__) << "SOFFICE IN LINUX SYSTEM" ;
    soffice = "soffice --invisible -convert-to csv "; 
    soffice += xlsFile;
    soffice += " --outdir "+dirPathFileFound;
    #endif 
    #ifdef Q_OS_WIN32
    if (m_soffice->dialog()->exec()==QDialog::Accepted)
    {
          soffice = m_soffice->findFilePath();
          soffice = "\""+soffice+"\"  ";
          soffice += xlsFile;
        }
    const QString informationWindow = trUtf8("A l'ouverture du fichier CCAM vous devez l'enregistrer en format csv au même emplacement.");
    QMessageBox::information(0,trUtf8("Information"),informationWindow,QMessageBox::Ok);
    #endif   

    qDebug() << __FILE__ << QString::number(__LINE__) << " soffice =" << soffice ;
    p->start(soffice);
    if (!p->waitForStarted())
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "process "+ soffice +" do not start." ;
        QApplication::restoreOverrideCursor();
        return;    	
        }
    if (!p->waitForFinished(3000000))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "process "+ soffice +" do not finish." ;
        QApplication::restoreOverrideCursor();
        return;
        }
    p->closeWriteChannel ();*/
    QString csvString = xlsFile.replace(".xls",".csv");
    QFileInfo csvFileInfo(csvString);
    if (!csvFileInfo.exists())
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << csvString << " don't exists" ;
          
        }

    
    if (!parseAndGetCsv(csvString))
    {
        qWarning() << __FILE__ << QString::number(__LINE__) << "cannot parse csv file" ;
         QApplication::restoreOverrideCursor();
        return;
        }

    //ccamButton->setEnabled(true);
    databaseButton->setEnabled(true);
    searchButton->setEnabled(false);
    QApplication::restoreOverrideCursor();
    //w->showMaximized();
}

/*bool ParseCcam::parseAndGetCsv(QString & csvString)
{
    QString lineCsv;
    int counterTD = 0;
    QRegExp TD = QRegExp(QString("<td*>"),Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp endTD = QRegExp(QString("</td>"),Qt::CaseSensitive,QRegExp::RegExp);
    QFile file(csvString);
    QFileInfo htmlFileInfo(file);
    QString dirPath = htmlFileInfo.absolutePath();
    QString csvFileName = "ccam.csv";
    QFile csvFile(dirPath+QDir::separator()+csvFileName);
    if (!csvFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << csvFileName+" cannot open";
        }
    QTextStream csvStream(&csvFile);
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << csvString+" cannot open !" ;
    	  return false;
        }
    QTextStream stream(&file);
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        ////qDebug() << __FILE__ << QString::number(__LINE__) << " line =" << line ;
        if (line.contains("</tr>"))
        {
            counterTD = 0;
            //qDebug() << __FILE__ << QString::number(__LINE__) << " lineCsv =" << lineCsv ;
            csvStream << lineCsv+"\n";
            lineCsv = QString();
            }
        if (counterTD>0)
        {
            line.remove(TD);
            line.remove(endTD);
            lineCsv += "\""+line+"\";";
            }
        if (line.contains("<tr"))
        {
            //qDebug() << __FILE__ << QString::number(__LINE__) << " counterTD =" << QString::number(counterTD) ;
            ++counterTD;            
            }
        
        }
    return true;
    
}*/

bool ParseCcam::parseAndGetCsv(QString & oldCsvString)
{
    bool ret = true;
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    QString csvString = sineCommaCsvFile(oldCsvString);
    m_csvFileNew = csvString;
    QFile file(csvString);
    //qDebug() << __FILE__ << QString::number(__LINE__) << " file  =" << file.fileName() ;
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << csvString+" cannot open !" ;
    	  ret = false;
        }
    else
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "OPENING "+csvString ;
        }
    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForTr ());
    QString ccamExpr ="[A-Z][A-Z][A-Z][A-Z][0-9][0-9][0-9]";
    QString excludeComma = "^;";
    QString excludeHook = "[[A-Z],";
    QString tinintString = "[1-9]{1}";
    QRegExp codeCcam(ccamExpr,Qt::CaseSensitive,QRegExp::RegExp);
    //QRegExp excludeCommaExp(excludeComma,Qt::CaseSensitive,QRegExp::RegExp);
    //QRegExp excludeHookExp(excludeHook,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp regExpInt(tinintString,Qt::CaseSensitive,QRegExp::RegExp);
    QString keepCCAMString;
    while (!stream.atEnd())
    {
    	QString line = stream.readLine();
    	QStringList listOfItems = line.split(";");
    	if (listOfItems.size()>(ITEMS_ENUM_MaxParam-1) )
    	{
    	        QString code = listOfItems[CODE].remove("\"");
    	        if (code.contains(codeCcam))
    	        {
    	        	  keepCCAMString = code;
    	            }
    	        ////qDebug() << __FILE__ << QString::number(__LINE__) << " code  =" << keepCCAMString ;
    	        QString ccode = listOfItems[C_CODE].remove("\"");
    	        QString text = listOfItems[TEXT].remove("\"");
    	        QString activity = listOfItems[ACTIVITY].remove("\"");
    	        ////qDebug() << __FILE__ << QString::number(__LINE__) << " activity =" << activity ;
    	        QString phase = listOfItems[PHASE].remove("\"");
    	        QString price = listOfItems[PRICE].remove("\"");
    	        QString reimbursment = listOfItems[REIMBURSMENT].remove("\"");
    	        QString agreement = listOfItems[AGREEMENT].remove("\"");
    	        QString exemption = listOfItems[EXEMPTION].remove("\"");
    	        QString regroupment = listOfItems[REGROUPMENT].remove("\"");
    	        QString blobXml = getXmlForBlobItem(activity,phase,reimbursment,agreement,exemption,regroupment);
    	        
    	        QStandardItem * itemCode = new QStandardItem(keepCCAMString);
    	        QStandardItem * itemText = new QStandardItem(text);
    	        QStandardItem * itemPrice = new QStandardItem(price);
                QStandardItem * itemBlob = new QStandardItem(blobXml);
    	        QList<QStandardItem*> listOfStandardItems;
    	        listOfStandardItems << itemCode << itemText << itemPrice << itemBlob;
    	        if (activity.contains(regExpInt) && activity.count() < 2)
    	        {
    	        	  m_model->appendRow(listOfStandardItems);
    	            }
    	          
    	        }//if

   	        }//while

    if (m_model->rowCount()<1)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "m_model size = 0" ;
    	  ret = false;
        }
    QApplication::restoreOverrideCursor();
    return ret;
}

/*bool ParseCcam::parseAndGetCsv()
{
    bool ret = true;
    QString fileFound;
    QString pathFound;
    if (m_ameli->dialog()->exec()==QDialog::Accepted)
    {
    	  fileFound = m_ameli->fileRecord();
    	  pathFound = m_ameli->findFilePath();
        }
     
    //qDebug() << __FILE__ << QString::number(__LINE__) << " found =" << fileFound ;
    //qDebug() << __FILE__ << QString::number(__LINE__) << " pathFound =" << pathFound ;
    QString csvFile = pathFound;
    QFile file(csvFile);
    if (!file.open(QIODevice::ReadOnly))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << csvFile+" cannot open !" ;
    	  ret = false;
        }
    else
    {
    	qWarning() << __FILE__ << QString::number(__LINE__) << "OPENING "+csvFile ;
        }
    QTextStream stream(&file);
    stream.setCodec("UTF8");
    QString ccamExpr ="[A-Z][A-Z][A-Z][A-Z][0-9][0-9][0-9]";
    QString excludeComma = "^;";
    QString excludeHook = "[[A-Z],";
    QRegExp codeCcam(ccamExpr,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp excludeCommaExp(excludeComma,Qt::CaseSensitive,QRegExp::RegExp);
    QRegExp excludeHookExp(excludeHook,Qt::CaseSensitive,QRegExp::RegExp);
    
    while (!stream.atEnd())
    {
    	QString line = stream.readLine();
    	if(line.contains(codeCcam)&& !line.contains(excludeHookExp)&& !line.contains("(")&& !line.contains(excludeCommaExp)){
    	    ////qDebug() << __FILE__ << QString::number(__LINE__) << " line =" << line ;
    	    QStringList listOfItems = line.split(";");
    	    //qDebug() << __FILE__ << QString::number(__LINE__) << "listOfItems.size()  =" << QString::number(listOfItems.size()) ;
    	    if(listOfItems.size()==10){
    	        QString code = listOfItems[CODE].remove("\"");
    	        //qDebug() << __FILE__ << QString::number(__LINE__) << " code  =" << code ;
    	        QString cCode = listOfItems[C_CODE].remove("\"");
    	        QString text = listOfItems[TEXT].remove("\"");
    	        QString activity = listOfItems[ACTIVITY].remove("\"");
    	        QString phase = listOfItems[PHASE].remove("\"");
    	        QString price = listOfItems[PRICE].remove("\"");
    	        QString reimbursment = listOfItems[REIMBURSMENT].remove("\"");
    	        QString agreement = listOfItems[AGREEMENT].remove("\"");
    	        QString exemption = listOfItems[EXEMPTION].remove("\"");
    	        QString regroupment = listOfItems[REGROUPMENT].remove("\"");

    	        QStandardItem * itemCode = new QStandardItem(code);
    	        QStandardItem * itemText = new QStandardItem(text);
    	        QStandardItem * itemPrice = new QStandardItem(price);
                QStandardItem * itemBlob = new QStandardItem(blobXml);
    	        QList<QStandardItem*> listOfStandardItems;
    	        listOfStandardItems << itemCode << itemText << itemPrice;
    	        m_model->appendRow(listOfStandardItems);
   	        }
    	    }
        }
    if (m_model->rowCount()<1)
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "m_model size = 0" ;
    	  ret = false;
        }
    return ret;
}*/

/*void ParseCcam::writeCsv()
{
    QString pathToCcamCsv = qApp->applicationDirPath()+"/../texts/ccam.csv";
    if (QFile::exists(pathToCcamCsv))
    {
    	  if (!QFile::remove(pathToCcamCsv))
    	  {
    	      qWarning() << __FILE__ << QString::number(__LINE__) << "UNABLE TO REMOVE ccam.csv" ;
    	      return;
    	      }
    	  }
    QFile newCsvFile(pathToCcamCsv);
    pathLabel->setText(pathToCcamCsv);
    if (!newCsvFile.open(QIODevice::ReadWrite|QIODevice::Text))
    {
    	  qWarning() << __FILE__ << QString::number(__LINE__) << "unable to create and fill ccam.csv" ;
    	  QMessageBox::warning(0,trUtf8("Warning"),
    	  trUtf8("unable to create and fill ccam.csv"),QMessageBox::Ok);
    	  return;
        }
    QTextStream stream(&newCsvFile);
    stream.setCodec("ISO 8859-1");
    QString delimiterBegin = "\"";
    QString delimiterEnd = "\";";
    int id = 1;
    for (int i = 0; i < m_model->rowCount(); i += 1)
    {
    	int ccamId = id;    	
    	QString code = m_model->data(m_model->index(i,MODEL_CODE),Qt::DisplayRole).toString();
    	QString abstract = m_model->data(m_model->index(i,MODEL_TEXT),Qt::DisplayRole).toString();
    	QString amount = m_model->data(m_model->index(i,MODEL_PRICE),Qt::DisplayRole).toString();
    	QString reimbursment = "70.00";
    	QString curdate = QDate::currentDate().toString("yyyy-MM-dd");
    	QString others = m_model->data(m_model->index(i,MODEL_BLOB),Qt::DisplayRole).toString();
    	if (amount == "Non pris en charge")
    	{
    		  amount = "0.00";
    	    }
    	QString line = delimiterBegin+QString::number(ccamId)+delimiterEnd
    	              +delimiterBegin+code+delimiterEnd
    	              +delimiterBegin+abstract+delimiterEnd
    	              +delimiterBegin+amount+delimiterEnd
    	              +delimiterBegin+reimbursment+delimiterEnd
    	              +delimiterBegin+curdate+delimiterEnd
    	              +delimiterBegin+others+delimiterEnd;
    	stream << line;
    	++id;
        }
    ccamButton->setEnabled(false);
    //ngapButton->setEnabled(true);
    databaseButton->setEnabled(true);
}*/

QString ParseCcam::getXmlForBlobItem(QString& activity,
                                     QString& phase,
                                     QString& reimbursment,
                                     QString& agreement,
                                     QString& exemption,
                                     QString& regroupment)
{

    QString xmlString;
    xmlString += "<?xml version=\"1.0\" encoding=\""+m_codec+"\"?>";
    xmlString += "<activity>";
    xmlString += activity;
    xmlString += "</activity>";
    xmlString += "<phase>";
    xmlString += phase;
    xmlString += "</phase>";
    xmlString += "<reimbursment>";
    xmlString += reimbursment;
    xmlString += "</reimbursment>";
    xmlString += "<agreement>";
    xmlString += agreement;
    xmlString += "</agreement>";
    xmlString += "<exemption>";
    xmlString += exemption;
    xmlString += "</exemption>";
    xmlString += "<regroupment>";
    xmlString += regroupment;
    xmlString += "</regroupment>";
    return xmlString;
}

void ParseCcam::fillCcamDatabase()
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_ccamBaseThread->getStandardItemModel(m_model);
    m_ccamBaseThread->start();
}

void ParseCcam::ccambasethreadIsFinished()
{
         QApplication::restoreOverrideCursor();
         databaseButton->setEnabled(false);
         QFile fileCsv(m_csvFile);
         QFile fileCsvNew(m_csvFileNew);
         fileCsv.remove();
         fileCsvNew.remove();
}

QString ParseCcam::sineCommaCsvFile(QString csvString)
{
    QRegExp nonSeparatingCommas(QString("\"*,*\""));
    QFile file(csvString);
    if (file.isOpen())
    {
          file.close();
        }
    if (!file.open(QIODevice::ReadOnly))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open "+csvString ;
        }
    QTextStream stream(&file);
    QByteArray codecarray = m_codec.toLatin1();
    const char * codecchar = codecarray.data();
    stream.setCodec(codecchar);
    QString newCsvString = csvString.replace(".csv","new.csv");
    QFile newFile(newCsvString);
    if (!newFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
          qWarning() << __FILE__ << QString::number(__LINE__) << "unable to open "+newFile.fileName() ;
        }
    QTextStream newStream(&newFile);
    newStream.setCodec(codecchar);
    //QRegExp changeDoubleSeparator(QString(",(?:\\d\\d\")"));
    QRegExp changeDoubleCommaSeparator (",(?![a-z])");
    QRegExp commaExceptSentenceComma(QString(",(?!\\s\\w|\\d{1,2}\")"));
    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        //line.replace(changeDoubleSeparator,".");
        //line.replace(changeDoubleCommaSeparator,".");
        line.replace(commaExceptSentenceComma,".");
        
        //qDebug() << __FILE__ << QString::number(__LINE__) << " line =" <<  line;
        newStream << line+"\n";
        }
    return newFile.fileName();
}

void ParseCcam::getDataFromThread(const QString & info)
{
    activityLabel->setText(info);
}

