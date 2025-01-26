#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    this->dataBase = new QSqlDatabase();
    //this->view = new QTableView;
    this->queryModel = new QSqlQueryModel;

    //simpleQuery = new QSqlQuery();
    this->tableWidget = new QTableWidget();
}

DataBase::~DataBase()
{
    delete dataBase;
    delete model;
    //delete view;
    delete queryModel;
    delete tableWidget;
    //delete simpleQuery;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    bool status;
    status = dataBase->open();
    emit sig_SendStatusConnection(status);

}
/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{
    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();

}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(int film_category, QString request)
{
    //пример использования QSqlQueryModel
    if (film_category == requestType::requestHorrors || film_category == requestType::requestComedy)
    {
        this->queryModel->setQuery(request, *dataBase);
        this->queryModel->setHeaderData(0, Qt::Horizontal, tr("Название фильма"));
        this->queryModel->setHeaderData(1, Qt::Horizontal, tr("Описание фильма"));
        emit sig_SendStatusRequest(this->queryModel);
    }

    //пример использования TableModel
    if (film_category == requestType::requestAllFilms)
    {
        this->model = new QSqlTableModel(this, *dataBase);
        this->model->setTable("film");
        this->model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        this->model->select();
        emit sig_SendStatusRequest(this->model);
    }

    //пример использования Sql запроса
    //*simpleQuery = QSqlQuery(*dataBase);
    //simpleQuery->exec(request);
    //emit sig_SendStatusRequest(simpleQuery);
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

/*void DataBase::ReadAnswerFromDB(int requestType)
{
    if (requestType == requestType::requestAllFilms)
    {
        //this->view->setModel(this->model);
        //this->view->hideColumn(0);//don't show the ID
        //this->view->show();
    }


    if (requestType == requestType::requestComedy || requestType == requestType::requestHorrors)
    {
        //this->view->setModel(this->queryModel);
        tableWidget->setColumnCount(NUM_COLUMS);
        tableWidget->setRowCount(0);

        QStringList  headers;
        headers << "Название фильма" << "Описание фильма";
        tableWidget->setHorizontalHeaderLabels(headers);
        uint32_t counterRows = 0;

        //начинаем заполнять tableWidget данными
        while (true)
        {
            QModelIndex model_index = this->queryModel->index(counterRows, 0);
            if (!model_index.isValid())
            {
                break;
            }
            QSqlRecord rec = this->queryModel->record(counterRows);
            tableWidget->insertRow(counterRows);
            ++counterRows;

            for (int numColumn = 0; numColumn < NUM_COLUMS; ++numColumn)
            {
                QSqlField field = rec.field(numColumn);
                QVariant variant = field.value();
                QTableWidgetItem *item0 = new QTableWidgetItem(variant.toString());
                tableWidget->setItem(tableWidget->rowCount() - 1, numColumn, item0);
            }
        }
        emit sig_SendDataFromDB(this->model, requestType);



           //QList<QSqlRecord> result;
           //const QModelIndexList mil = this->view->selectionModel()->selectedRows();

            //Q_FOREACH( const QModelIndex & mi, mil)
            //{
            //    if ( mi.isValid() ) {
            //        result.push_back(this->view->record(mi.row()));
            //    }
            //}

        }
    }
*/
