#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */

    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    /*
     * Соединяем сигнал, который передает ответ от БД с методом, который отображает ответ в ПИ
     */
     //connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    /*
     *  Сигнал для подключения к БД
     */
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);

    /*
     * Соединяем сигнал, который запрос к БД
     */
    connect(dataBase, &DataBase::sig_SendStatusRequest, this, &MainWindow::ReceiveStatusRequestToDB);

}

MainWindow::~MainWindow()
{
    delete ui;
    //delete tbWid;//ругается значит не нужно
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно.
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */

void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */

    if(ui->lb_statusConnect->text() == "Отключено"){

       ui->lb_statusConnect->setText("Подключение");
       ui->lb_statusConnect->setStyleSheet("color : black");


       auto conn = [this]{dataBase->ConnectToDataBase(dataForConnect);};
       QtConcurrent::run(conn);

    }
    else
    {
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }
}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
    if (ui->cb_category->currentIndex() + 1 == requestType::requestHorrors)
    {
        request = request_horror_category;
    }
    if (ui->cb_category->currentIndex() + 1 == requestType::requestComedy)
    {
        request = request_comedy_category;
    }
    auto request_to_db = [this]{dataBase->RequestToDB(ui->cb_category->currentIndex() + 1, request);};
    QtConcurrent::run(request_to_db);//run lambda[](){RequestToDB} in new thread to ui processing without stop

}

/*!
 * \brief Метод изменяет состояние формы в зависимости от статуса подключения к БД
 * \param status
 */
void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if (status)
    {
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else
    {
        dataBase->DisconnectFromDataBase(DB_NAME);
        msg->setIcon(QMessageBox::Critical);
        msg->setText(dataBase->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }
}

void MainWindow::ReceiveStatusRequestToDB(QSqlQueryModel* model)//QSqlError err)
{
    const QSqlError err = model->lastError();
    if (err.isValid())
    {
        msg->setText(err.text());
        msg->show();
    }
    else
    {
        ui->tv_result->setModel(model);
        if (ui->cb_category->currentIndex() == 0)
        {
            ui->tv_result->hideColumn(0);
        } else
        {
             ui->tv_result->showColumn(0);
        }

        //dataBase->ReadAnswerFromDB(ui->cb_category->currentIndex() + 1);//
    }
}

void MainWindow::on_pb_clear_clicked()
{
    ui->tv_result->setModel(new QSqlQueryModel);
}

/*!
 * \brief Слот отображает значение в QTableWidget
 * \param widget
 * \param typeRequest
 */
/*void MainWindow::ScreenDataFromDB(QTableWidget *widget, int typeRequest)
{
    switch (typeRequest) {

    case requestAllFilms:
    case requestHorrors:
    case requestComedy:{

        ///Отобразить данные
        ui->tb_result->setColumnCount(widget->columnCount());
        ui->tb_result->setRowCount(widget->rowCount());

        QStringList hdrs;
        for (int i = 0; i<widget->columnCount(); ++i)
        {
            hdrs << widget->horizontalHeaderItem(i)->text();
        }

        ui->tb_result->setHorizontalHeaderLabels(hdrs);
        for (int i=0; i<widget->rowCount(); ++i)
        {
            for (int j = 0; j<widget->columnCount(); ++j)
            {
                ui->tb_result->setItem(i, j, widget->item(i,j)->clone( ));
            }
        }
        //ui->tb_result->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->tb_result->update( );


        //ui->tb_result = widget;
        //ui->tb_result->update();
        //ui->centralwidget->layout()->addWidget(ui->tb_result);
        //ui->centralwidget->layout()->addWidget(widget);можно ли в centralwidget просто засетать view?
        break;

    }
    default:
        break;
    }

}*/
