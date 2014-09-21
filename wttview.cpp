#include "wttview.h"
#include <QStringList>
#include <QStandardItemModel>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QDataWidgetMapper>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QGridLayout>
#include <QDate>
#include <QDateEdit>
#include <QRegExp>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>

WTTView::WTTView(QWidget *parent) :
  QWidget(parent),
  _model(NULL)
{
  setupModel();

  _listView = new QListView();
  Q_CHECK_PTR(_listView);
  listView()->setModel(model());
  listView()->setModelColumn(1);
  connect(listView(), SIGNAL(clicked(const QModelIndex&)), this, SLOT(listViewItemActivated(const QModelIndex&)));
  connect(listView()->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(listSelectionCurrentChanged(const QModelIndex&, const QModelIndex&)));

  _nameLabel = new QLabel(tr("Na&me:"));
  _nameEdit = new QLineEdit();
  _phoneLabel = new QLabel(tr("&Phone:"));
  _phoneEdit = new QLineEdit();
  _phoneEdit->setInputMask("\\0\\900-000-000");
  _startLabel = new QLabel(tr("&Start Date:"));
  _startEdit = new QDateEdit();
  _endLabel = new QLabel(tr("&End Date:"));
  _endEdit = new QDateEdit();
  _moneyLabel = new QLabel(tr("&Money"));
  _moneyEdit = new QSpinBox();
  _moneyEdit->setSingleStep(1000);
  _moneyEdit->setMaximum(20000);
  _nextButton = new QPushButton(tr("&Next"));
  _previousButton = new QPushButton(tr("&Previous"));
  _deleteButton = new QPushButton(tr("&Delete"));
  _submitButton = new QPushButton(tr("&Submit"));
  _revertButton = new QPushButton(tr("&Revert"));

  _nameLabel->setBuddy(_nameEdit);
  _phoneLabel->setBuddy(_phoneEdit);
  _startLabel->setBuddy(_startEdit);
  _endLabel->setBuddy(_endEdit);
  _moneyLabel->setBuddy(_moneyEdit);

  _mapper = new QDataWidgetMapper(this);
  _mapper->setModel(model());
  _mapper->addMapping(_nameEdit,  model()->fieldIndex(("name")));
  _mapper->addMapping(_phoneEdit, model()->fieldIndex("phone"));
  _mapper->addMapping(_startEdit, model()->fieldIndex("startDate"));
  _mapper->addMapping(_endEdit,   model()->fieldIndex("endDate"));
  _mapper->addMapping(_moneyEdit, model()->fieldIndex("money"));

  connect(_previousButton, SIGNAL(clicked()), this, SLOT(toPrevious()));
  connect(_nextButton,     SIGNAL(clicked()), this, SLOT(toNext()));
  connect(_deleteButton,   SIGNAL(clicked()), this, SLOT(deleteSelectedRow()));
  connect(_submitButton,   SIGNAL(clicked()), this, SLOT(submit()));
  connect(_revertButton,   SIGNAL(clicked()), this, SLOT(revert()));
  connect(_mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePreviousNextButtons(int)));
  connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateSubmitRevertButtons()));

  QGridLayout *layout = new QGridLayout();
  layout->addWidget(_nameLabel,      0, 0, 1, 1);
  layout->addWidget(_nameEdit,       0, 1, 1, 1);
  layout->addWidget(_previousButton, 0, 2, 1, 1);
  layout->addWidget(_phoneLabel,     1, 0, 1, 1);
  layout->addWidget(_phoneEdit,      1, 1, 1, 1);
  layout->addWidget(_nextButton,     1, 2, 1, 1);
  layout->addWidget(_startLabel,     2, 0, 1, 1);
  layout->addWidget(_startEdit,      2, 1, 1, 1);
  layout->addWidget(_deleteButton,   2, 2, 1, 1);
  layout->addWidget(_endLabel,       3, 0, 1, 1);
  layout->addWidget(_endEdit,        3, 1, 1, 1);
  layout->addWidget(_submitButton,   3, 2, 1, 1);
  layout->addWidget(_moneyLabel,     4, 0, 1, 1);
  layout->addWidget(_moneyEdit,      4, 1, 1, 1);
  layout->addWidget(_revertButton,   4, 2, 1, 1);

  selectFirstItem();

  _addButton = new QPushButton("Add", this);
  Q_CHECK_PTR(_addButton);
  connect(_addButton, SIGNAL(clicked()), this, SLOT(addNewItemAndSelect()));

  QVBoxLayout* l = new QVBoxLayout(this);
  l->addWidget(listView(), 1);
  l->addWidget(_addButton, 0);
  l->addLayout(layout);

  updateSubmitRevertButtons(false);
}

WTTView::~WTTView(void)
{
}

void
WTTView::selectFirstItem(void) const
{
  if(0 == model()->rowCount()) {
    return;
  }
  listView()->selectionModel()->setCurrentIndex(model()->index(0, 0), QItemSelectionModel::ClearAndSelect);
}

void
WTTView::setupModel()
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  //db.setDatabaseName(":memory:");
  db.setDatabaseName("account.sqlite");
  if (!db.open()) {
    QMessageBox::critical(this, tr("Cannot open database"),
      tr("Unable to establish a database connection.\n"
         "This example needs SQLite support. Please read "
         "the Qt SQL driver documentation for information how "
         "to build it."), QMessageBox::Cancel);
    return;
  }

  QSqlQuery query(db);
  query.exec("create table if not exists person (id int primary key, "
             "name varchar(20), phone varchar(10), startDate date, endDate date,money int)");
  query.exec("insert into person values(1, 'Alice', "
             "0922000123, '2012-08-08', '2014-12-12', 5000)");
  query.exec("insert into person values(2, 'Bob', "
             "0922222222, '2012-08-08', '2014-12-12', 6000)");
  query.exec("insert into person values(3, 'Carol', "
             "0922333333, '2012-08-08', '2014-12-12', 7000)");
  query.exec("insert into person values(4, 'Donald', "
             "0922444444, '2012-08-08', '2014-12-12', 8000)");
  query.exec("insert into person values(5, 'Emma', "
             "0922555555, '2012-08-08', '2014-12-12', 9000)");

  _model = new QSqlRelationalTableModel(this);
  model()->setTable("person");
  model()->setEditStrategy(QSqlTableModel::OnManualSubmit);
  model()->select();
}

void
WTTView::addNewItemAndSelect(void)
{
  if(!model()->insertRow(model()->rowCount())){
    QMessageBox::critical(0, tr("Cannot Add Data"),
        tr("Unable to add data."), QMessageBox::Cancel);
    return;
  }
  int row = model()->rowCount() - 1;
  initializeRow(row);
  selectRow(row);
}

void
WTTView::deleteSelectedRow(void)
{
  int row = selectedRow();
  if(-1 == row) {
    return;
  }
  int ret = QMessageBox::question(this, tr("Delete Account"),
              tr("Do you want to delete \"%1\"?").arg(model()->data(model()->index(row, model()->fieldIndex("name"))).toString()),
              QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
  if(QMessageBox::No == ret) {
    return;
  }

  model()->removeRow(row);
  model()->submitAll();

  int nextSelectedRow = row - 1;
  if(0 > nextSelectedRow) {
    nextSelectedRow = 0;
  }
  selectRow(nextSelectedRow);
}

int
WTTView::selectedRow(void) const
{
  QModelIndexList ml = listView()->selectionModel()->selectedIndexes();
  if(0 == ml.size()) {
    return -1;
  }
  QModelIndex m = ml.first();
  return m.row();
}

void
WTTView::toPrevious(void) const
{
  int row = selectedRow();
  if(0 == row) {
    return;
  }
  selectRow(row - 1);
}

void
WTTView::toNext(void) const
{
  int row = selectedRow();
  if((model()->rowCount() - 1) == row) {
    return;
  }
  selectRow(row + 1);
}

void
WTTView::submit()
{
  model()->database().transaction();
  if(model()->submitAll()) {
    model()->database().commit();
    updateSubmitRevertButtons(false);
  } else {
    model()->database().rollback();
    QMessageBox::warning(this, tr("Cannot Submit"), tr("The database reported an error: %1").arg(model()->lastError().text()));
  }
}

void
WTTView::revert()
{
  model()->revertAll();
  updateSubmitRevertButtons(false);
}

void
WTTView::listSelectionCurrentChanged(const QModelIndex& current, const QModelIndex& previous) const
{
  (void) previous;
  _mapper->setCurrentIndex(current.row());
}

void
WTTView::listViewItemActivated(const QModelIndex& index) const
{
  _mapper->setCurrentIndex(index.row());
}

void
WTTView::updatePreviousNextButtons(int row) const
{
  _previousButton->setEnabled(row > 0);
  _nextButton->setEnabled(row < model()->rowCount() - 1);
  if(0 == row) {
    _nextButton->setFocus();
  } else if((model()->rowCount() - 1 )== row) {
    _previousButton->setFocus();
  }
}

void
WTTView::updateSubmitRevertButtons(const bool b) const
{
  _submitButton->setEnabled(b);
  _revertButton->setEnabled(b);
}

void
WTTView::selectRow(const int row) const
{
  listView()->selectionModel()->setCurrentIndex(model()->index(row, 0), QItemSelectionModel::ClearAndSelect);
}

void
WTTView::initializeRow(const int row)
{
  model()->setData(model()->index(row, model()->fieldIndex("id")), row + 1);
  model()->setData(model()->index(row, model()->fieldIndex("name")), "Empty");
  model()->setData(model()->index(row, model()->fieldIndex("phone")), "0900000000");
  model()->setData(model()->index(row, model()->fieldIndex("startDate")), QDateTime::currentDateTime().date());
  model()->setData(model()->index(row, model()->fieldIndex("endDate")), QDateTime::currentDateTime().date());
  model()->setData(model()->index(row, model()->fieldIndex("money")), 5000);
}
