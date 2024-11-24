#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{
    this->dataBase = new QSqlDatabase();
    this->model = new QSqlTableModel;
    this->view = new QTableView;
    this->queryModel = new QSqlQueryModel;

    //simpleQuery = new QSqlQuery();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //tableWidget = new QTableWidget();
}

DataBase::~DataBase()
{
    delete dataBase;
    delete model;
    delete view;
    delete queryModel;
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
        emit sig_SendStatusRequest(this->queryModel->lastError());
    }

    //пример использования TableModel
    if (film_category == requestType::requestAllFilms)
    {
        this->model = new QSqlTableModel(this, *dataBase);
        this->model->setTable("film");
        this->model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        this->model->select();
        emit sig_SendStatusRequest(this->model->lastError( ));
    }




    //пример использования Sql запроса
    //*simpleQuery = QSqlQuery(*dataBase);
    //simpleQuery->exec(request);
    //emit sig_SendStatusRequest(simpleQuery->lastError( ));
}

/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

void DataBase::ReadAnswerFromDB(int requestType)//new
{
    if (requestType == requestType::requestAllFilms)
    {
        this->view->setModel(this->model);
        this->view->hideColumn(0); //don't show the ID
        this->view->show();
    }


    if (requestType == requestType::requestComedy || requestType == requestType::requestHorrors)
    {
        this->view->setModel(this->queryModel);
        this->view->show();

    }













//this->view = new QTableView;
/*
   QSqlTableModel* model = new QSqlTableModel;
       model->setTable("employee");
       model->setEditStrategy(QSqlTableModel::OnManualSubmit);
       model->select();
       model->setHeaderData(0, Qt::Horizontal, tr("Name"));
       model->setHeaderData(1, Qt::Horizontal, tr("Salary"));
*/
       //QTableView* view = new QTableView;


/*
    std::unique_ptr<QTableView> view{new QTableView};
    view->setModel(model);
    view->setItemDelegate(new QSqlRelationalDelegate(view.get()));
*/




    //emit sig_SendDataFromDB(tableWidget, requestType);
}
