#ifndef WTTVIEW_H
#define WTTVIEW_H

#include <QWidget>
class QSqlRelationalTableModel;
class QListView;
class QDataWidgetMapper;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QModelIndex;
class QDateEdit;

class WTTView : public QWidget
{
    Q_OBJECT
public:
  explicit WTTView(QWidget *parent = 0);
  virtual ~WTTView(void);

signals:

public slots:
private slots:
  void addNewItemAndSelect(void);
  void deleteSelectedRow(void);
  void toPrevious(void) const;
  void toNext(void) const;
  void submit(void);
  void revert(void);
  void listSelectionCurrentChanged(const QModelIndex& current, const QModelIndex& previous) const;
  void listViewItemActivated(const QModelIndex& index) const;
  void updatePreviousNextButtons(int row) const;
  void updateSubmitRevertButtons(const bool b = true) const;
private:
  int selectedRow(void) const;
  void selectFirstItem(void) const;
  void setupModel(void);
  void selectRow(const int row) const;
  void initializeRow(const int row);

  const QSqlRelationalTableModel* model(void) const {return _model;}
  QSqlRelationalTableModel* model(void) {return const_cast<QSqlRelationalTableModel*>(static_cast<const WTTView*>(this)->model());}
  const QListView* listView(void) const {return _listView;}
  QListView* listView(void) {return const_cast<QListView*>(static_cast<const WTTView*>(this)->listView());}

  QSqlRelationalTableModel* _model;

  QListView*   _listView;
  QLabel*      _nameLabel;
  QLabel*      _phoneLabel;
  QLabel*      _startLabel;
  QLabel*      _endLabel;
  QLabel*      _moneyLabel;
  QLineEdit*   _nameEdit;
  QLineEdit*   _phoneEdit;
  QDateEdit*   _startEdit;
  QDateEdit*   _endEdit;
  QSpinBox*    _moneyEdit;
  QPushButton* _nextButton;
  QPushButton* _previousButton;
  QPushButton* _deleteButton;
  QPushButton* _submitButton;
  QPushButton* _revertButton;
  QPushButton* _addButton;

  QDataWidgetMapper *_mapper;
};

#endif // WTTVIEW_H
