#include "wttmainwindow.h"
#include "wttview.h"

WTTMainWindow::WTTMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  WTTView* view = new WTTView(this);
  setCentralWidget(view);
  setWindowTitle(tr("Account Manager"));
}

WTTMainWindow::~WTTMainWindow()
{
}
