/*
 * Copyright (c) 2014 Emanuel Koczwara Software <emanuel.koczwara@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP


#include <QMainWindow>
#include <QModelIndex>
#include <QStringListModel>
#include <QSortFilterProxyModel>

#include <fiscal-printer/FiscalPrinter.hpp>


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:

  explicit MainWindow(QWidget *parent = 0);
  virtual ~MainWindow();

private slots:

  void on_openButton_clicked();
  void on_closeButton_clicked();

  void on_bellButton_clicked();
  void on_cancelButton_clicked();
  void on_getEnqStatusButton_clicked();
  void on_getDleStatusButton_clicked();
  void on_setErrorHandlingModeButton_clicked();
  void on_getLastErrorButton_clicked();
  void on_getCashRegisterInfo1Button_clicked();
  void on_getCashRegisterInfo2Button_clicked();
  void on_getCashRegisterInfo3Button_clicked();
  void on_getCashRegisterInfo4Button_clicked();
  void on_getCashRegisterInfo5Button_clicked();
  void on_getCashRegisterInfo6Button_clicked();
  void on_getCashRegisterInfo7Button_clicked();
  void on_getVersionInfoButton_clicked();
  void on_getDeviceInfo1Button_clicked();
  void on_getDeviceInfo2Button_clicked();
  void on_beginFiscalMemoryReadByDateButton_clicked();
  void on_beginFiscalMemoryReadByRowButton_clicked();
  void on_getFiscalMemoryRecordButton_clicked();
  void on_setClockButton_clicked();
  void on_getClockButton_clicked();
  void on_setVatRatesButton_clicked();
  void on_setHeaderButton_clicked();
  void on_getHeaderButton_clicked();
  void on_openDrawerButton_clicked();
  void on_setDisplayMessageButton_clicked();
  void on_setDisplayModeButton_clicked();
  void on_setDiscountAlgorithmButton_clicked();
  void on_getServiceCheckDateButton_clicked();
  void on_getServiceLockDateButton_clicked();
  void on_beginTransactionButton_clicked();
  void on_printReceiptLineButton_clicked();
  void on_printDepositLineButton_clicked();
  void on_depositCollectedButton_clicked();
  void on_correctDepositCollectedButton_clicked();
  void on_depositReturnedButton_clicked();
  void on_correctDepositReturnedButton_clicked();
  void on_cancelTransactionButton_clicked();
  void on_confirmTransactionButton_clicked();
  void on_confirmTransactionWithPaymentForms1Button_clicked();
  void on_confirmTransactionWithPaymentForms2Button_clicked();
  void on_paymentFormServiceButton_clicked();
  void on_addDiscountButton_clicked();
  void on_addVatDiscountButton_clicked();
  void on_addSubtotalDiscountButton_clicked();
  void on_extraLineContainerReturnedButton_clicked();
  void on_extraLineContainerReceivedButton_clicked();
  void on_formsOfPaymentClearingButton_clicked();
  void on_extraLineButton_clicked();
  void on_defineInfoLinesButton_clicked();
  void on_euroPaymentButton_clicked();
  void on_finishButton_clicked();
  void on_beginInvoiceButton_clicked();
  void on_finishInvoiceButton_clicked();
  void on_setInvoiceOptionButton_clicked();
  void on_extraLinesInvoiceButton_clicked();
  void on_loginButton_clicked();
  void on_logoutButton_clicked();
  void on_paymentToCashButton_clicked();
  void on_paymentFromCashButton_clicked();
  void on_printCashStateButton_clicked();
  void on_printShiftReportButton_clicked();
  void on_printDailyReportButton_clicked();
  void on_printPeriodicalReportByDateButton_clicked();
  void on_printPeriodicalReportByNumberButton_clicked();
  void on_containerReturnButton_clicked();
  void on_saleReceiptButton_clicked();
  void on_returnOfArticleButton_clicked();
  void on_beginNonFiscalButton_clicked();
  void on_printNonFiscalButton_clicked();
  void on_finishNonFiscalButton_clicked();
  void on_setCustomerIdButton_clicked();
  void on_feedButton_clicked();

  void on_debugGenerateErrorButton_clicked();

  void on_listView_clicked(const QModelIndex &index);

  void on_filterLineEdit_textChanged(const QString &arg1);



private:

  Ui::MainWindow *ui;
  QStringListModel *model;
  QSortFilterProxyModel *proxy;

  fp::FiscalPrinter *printer;


}; // class MainWindow


#endif // MAINWINDOW_HPP
