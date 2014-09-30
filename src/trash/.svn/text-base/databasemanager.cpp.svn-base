/***************************************************************************
 *   Copyright (C) 2009 by Eric Maeker, MD.                                *
 *   eric.maeker@free.fr                                                   *
 *                                                                         *
 *   This code is free and open source .                                   *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
#include "databasemanager.h"
#include "settings.h"
#include "constants.h"
#include "icore.h"
#include "./configure/mdp.h"
//#include "completeBase.h"

#include <QProgressDialog>
#include <QMessageBox>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QtSql>
#include <QDebug>

enum { WarnDebugMessage = true };

using namespace Common;
using namespace Constants;

static inline  Settings *settings() {return Common::ICore::instance()->settings(); }

DatabaseManager::DatabaseManager(QObject *parent) :
        QObject(parent),
        m_IsDrTuxConfigured(false),
        m_IsAccountancyConfigured(false)
{
}

DatabaseManager::~DatabaseManager()
{}

bool DatabaseManager::connectDrTux(const int driverType)
{
    Q_ASSERT(!settings()->isClean());
    QString log  = settings()->value(Constants::S_DRTUX_DB_LOGIN).toString();
    QString pass = settings()->value(Constants::S_DRTUX_DB_PASSWORD).toString();
    QString host = settings()->value(Constants::S_DRTUX_DB_HOST).toString();
    QString port = settings()->value(Constants::S_DRTUX_DB_PORT).toString();
    QSqlDatabase db1;
    switch (driverType) {
        case Driver_MySQL :
        {
            db1 = QSqlDatabase::addDatabase("QMYSQL", Constants::DB_DRTUX);
            db1.setHostName(host);
            db1.setDatabaseName(Constants::DB_DRTUX);
            db1.setUserName(log);
            db1.setPassword(pass);
            if (WarnDebugMessage)
                qWarning() << "Connecting DrTux Database" << host << Constants::DB_DRTUX << log << pass << port;
            break;
        }
        case Driver_SQLite :
        {
            break;
        }
        case Driver_PostGreSQL :
            break;
    }

    if ((!db1.isOpen()) && (!db1.open())) {
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("Unable to connect DrTux database. Application will not work."),
                             QMessageBox::Abort);
        return false;
    }

    return true;
}

bool DatabaseManager::connectAccountancy(const int driverType)
{
    Q_ASSERT(!settings()->isClean());
    QString log  = settings()->value(Constants::S_COMPTA_DB_LOGIN).toString();
    QString pass = settings()->value(Constants::S_COMPTA_DB_PASSWORD).toString();
    QString host = settings()->value(Constants::S_COMPTA_DB_HOST).toString();
    QString port = settings()->value(Constants::S_COMPTA_DB_PORT).toString();
    QSqlDatabase db;
    switch (driverType) {
        case Driver_MySQL :
        {
            db = QSqlDatabase::addDatabase("QMYSQL", Constants::DB_ACCOUNTANCY);
            db.setHostName(host);
            db.setDatabaseName(Constants::DB_ACCOUNTANCY);
            db.setUserName(log);
            db.setPassword(pass);
            if (WarnDebugMessage)
                qWarning() << "Connecting Accountancy database" << host << Constants::DB_ACCOUNTANCY << log << pass << port;
            break;
        }
        case Driver_SQLite :
        {
            db = QSqlDatabase::addDatabase("QSQLITE", Constants::DB_ACCOUNTANCY);
            db.setDatabaseName(settings()->userResourcesPath() + QDir::separator() + Constants::DB_ACCOUNTANCY + ".db");
            if (WarnDebugMessage)
                qWarning() << "Connecting Accountancy database SQLite" << db.databaseName();
            break;
        }
        case Driver_PostGreSQL :
            break;
    }

    if ((!db.isOpen()) || (!db.open())) {
        if (!createAccountancyDatabaseSchema(driverType)) {
            QMessageBox::warning(qApp->activeWindow(),
                                 tr("Fatal error"),
                                 tr("Unable to connect Accountancy database. Application will not work."),
                                 QMessageBox::Abort);
            qWarning() << "FATAL ERROR : Enable to create the accountancy database";
            return false;
        } else {
            if (WarnDebugMessage)
                qWarning() << "Connected to Accountancy database" << db.databaseName();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::isAccountancyDatabaseCorrupted()
{
    /** \todo Take into account the driver */
    QSqlDatabase db = QSqlDatabase::database("QMYSQL", Constants::DB_ACCOUNTANCY);
    return db.tables().count() != 14;
}

//    QStringList nbOfTables;
//    nbOfTables = db.tables();
//    if (nbOfTables.size() < 14) {
//        QString listOfTables = nbOfTables.join("\n");
//        QMessageBox messBox;
//        messBox.setText(tr("La base comptabilite a moins de 14 tables.\nCes tables sont :\n")
//                        +listOfTables+"\nVoulez vous essayer de compléter la base ?");
//        messBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Close);
//        messBox.setDefaultButton(QMessageBox::Close);
//        int ret = messBox.exec();
//        switch (ret) {
//        case QMessageBox::Ok :
//            tryToCorrectAccountancyDatabase();
//            delete button;
//            break;
//        case QMessageBox::Close :
//            break;
//        default :
//                break;
//    }
//
//    }
//}

bool DatabaseManager::createFirstSqlConnection() const
{
     qDebug() << "in createFirstSqlConnection";
     if(!settings()->writeAllDefaults()) {
         return false;
     }
     mdp * databaseData = new mdp;
     if(databaseData->exec() == QDialog::Accepted) {
         settings()->setValue(Constants::S_COMPTA_DB_LOGIN,   databaseData->mdpLogin());
         settings()->setValue(Constants::S_COMPTA_DB_PASSWORD,databaseData->mdpPassword());
         settings()->setValue(Constants::S_COMPTA_DB_HOST,    databaseData->mdpHost());

     }
     QSqlDatabase db = QSqlDatabase :: addDatabase(Constants::MYSQL,Constants::DB_FIRST_CONNECTION);
     db.setHostName(settings()->value(S_COMPTA_DB_HOST).toString());
     db.setDatabaseName("mysql");
     db.setUserName(settings()->value(S_COMPTA_DB_LOGIN).toString());
     db.setPassword(settings()->value(S_COMPTA_DB_PASSWORD).toString());
     if(!db.open()) {
         qWarning() << "la base firstConnection ne peut être ouverte.";
     return false;
     }
  return true;
}

bool DatabaseManager::createAccountancyDatabase(const int driver) const
{
    qDebug()<< "in createAccountancyDatabase";
    if (driver==Driver_SQLite)
        return true;

    if (!createFirstSqlConnection()) {
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during accountancy database creation.\nDo you have ")+Constants::MYSQL+""
                             ""+tr(" installed ?"),
                             QMessageBox::Abort);
    }
    QSqlDatabase dbFirst = QSqlDatabase::database(Constants::DB_FIRST_CONNECTION);
    QString reqForA = "CREATE DATABASE comptabilite";
    QSqlQuery q(dbFirst);
    q.exec(reqForA);
    if (q.lastError().isValid() == false) {
        QMessageBox::warning(qApp->activeWindow(),
                             tr("Fatal error"),
                             tr("An error occured during accountancy database creation."),
                             QMessageBox::Abort);
        if (WarnDebugMessage)
            qWarning() << "SQL Error" << q.lastError().text() ;
        return false;
    }
    return true;
}

bool DatabaseManager::createAccountancyDatabaseSchema(const int driver) const
{ 
    if ((driver!=Driver_SQLite) && (!createAccountancyDatabase(driver))) {
        QMessageBox::critical(0, tr("Critical"), tr("Unable to create accountancy database."), QMessageBox::Ok);
        return false;
    }
    QHash<QString, QString> name_sql;
//    name_sql.insert("Database creation", "CREATE DATABASE comptabilite");
    name_sql.insert("Table creation : actes_disponibles",
                    "CREATE TABLE IF NOT EXISTS	actes_disponibles ("
                    "id_acte_dispo  int(10)       UNSIGNED NOT NULL auto_increment,"
                    "nom_acte 	    varchar(50)   COLLATE utf8_unicode_ci NOT NULL,"
                    "desc_acte      varchar(200)  COLLATE utf8_unicode_ci NOT NULL,"
                    "type           varchar(100)  COLLATE utf8_unicode_ci NOT NULL,"
                    "montant_total  double        NOT NULL,"
                    "montant_tiers  double        NOT NULL,"
                    "date_effet	    date          NOT NULL,"
                    "PRIMARY KEY(id_acte_dispo));");
    name_sql.insert("Table creation comptes_bancaires",
                    "CREATE TABLE IF NOT EXISTS	comptes_bancaires ("
                    "id_compte	      int(10)		UNSIGNED 			NOT NULL	auto_increment 	,"
                    "id_usr 	      int(10) 		UNSIGNED 			NOT NULL 			,"
                    "libelle 	      varchar(150) 	COLLATE utf8_unicode_ci  	NULL 				,"
                    "titulaire 	      varchar(100) 	COLLATE utf8_unicode_ci 	NULL 				,"
                    "nom_banque       varchar(50) 	COLLATE utf8_unicode_ci 	NULL 				,"
                    "rib_code_banque  varchar(5) 	COLLATE utf8_unicode_ci 	NULL 				,"
                    "rib_code_guichet varchar(5) 	COLLATE utf8_unicode_ci  	NULL 				,"
                    "rib_numcompte    varchar(11) 	COLLATE utf8_unicode_ci 	NULL 				,"
                    "rib_cle 	      varchar(2) 	COLLATE utf8_unicode_ci 	NULL 				,"
                    "solde_initial    double 						NULL				,"
                    "remarque 	      blob 		 	 			NULL 				,"
                    "PRIMARY KEY(id_compte));");
    name_sql.insert("Table creation depots",
                    "CREATE TABLE IF NOT EXISTS	depots 	  ("
                    "id_depot  	 int(11)      NOT NULL auto_increment  ,"
                    "id_usr 	 int(11)      NOT NULL,"
                    "id_compte 	 int(10)      UNSIGNED NOT NULL,"
                    "type_depot  varchar(10)  COLLATE utf8_unicode_ci NULL,"
                    "date        date         NOT NULL,"
                    "periode_deb date         NULL,"
                    "periode_fin date         NULL,"
                    "blob_depot  blob         NULL,"
                    "remarque 	 text         COLLATE utf8_unicode_ci NULL,"
                    "valide 	 tinyint(4)   NULL,"
                    "PRIMARY KEY(id_depot));");
    name_sql.insert("Table creation honoraires",
                    "CREATE TABLE IF NOT EXISTS	honoraires	("
                    "id_hono  	   int(11)        UNSIGNED NOT NULL	 auto_increment  ,"
                    "id_usr 	   int(11)        NOT NULL,"
                    "id_drtux_usr  int(11)        NULL,"
                    "patient 	   varchar(75) 	  COLLATE utf8_unicode_ci NOT NULL,"
                    "id_site       varchar(10)    COLLATE utf8_unicode_ci NULL,"
                    "id_payeurs    varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "GUID          varchar(36)    COLLATE utf8_unicode_ci NULL,"
                    "praticien 	   varchar(75)    COLLATE utf8_unicode_ci NOT NULL,"
                    "date          date           NOT NULL,"
                    "acte          blob           NULL,"
                    "acteclair     varchar(12)    COLLATE utf8_unicode_ci NULL,"
                    "remarque      blob           NULL,"
                    "esp           double         NOT NULL,"
                    "chq           double         NOT NULL,"
                    "cb            double         NOT NULL,"
                    "daf           double         NOT NULL,"
                    "autre         double         NOT NULL,"
                    "du            double         NOT NULL,"
                    "du_par        varchar(100)   COLLATE utf8_unicode_ci NULL,"
                    "valide        tinyint(1)     NULL,"
                    "tracabilite   blob           NULL,"
                    "PRIMARY KEY(id_hono));");
    name_sql.insert("Table creation immobilisations",
                    "CREATE TABLE IF NOT EXISTS	immobilisations ("
                    "id_immob  	    int(10)         UNSIGNED NOT NULL auto_increment,"
                    "id_usr 	    int(10)         UNSIGNED NOT NULL,"
                    "id_compte 	    int(10)         UNSIGNED NOT NULL,"
                    "libelle 	    varchar(150)    COLLATE utf8_unicode_ci NULL,"
                    "date_service   date            NOT NULL,"
                    "duree          int(11)         NOT NULL,"
                    "mode           tinyint(1)      NOT NULL,"
                    "valeur         bigint(20) 	    UNSIGNED NOT NULL,"
                    "montant_tva    double          NULL,"
                    "valeur_residuelle bigint(20)   NOT NULL,"
                    "resultat       blob            NOT NULL,"
                    "mouvements     blob            NULL,"
                    "remarque       text            COLLATE utf8_unicode_ci NULL,"
                    "tracabilite    blob            NULL,"
                    "PRIMARY KEY(id_immob));");
    name_sql.insert("Table creation mouvements",
                    "CREATE TABLE IF NOT EXISTS	mouvements	("
                    "id_mouvement  int(10) 	UNSIGNED NOT NULL auto_increment ,"
                    "id_mvt_dispo  int(11)      NOT NULL,"
                    "id_usr        int(10) 	UNSIGNED NOT NULL,"
                    " id_compte    int(10) 	UNSIGNED NOT NULL,"
                    " type         tinyint(3) 	UNSIGNED NOT NULL,"
                    " libelle      varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    " date         date         NOT NULL,"
                    " date_valeur  date         NOT NULL,"
                    " montant      double       UNSIGNED NOT NULL,"
                    "remarque      blob         NULL,"
                    " valide       tinyint(4)   NOT NULL,"
                    " tracabilite  blob         NULL,"
                    " validation   tinyint(1)   NULL,"
                    " detail       varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "PRIMARY KEY(id_mouvement));");
    name_sql.insert("Table creation mouvements_disponibles",
                    "CREATE TABLE IF NOT EXISTS	mouvements_disponibles ("
                    "id_mvt_dispo   int(10)      UNSIGNED NOT NULL auto_increment,"
                    "id_mvt_parent  int(11)      NULL,"
                    "type           tinyint(3)   UNSIGNED NOT NULL,"
                    "libelle 	    varchar(100) COLLATE utf8_unicode_ci NOT NULL,"
                    "remarque 	    blob         NULL,"
                    "PRIMARY KEY(id_mvt_dispo));");
    name_sql.insert("Table creation payeurs",
                    "CREATE TABLE IF NOT EXISTS	payeurs	("
                    "ID_Primkey  	bigint(20)   NOT NULL auto_increment,"
                    "id_payeurs 	bigint(20)   NOT NULL,"
                    "nom_payeur 	varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "adresse_payeur     varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "ville_payeur 	varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "code_postal 	varchar(5)   COLLATE utf8_unicode_ci NULL,"
                    "telephone_payeur   varchar(100) COLLATE utf8_unicode_ci NULL,"
                    "PRIMARY KEY(ID_Primkey));");
    name_sql.insert("Table creation sites",
                    "CREATE TABLE IF NOT EXISTS	sites ("
                    "ID_Primkey   bigint(10)  	NOT NULL auto_increment,"
                    "id_site      varchar(10) 	COLLATE utf8_unicode_ci NOT NULL,"
                    "site 	  varchar(100) 	COLLATE utf8_unicode_ci NULL,"
                    "adresse_site varchar(100) 	COLLATE utf8_unicode_ci NULL,"
                    "cp_site      varchar(5) 	COLLATE utf8_unicode_ci	NULL,"
                    "ville_site   varchar(100) 	COLLATE utf8_unicode_ci NULL,"
                    "telsite 	  varchar(50) 	COLLATE utf8_unicode_ci NULL,"
                    "faxsite 	  varchar(50) 	COLLATE utf8_unicode_ci NULL,"
                    "mailsite 	  varchar(100) 	COLLATE utf8_unicode_ci NULL,"
                    "PRIMARY KEY(ID_Primkey));");
    name_sql.insert("Table creation utilisateurs",
                    "CREATE TABLE IF NOT EXISTS	utilisateurs ("
                    "id_usr  	int(10)  	 	UNSIGNED NOT NULL auto_increment,"
                    "nom_usr 	varchar(150) 		COLLATE utf8_unicode_ci NULL,"
                    "login 	varchar(15) 		COLLATE utf8_unicode_ci	NOT NULL,"
                    "password 	varchar(50) 		COLLATE utf8_unicode_ci	NOT NULL,"
                    "id_drtux_usr 	int(11) 	NULL,"
                    "PRIMARY KEY(id_usr));");
    name_sql.insert("Table creation pourcentages",
                    "CREATE TABLE IF NOT EXISTS	pourcentages ("
                    "id_pourcent	int(10)		UNSIGNED NOT NULL auto_increment,"
                    "type		varchar(100)	COLLATE utf8_unicode_ci NULL,"
                    "pourcentage	varchar(6)	COLLATE utf8_unicode_ci	NOT NULL,"
                    "PRIMARY KEY(id_pourcent));");
    name_sql.insert("Table creation seances",
                    "CREATE TABLE IF NOT EXISTS	seances  ("
                    "id_paiement    int(11)		unsigned NOT NULL auto_increment ,"
                    "id_hono        int(11)  	 	UNSIGNED NOT NULL,"
                    "date           date                NOT NULL,"
                    "id_payeurs     bigint(20)          NOT NULL,"
                    "acte           text                COLLATE utf8_unicode_ci	NULL,"
                    "esp            double 		NOT NULL,"
                    "chq            double 		NOT NULL,"
                    "cb             double 		NOT NULL,"
                    "daf            double 		NOT NULL,"
                    "autre          double 		NOT NULL,"
                    "du             double 		NOT NULL,"
                    "valide 	tinyint(1) 		NULL,"
                    "PRIMARY KEY(id_paiement));");
    name_sql.insert("Table paiements",
                    "CREATE TABLE IF NOT EXISTS paiements ("
                    "id_paiement   int(11)  	 	UNSIGNED  NOT NULL  auto_increment ,"
                    "id_hono  	   int(11)  	 	UNSIGNED  NOT NULL ,"
                    "date  	   date  	 	  	  NOT NULL,"
                    "id_payeurs    bigint(20)  	 	  	  NOT NULL,"
                    "acte  	   blob  	 	          NULL  ,"
                    "esp  	   double  	 	  	  NOT  NULL,"
                    "chq  	   double  	 	  	  NOT  NULL,"
                    "cb  	   double  	 	  	  NOT  NULL,"
                    "daf  	   double  	 	  	  NOT  NULL,"
                    "autre  	   double  	 	  	  NOT  NULL,"
                    "du 	   double  	 	  	  NOT  NULL,"
                    "valide  	   tinyint(1)  	 	  	  NULL ,"
                    "PRIMARY KEY(id_paiement));");
    name_sql.insert("Table creation z_version",
                    "CREATE TABLE IF NOT EXISTS	z_version  ("
                    "infos_version blob NULL);");

    // Mass execute SQL queries
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    if ((!db.isOpen()) && (!db.open())) {
        if (WarnDebugMessage)
            qWarning() << "Unable to create accountancy database, database can not be open.";
        return false;
    }
    qDebug() << "Hash Size "+QString::number(name_sql.size());
    QHashIterator<QString, QString> it(name_sql);
     while (it.hasNext()) {
         it.next();
         QSqlQuery q(db);
         QString req = it.value();
         if (driver==Driver_SQLite) {
             req = req.remove("COLLATE utf8_unicode_ci", Qt::CaseInsensitive).remove("auto_increment", Qt::CaseInsensitive).remove("UNSIGNED", Qt::CaseInsensitive);
         }
         q.exec(req);
         // Is there an error with this req ?
         if (q.lastError().isValid()) {
             QMessageBox::warning(qApp->activeWindow(),
                                  tr("Fatal error"),
                                  tr("An error occured during accountancy database creation.\n"
                                     "Reference : %1\n"
                                     "Error : %2\n\n"
                                     "Application will not work.")
                                  .arg(it.key()).arg(q.lastError().text()),
                                  QMessageBox::Abort);
             if (WarnDebugMessage)
                 qWarning() << "SQL Error" << it.key() << q.lastError().text();
             return false;
         }
     }
     //-------table paiements et remplissage des bases d'initialisation ----------------------------------
   feedAccountancyDatabaseWithDefaults();
   QMessageBox mess;
               mess.setText(trUtf8("La base comptabilite a été installée avec 14 tables.\nVeuillez relancer l'application."));
               mess.setStandardButtons(QMessageBox::Close);
               mess.setDefaultButton(QMessageBox::Close);
               int r = mess.exec();
               if(r == QMessageBox::Close)
                 {
                   qApp->closeAllWindows();
                   qApp->quit();
                 return true;}
    return true;
}

/*bool DatabaseManager::feedAccountancyDatabaseWithDefaults() const
{
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QDir dirDump(Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump");
    QStringList filters;
                filters << "*.sql";
    dirDump.setNameFilters(filters);
    QStringList listDump;
                listDump = dirDump.entryList(filters);
     for(int i = 0 ; i < listDump.size() ; i++){
     qDebug() << Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump/"+listDump[i];
     QStringList list;
     QString strList        = "";
     QFile file(Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/dump/"+listDump[i]);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),listDump[i]+QObject::trUtf8(" non trouvé."),QMessageBox::Ok);
         }
    QTextStream stream(&file);
                stream.setCodec("ISO 8859-1");
       while(!stream.atEnd()){
          QString ligne = stream.readLine();
          strList      += ligne;
           if (ligne.endsWith(";") && (ligne.endsWith("*////;")) == false ){  pour remettre enlever les // dans endsWhith n° 2
 /*               list       << strList;
                strList          = "";
            }
        }
       for(int i = 0; i < list.size() ; i++){
          QSqlQuery query(db);
          query.exec(list[i]);
       }
     }
    return true;
}*/

bool DatabaseManager::feedAccountancyDatabaseWithDefaults() const{
    QHash<QString,QStringList> hash;
    QStringList listeActes;
                listeActes      << "id_acte_dispo" 
                                <<   "nom_acte" 
                                <<  "desc_acte" 
                                << "type" 
                                << "montant_total"  
                                << "montant_tiers" 
                                << "date_effet";
    QStringList listeMouvements;
                listeMouvements <<  "id_mvt_dispo"  
                                <<  "id_mvt_parent" 
                                <<  "type" 
                                <<  "libelle" 
                                <<  "remarque";
    QStringList listPourcentages;
                listPourcentages<<  "id_pourcent"
                                <<  "type"  
                                <<  "pourcentage";
                                
                hash.insert("actes_disponibles",listeActes);
                hash.insert("mouvements_disponibles",listeMouvements);
                hash.insert("pourcentages",listPourcentages);
                
    QSqlDatabase db = QSqlDatabase::database(Constants::DB_ACCOUNTANCY);
    QDir dirDump(Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/csv_files");
    QStringList filters;
                filters << "*.csv";
    dirDump.setNameFilters(filters);
    QStringList listDump;
                listDump = dirDump.entryList(filters);
     for(int i = 0 ; i < listDump.size() ; i++){
         QStringList listNameTable = listDump[i].split(".");
         QString table = listNameTable[0];
     //---------récupérer les champs---------------
         QStringList listFields = hash[table];
         QString csvFile = Common::ICore::instance()->settings()->applicationBundlePath()+"/../resources/csv_files/"+listDump[i];
         QFile file(csvFile);
         if(!file.open(QIODevice::ReadOnly)){
             QMessageBox::warning(0,QObject::tr("Erreur"),QObject::trUtf8("Fichier.csv non trouvé."),QMessageBox::Ok);
         }
         QTextStream stream(&file);
                    stream.setCodec("UTF-8");//ISO 8859-1
         while(!stream.atEnd()){
             QString ligne = stream.readLine();
             qDebug() << "ligne ="+ligne;
                     ligne.replace("\"","'");
                     ligne.remove(QRegExp("[\n]"));
                     ligne.remove(QRegExp("[\r\n]"));
             QStringList listValues = ligne.split(";");
             QStringList fields;
             QStringList values;
             for(int a = 0 ; a < listValues.size() ; a++){
                 fields << listFields[a];
                 values << listValues[a];
             }
             QString fieldsStr = fields.join(",");
             QString valuesStr = values.join(",");
             QString insertStr = "INSERT INTO "+table+" ("+fieldsStr+") "
                                 "VALUES("+valuesStr+")";
             QSqlQuery insertQuery(db);
                       insertQuery.exec(insertStr);
                       qDebug() << "requete insertion ="+insertStr;
         }
         qDebug() << "table ="+ table;
         qDebug() << "csvFile ="+csvFile;

     }
     return true;
}

    
