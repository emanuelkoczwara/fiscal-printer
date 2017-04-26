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


#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QMessageBox>


using namespace fp;


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  printer(new fp::FiscalPrinter())
{
  ui->setupUi(this);
  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

  QStringList methods;

  methods << "bell"
          << "cancel"
          << "genEnqStatus"
          << "getDleStatus"
          << "setErrorHandlingMode"
          << "getLastError"
          << "getCashRegisterInfo1"
          << "getCashRegisterInfo2"
          << "getCashRegisterInfo3"
          << "getCashRegisterInfo4"
          << "getCashRegisterInfo5"
          << "getCashRegisterInfo6"
          << "getCashRegisterInfo7"
          << "getVersionInfo"
          << "getDeviceInfo1"
          << "getDeviceInfo2"
          << "beginFiscalMemoryReadByDate"
          << "beginFiscalMemoryReadByRow"
          << "getFiscalMemoryRecord"
          << "setClock"
          << "getClock"
          << "setVatRates"
          << "setHeader"
          << "getHeader"
          << "openDrawer"
          << "setDisplayMessage"
          << "setDisplayMode"
          << "setDiscountAlgorithm"
          << "getServiceCheckDate"
          << "getServiceLockDate"
          << "beginTransaction"
          << "printReceiptLine"
          << "printDepositLine"
          << "depositCollected"
          << "correctDepositCorrected"
          << "depositReturned"
          << "correctDepositReturned"
          << "cancelTransaction"
          << "confirmTransaction"
          << "confirmTransactionWithPaymentForms1"
          << "confirmTransactionWithPaymentForms2"
          << "paymentFormService"
          << "addDiscount"
          << "addVatRateDiscount"
          << "addSubtotalDiscount"
          << "extraLineContainerReturned"
          << "extraLineContainerReceived"
          << "formsOfPaymentClearing"
          << "extraLine"
          << "defineInfoLines"
          << "euroPayment"
          << "finish"
          << "beginInvoice"
          << "finishInvoice"
          << "setInvoiceOption"
          << "extraLinesInvoice"
          << "login"
          << "logout"
          << "paymentToCash"
          << "withdrawalFromCash"
          << "printCashState"
          << "printShiftReport"
          << "printDailyReport"
          << "printPeriodicalReportByDate"
          << "printPeriodicalReportByNumber"
          << "containerReturn"
          << "saleReceipt"
          << "returnOfArticle"
          << "beginNonFiscal"
          << "printNonFiscal"
          << "finishNonFiscal"
          << "setClientId"
          << "paperFeed"
          << "debugGenerateError";

  model = new QStringListModel(this);
  model->setStringList(methods);

  proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(model);
  proxy->setFilterKeyColumn(0);

  ui->listView->setModel(proxy);
}


MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_openButton_clicked()
{
  try
  {
    printer->open(ui->device->text().toStdString());

    ui->listView->setEnabled(true);
    ui->stackedWidget->setEnabled(true);
    ui->closeButton->setEnabled(true);
    ui->openButton->setEnabled(false);
  }
  catch (std::exception &e)
  {
    QMessageBox::critical(this, QString::fromUtf8("Wystąpił błąd"), QString::fromUtf8(e.what()));

    printer->close();

    ui->listView->setEnabled(false);
    ui->stackedWidget->setEnabled(false);
    ui->closeButton->setEnabled(false);
    ui->openButton->setEnabled(true);
  }
  catch (...)
  {
    QMessageBox::critical(this, QString::fromUtf8("Wystąpił błąd"), QString::fromUtf8("Wystąpił nieoczekiwany błąd."));

    printer->close();

    ui->listView->setEnabled(false);
    ui->stackedWidget->setEnabled(false);
    ui->closeButton->setEnabled(false);
    ui->openButton->setEnabled(true);
  }
}


void MainWindow::on_closeButton_clicked()
{
  printer->close();

  ui->listView->setEnabled(false);
  ui->stackedWidget->setEnabled(false);
  ui->closeButton->setEnabled(false);
  ui->openButton->setEnabled(true);
}


void MainWindow::on_bellButton_clicked()
{
  printer->bell();
}


void MainWindow::on_cancelButton_clicked()
{
  printer->cancel();
}


void MainWindow::on_getEnqStatusButton_clicked()
{
  EnqStatus status = printer->getEnqStatus();

  ui->getEnqResultFiscal->setText(QString::number((int)status.fiscal));
  ui->getEnqResultCommand->setText(QString::number((int)status.command));
  ui->getEnqResultTransaction->setText(QString::number((int)status.transaction));
  ui->getEnqResultTransactionOk->setText(QString::number((int)status.transactionOk));
}


void MainWindow::on_getDleStatusButton_clicked()
{
  DleStatus status = printer->getDleStatus();

  ui->getDleStatusResultOnline->setText(QString::number((int)status.online));
  ui->getDleStatusResultPaper->setText(QString::number((int)status.paper));
  ui->getDleStatusResultError->setText(QString::number((int)status.error));
}


void MainWindow::on_setErrorHandlingModeButton_clicked()
{
  printer->setErrorHandlingMode((ERROR_HANDLING_MODE)ui->errorHandlingMode->currentIndex());
}


void MainWindow::on_getLastErrorButton_clicked()
{
  PrinterError error = printer->getLastError();

  ui->getLastErrorResultErrorCode->setText(QString::number(error.code));
  ui->getLastErrorResultErrorText->setText(QString::fromUtf8(error.toString().c_str()));
}


void MainWindow::on_getCashRegisterInfo1Button_clicked()
{
  CashRegisterInfo1 info = printer->getCashRegisterInfo1();

  ui->cashRegisterInfo1LastError->setText(QString::number(info.lastError));

  ui->cashRegisterInfo1Fiscal->setText(QString::number((int)info.fiscal));
  ui->cashRegisterInfo1Transaction->setText(QString::number((int)info.transaction));
  ui->cashRegisterInfo1TransactionOk->setText(QString::number((int)info.transactionOk));

  ui->cashRegisterInfo1Resets->setText(QString::number(info.ramResets));

  ui->cashRegisterInfo1LastWriteDate->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));

  ui->cashRegisterInfo1VatA->setText(QString::number(info.vatA, 'f', 2));
  ui->cashRegisterInfo1VatB->setText(QString::number(info.vatB, 'f', 2));
  ui->cashRegisterInfo1VatC->setText(QString::number(info.vatC, 'f', 2));
  ui->cashRegisterInfo1VatD->setText(QString::number(info.vatD, 'f', 2));
  ui->cashRegisterInfo1VatE->setText(QString::number(info.vatE, 'f', 2));
  ui->cashRegisterInfo1VatF->setText(QString::number(info.vatF, 'f', 2));

  ui->cashRegisterInfo1Reciepts->setText(QString::number(info.reciepts));

  ui->cashRegisterInfo1TotA->setText(QString::number(info.totA, 'f', 2));
  ui->cashRegisterInfo1TotB->setText(QString::number(info.totB, 'f', 2));
  ui->cashRegisterInfo1TotC->setText(QString::number(info.totC, 'f', 2));
  ui->cashRegisterInfo1TotD->setText(QString::number(info.totD, 'f', 2));
  ui->cashRegisterInfo1TotE->setText(QString::number(info.totE, 'f', 2));
  ui->cashRegisterInfo1TotF->setText(QString::number(info.totF, 'f', 2));
  ui->cashRegisterInfo1TotG->setText(QString::number(info.totG, 'f', 2));

  ui->cashRegisterInfo1Cash->setText(QString::number(info.cash, 'f', 2));

  ui->cashRegisterInfo1Number->setText(QString::fromStdString(info.number));
}


void MainWindow::on_getCashRegisterInfo2Button_clicked()
{
  CashRegisterInfo2 info = printer->getCashRegisterInfo2((CASH_REGISTER_INFO_2_MODE)ui->cashRegisterInfo2Mode->currentText().toInt(),
    (bool)ui->cashRegisterInfo2Invoices->currentIndex());

  ui->cashRegisterInfo2LastError->setText(QString::number(info.lastError));

  ui->cashRegisterInfo2Fiscal->setText(QString::number((int)info.fiscal));
  ui->cashRegisterInfo2Transaction->setText(QString::number((int)info.transaction));
  ui->cashRegisterInfo2TransactionOk->setText(QString::number((int)info.transactionOk));

  ui->cashRegisterInfo2Resets->setText(QString::number(info.resets));

  ui->cashRegisterInfo2LastWrite->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));

  ui->cashRegisterInfo2VatA->setText(QString::number(info.vatA, 'f', 2));
  ui->cashRegisterInfo2VatB->setText(QString::number(info.vatB, 'f', 2));
  ui->cashRegisterInfo2VatC->setText(QString::number(info.vatC, 'f', 2));
  ui->cashRegisterInfo2VatD->setText(QString::number(info.vatD, 'f', 2));
  ui->cashRegisterInfo2VatE->setText(QString::number(info.vatE, 'f', 2));
  ui->cashRegisterInfo2VatF->setText(QString::number(info.vatF, 'f', 2));
  ui->cashRegisterInfo2VatG->setText(QString::number(info.vatG, 'f', 2));

  ui->cashRegisterInfo2Reciepts->setText(QString::number(info.reciepts));

  ui->cashRegisterInfo2TotA->setText(QString::number(info.totA, 'f', 2));
  ui->cashRegisterInfo2TotB->setText(QString::number(info.totB, 'f', 2));
  ui->cashRegisterInfo2TotC->setText(QString::number(info.totC, 'f', 2));
  ui->cashRegisterInfo2TotD->setText(QString::number(info.totD, 'f', 2));
  ui->cashRegisterInfo2TotE->setText(QString::number(info.totE, 'f', 2));
  ui->cashRegisterInfo2TotF->setText(QString::number(info.totF, 'f', 2));
  ui->cashRegisterInfo2TotG->setText(QString::number(info.totG, 'f', 2));

  ui->cashRegisterInfo2Cash->setText(QString::number(info.cash, 'f', 2));

  ui->cashRegisterInfo2Number->setText(QString::fromStdString(info.number));
}


void MainWindow::on_getCashRegisterInfo3Button_clicked()
{
  CashRegisterInfo3 info = printer->getCashRegisterInfo3();

  ui->cashRegisterInfo3LastWrite->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));

  ui->cashRegisterInfo3FreeReports->setText(QString::number(info.freeReports));
  ui->cashRegisterInfo3UsedReports->setText(QString::number(info.usedReports));
  ui->cashRegisterInfo3Locked->setText(QString::number(info.locked));

  ui->cashRegisterInfo3TotA->setText(QString::number(info.totA, 'f', 2));
  ui->cashRegisterInfo3TotB->setText(QString::number(info.totB, 'f', 2));
  ui->cashRegisterInfo3TotC->setText(QString::number(info.totC, 'f', 2));
  ui->cashRegisterInfo3TotD->setText(QString::number(info.totD, 'f', 2));
  ui->cashRegisterInfo3TotE->setText(QString::number(info.totE, 'f', 2));
  ui->cashRegisterInfo3TotF->setText(QString::number(info.totF, 'f', 2));
  ui->cashRegisterInfo3TotG->setText(QString::number(info.totG, 'f', 2));
}


void MainWindow::on_getCashRegisterInfo4Button_clicked()
{
  CashRegisterInfo4 info = printer->getCashRegisterInfo4();

  ui->cashRegisterInfo4Documents->setText(QString::number(info.documents));
  ui->cashRegisterInfo4Invoices->setText(QString::number(info.invoices));
}


void MainWindow::on_getCashRegisterInfo5Button_clicked()
{
  CashRegisterInfo5 info = printer->getCashRegisterInfo5();

  ui->cashRegisterInfo5Label->setText(QString::fromStdString(info.label));
  ui->cashRegisterInfo5State->setText(QString::fromStdString(info.state));

  ui->cashRegisterInfo5Size->setText(QString::number(info.size));
  ui->cashRegisterInfo5FreeMem->setText(QString::number(info.freeMem));
  ui->cashRegisterInfo5Files->setText(QString::number(info.files));

  ui->cashRegisterInfo5FreeReports->setText(QString::number(info.freeReports));
  ui->cashRegisterInfo5LastReportNr->setText(QString::number(info.lastReportNr));

  ui->cashRegisterInfo5LastWrite->setText(QString::fromStdString(info.lastWrite));
}


void MainWindow::on_getCashRegisterInfo6Button_clicked()
{
  CashRegisterInfo6 info = printer->getCashRegisterInfo6((CASH_REGISTER_INFO_6_MODE)ui->cashRegisterInfo6Mode->currentText().toInt());

  ui->cashRegisterInfo6Type->setText(QString::number(info.type));
  ui->cashRegisterInfo6Transaction->setText(QString::number(info.transaction));

  ui->cashRegisterInfo6Total->setText(QString::number(info.total, 'f', 2));

  ui->cashRegisterInfo6TotA->setText(QString::number(info.totA, 'f', 2));
  ui->cashRegisterInfo6TotB->setText(QString::number(info.totB, 'f', 2));
  ui->cashRegisterInfo6TotC->setText(QString::number(info.totC, 'f', 2));
  ui->cashRegisterInfo6TotD->setText(QString::number(info.totD, 'f', 2));
  ui->cashRegisterInfo6TotE->setText(QString::number(info.totE, 'f', 2));
  ui->cashRegisterInfo6TotF->setText(QString::number(info.totF, 'f', 2));
  ui->cashRegisterInfo6TotG->setText(QString::number(info.totG, 'f', 2));
}


void MainWindow::on_getCashRegisterInfo7Button_clicked()
{
  CashRegisterInfo7 info = printer->getCashRegisterInfo7(ui->cashRegisterInfo7Item->value(),
    (CASH_REGISTER_INFO_7_MODE)ui->cashRegisterInfo7Mode->currentText().toInt());

  ui->cashRegisterInfo7Amount->setText(QString::number(info.amount, 'f', 2));
}


void MainWindow::on_getVersionInfoButton_clicked()
{
  VersionInfo info = printer->getVersionInfo();

  ui->versionInfoType->setText(QString::fromStdString(info.type));
  ui->versionInfoVersion->setText(QString::fromStdString(info.version));
}


void MainWindow::on_getDeviceInfo1Button_clicked()
{
  DeviceInfo1 info = printer->getDeviceInfo1();

  ui->deviceInfo1Name->setText(QString::fromStdString(info.name));

  ui->deviceInfo1SoftVer->setText(QString::fromStdString(info.softwareVersion));
  ui->deviceInfo1PrintModVer->setText(QString::fromStdString(info.printModuleVersion));

  ui->deviceInfo1SysNr->setText(QString::fromStdString(info.systemName));
  ui->deviceInfo1SysVer->setText(QString::fromStdString(info.systemVer));

  ui->deviceInfo1Displays->setText(QString::number(info.displays));

  ui->deviceInfo1PrintWidth->setText(QString::number(info.printingWidth));

  ui->deviceInfo1ECopy->setText(QString::number(info.eCopy));

  ui->deviceInfo1FiscalMemSize->setText(QString::number(info.fiscalMemorySize));
}


void MainWindow::on_getDeviceInfo2Button_clicked()
{
  DeviceInfo2 info = printer->getDeviceInfo2();

  ui->deviceInfo2FiscalMemSize->setText(QString::number(info.fiscalMemorySize));
  ui->deviceInfo2RecordSize->setText(QString::number(info.recordSize));

  ui->deviceInfo2Mode->setText(QString::number(info.mode));

  ui->deviceInfo2UniqueNumber->setText(QString::fromStdString(info.uniqueNumber));

  ui->deviceInfo2Nip->setText(QString::fromStdString(info.nip));

  ui->deviceInfo2MaxRecordsCount->setText(QString::number(info.maxRecordsCount));
  ui->deviceInfo2RecordsCount->setText(QString::number(info.recordsCount));

  ui->deviceInfo2MaxDailyReportsCount->setText(QString::number(info.maxDailyReportsCount));
  ui->deviceInfo2DailyReportsCount->setText(QString::number(info.dailyReportsCount));

  ui->deviceInfo2MaxRamResetsCount->setText(QString::number(info.maxRamResetsCount));
  ui->deviceInfo2RamResetsCount->setText(QString::number(info.ramResetsCount));

  ui->deviceInfo2MaxVatRatesChanesCount->setText(QString::number(info.maxVatRatesChangesCount));
  ui->deviceInfo2VatRatesChangesCount->setText(QString::number(info.vatRatesChangesCount));

  ui->deviceInfo2MaxCurrencyChangesCount->setText(QString::number(info.maxCurrencyChangesCount));
  ui->deviceInfo2CurrencyChangesCount->setText(QString::number(info.currencyChangesCount));
}


void MainWindow::on_beginFiscalMemoryReadByDateButton_clicked()
{
  printer->beginFiscalMemoryReadByDate( ui->fiscalMemotyReadByDateYear->value(),
    ui->fiscalMemotyReadByDateMonth->value(), ui->fiscalMemotyReadByDateDay->value(),
    ui->fiscalMemotyReadByDateHour->value(), ui->fiscalMemotyReadByDateMinute->value(),
    ui->fiscalMemotyReadByDateSecond->value());
}


void MainWindow::on_beginFiscalMemoryReadByRowButton_clicked()
{
  printer->beginFiscalMemoryReadByRow(ui->fiscalMemoryReadByRowNr->value());
}


void MainWindow::on_getFiscalMemoryRecordButton_clicked()
{
  FiscalMemoryRecord *record = printer->getFiscalMemoryRecord();

  if (record == NULL)
  {
    ui->recordType->setCurrentIndex(0);
    ui->recordsStackedWidget->setCurrentIndex(0);
  }
  else
  {
    switch (record->getType())
    {
    case FiscalMemoryRecord::RT_DAILY_REPORT:
    {
      DailyReportRecord *dailyReport = static_cast<DailyReportRecord *>(record);

      ui->recordDailyReportDate->setText(QString("%1-%2-%3 %4:%5:%6").arg(dailyReport->day).arg(dailyReport->month).arg(dailyReport->year)
                                         .arg(dailyReport->hour).arg(dailyReport->minute).arg(dailyReport->second));

      ui->recordDailyReportReceipts->setText(QString::number(dailyReport->receipts));
      ui->recordDailyReportCancelledReceipts->setText(QString::number(dailyReport->cancelledReceipts));
      ui->recordDailyReportDatabaseChanges->setText(QString::number(dailyReport->databaseChanges));

      ui->recordDailyReportCancelledReceiptsValue->setText(QString::number(dailyReport->cancelledReceiptsValue, 'f', 2));

      ui->recordDailyReportTotA->setText(QString::number(dailyReport->totA, 'f', 2));
      ui->recordDailyReportTotB->setText(QString::number(dailyReport->totB, 'f', 2));
      ui->recordDailyReportTotC->setText(QString::number(dailyReport->totC, 'f', 2));
      ui->recordDailyReportTotD->setText(QString::number(dailyReport->totD, 'f', 2));
      ui->recordDailyReportTotE->setText(QString::number(dailyReport->totE, 'f', 2));
      ui->recordDailyReportTotF->setText(QString::number(dailyReport->totF, 'f', 2));
      ui->recordDailyReportTotG->setText(QString::number(dailyReport->totG, 'f', 2));

      ui->recordType->setCurrentIndex(1);
      ui->recordsStackedWidget->setCurrentIndex(1);

      break;
    }

    case FiscalMemoryRecord::RT_VAT_CHANGE_REPORT:
    {
      VatChangeRecord *vatChange = static_cast<VatChangeRecord *>(record);

      ui->recordVatChangeDate->setText(QString("%1-%2-%3 %4:%5:%6").arg(vatChange->day).arg(vatChange->month).arg(vatChange->year)
                                       .arg(vatChange->hour).arg(vatChange->minute).arg(vatChange->second));

      ui->recordVatChangeVatA->setText(QString::number(vatChange->vatA, 'f', 2));
      ui->recordVatChangeVatB->setText(QString::number(vatChange->vatB, 'f', 2));
      ui->recordVatChangeVatC->setText(QString::number(vatChange->vatC, 'f', 2));
      ui->recordVatChangeVatD->setText(QString::number(vatChange->vatD, 'f', 2));
      ui->recordVatChangeVatE->setText(QString::number(vatChange->vatE, 'f', 2));
      ui->recordVatChangeVatF->setText(QString::number(vatChange->vatF, 'f', 2));
      ui->recordVatChangeVatG->setText(QString::number(vatChange->vatG, 'f', 2));

      ui->recordType->setCurrentIndex(2);
      ui->recordsStackedWidget->setCurrentIndex(2);

      break;
    }

    case FiscalMemoryRecord::RT_RAM_RESET_REPORT:
    {
      RamResetRecord *ramReset = static_cast<RamResetRecord *>(record);

      ui->recordRamResetDate->setText(QString("%1-%2-%3 %4:%5:%6").arg(ramReset->day).arg(ramReset->month).arg(ramReset->year)
                                      .arg(ramReset->hour).arg(ramReset->minute).arg(ramReset->second));

      ui->recordRamResetReason->setText(QString::number(ramReset->reason));
      ui->recordRamResetNr->setText(QString::number(ramReset->resetNumber));

      ui->recordType->setCurrentIndex(3);
      ui->recordsStackedWidget->setCurrentIndex(3);

      break;
    }

    case FiscalMemoryRecord::RT_SELL_AFTER_RAM_RESET:
    {
      SellAfterRamResetRecord *sellAfterReset = static_cast<SellAfterRamResetRecord *>(record);

      ui->recordSellAfterRamResetDate->setText(QString("%1-%2-%3 %4:%5:%6").arg(sellAfterReset->day).arg(sellAfterReset->month).arg(sellAfterReset->year)
                                               .arg(sellAfterReset->hour).arg(sellAfterReset->minute).arg(sellAfterReset->second));

      ui->recordType->setCurrentIndex(4);
      ui->recordsStackedWidget->setCurrentIndex(4);

      break;
    }

    case FiscalMemoryRecord::RT_EMPTY:
    default:
      ui->recordType->setCurrentIndex(0);
      ui->recordsStackedWidget->setCurrentIndex(0);
      break;
    }

    delete record;
    record = NULL;
  }
}


void MainWindow::on_setClockButton_clicked()
{
  Id id(ui->setClockPrinterId->text().toUtf8().data(), ui->setClockOperatorId->text().toUtf8().data());

  printer->setClock(id, ui->setClockYear->value(), ui->setClockMonth->value(),
    ui->setClockDay->value(), ui->setClockHour->value(), ui->setClockMinute->value(),
    ui->setClockSecond->value());
}


void MainWindow::on_getClockButton_clicked()
{
  ClockInfo info = printer->getClock();

  ui->getClockDate->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));
  ui->getClockTime->setText(QString("%1:%2").arg(info.hour).arg(info.minute));
}


void MainWindow::on_setVatRatesButton_clicked()
{
  Id id(ui->setVatRatesPrinterId->text().toUtf8().data(), ui->setVatRatesOperatorId->text().toUtf8().data());

  printer->setVatRates(id, ui->setVatRatesCount->value(), ui->setVatRatesVatA->value(),
    ui->setVatRatesVatB->value(), ui->setVatRatesVatC->value(), ui->setVatRatesVatD->value(),
    ui->setVatRatesVatE->value(), ui->setVatRatesVatF->value(), ui->setVatRatesVatG->value());
}


void MainWindow::on_setHeaderButton_clicked()
{
  Id id(ui->setHeaderPrinterId->text().toUtf8().data(), ui->setHeaderOperatorId->text().toUtf8().data());

  printer->setHeader(id, ui->setHeaderContent->text().toUtf8().data());
}


void MainWindow::on_getHeaderButton_clicked()
{
  ui->getHeaderContent->setText(QString::fromUtf8(printer->getHeader().c_str()));
}


void MainWindow::on_openDrawerButton_clicked()
{
  printer->openDrawer();
}


void MainWindow::on_setDisplayMessageButton_clicked()
{
  printer->setDisplayMessage(ui->displayMessage->toPlainText().toUtf8().data());
}


void MainWindow::on_setDisplayModeButton_clicked()
{
  printer->setDisplayMode((DISPLAY_MODE) ui->displayMode->currentText().toInt());
}


void MainWindow::on_setDiscountAlgorithmButton_clicked()
{
  printer->setDiscountAlgorithm((DISCOUNT_ALGORITHM) ui->discountAlgorithm->currentText().toInt());
}


void MainWindow::on_getServiceCheckDateButton_clicked()
{
  ServiceDate info = printer->getServiceCheckDate();

  ui->serviceCheckDate->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));
  ui->serviceCheckMessage->setText(QString::fromStdString(info.message));
}


void MainWindow::on_getServiceLockDateButton_clicked()
{
  ServiceDate info = printer->getServiceLockDate();

  ui->serviceLockDate->setText(QString("%1-%2-%3").arg(info.day).arg(info.month).arg(info.year));
  ui->serviceLockMessage->setText(QString::fromStdString(info.message));
}


void MainWindow::on_beginTransactionButton_clicked()
{
  ExtraLines extraLines;
  extraLines.line1 = ui->transactionExtraLine1->text().toUtf8().data();
  extraLines.line2 = ui->transactionExtraLine2->text().toUtf8().data();
  extraLines.line3 = ui->transactionExtraLine3->text().toUtf8().data();

  printer->beginTransaction(ui->transactionItems->value(), extraLines,
    (CLIENT_ID_TYPE)ui->transactionClientIdType->currentText().toInt(),
    ui->transactionClientId->text().toUtf8().data());
}


void MainWindow::on_printReceiptLineButton_clicked()
{
  Item item;

  item.line = ui->itemLine->value();
  item.name = ui->itemName->text().toUtf8().data();
  item.barcode = ui->itemBarcode->text().toUtf8().data();
  item.description = ui->itemDescription->text().toUtf8().data();
  item.vat = ui->itemVat->currentText().toUtf8().data();
  item.quantity = QString::number(ui->itemQuantity->value()).toUtf8().data();
  item.price = ui->itemPrice->value();
  item.gross = ui->itemGross->value();
  item.discountType = (ITEM_DISCOUNT_TYPE)ui->itemDiscountType->currentText().toInt();
  item.discountDesc = (DISCOUNT_DESCRIPTION_TYPE)ui->itemDiscountDesc->currentText().toInt();
  item.discountValue = ui->itemDiscountValue->value();
  item.discountName = ui->itemDiscountName->text().toUtf8().data();

  printer->printReceiptLine(item);
}


void MainWindow::on_printDepositLineButton_clicked()
{
  printer->printDepositLine((DEPOSIT_TYPE)ui->depositType->currentText().toInt(),
    ui->depositNr->text().toUtf8().data(), QString::number(ui->depositQuantity->value()).toUtf8().data(),
    ui->depositPrice->value());
}


void MainWindow::on_depositCollectedButton_clicked()
{
  printer->depositCollected(ui->depositCollectedAmount->value(), ui->depositCollectedNr->value(),
    QString::number(ui->depositCollectedQuantity->value()).toUtf8().data());
}


void MainWindow::on_correctDepositCollectedButton_clicked()
{
  printer->correctDepositCollected(ui->correctDepositCollectedAmount->value(),
    ui->correctDepositCollectedNr->value(), QString::number(ui->correctDepositCollectedQuantity->value()).toUtf8().data());
}


void MainWindow::on_depositReturnedButton_clicked()
{
  printer->depositReturned(ui->depositReturnedAmount->value(), ui->depositReturnedNr->value(),
    QString::number(ui->depositReturnedQuantity->value()).toUtf8().data());
}


void MainWindow::on_correctDepositReturnedButton_clicked()
{
  printer->correctDepositReturned(ui->correctDepositReturnedAmount->value(),
    ui->correctDepositReturnedNr->value(), QString::number(ui->correctDepositReturnedQuantity->value()).toUtf8().data());
}


void MainWindow::on_cancelTransactionButton_clicked()
{
  Id id(ui->cancelTransactionPrinterId->text().toUtf8().data(), ui->cancelTransactionOperatorId->text().toUtf8().data());

  printer->cancelTransaction(id);
}


void MainWindow::on_confirmTransactionButton_clicked()
{
  Id id(ui->confirmTransactionPrinterId->text().toUtf8().data(), ui->confirmTransactionOperatorId->text().toUtf8().data());

  ExtraLines extraLines;
  extraLines.line1 = ui->confirmTransactionExtraLine1->text().toUtf8().data();
  extraLines.line2 = ui->confirmTransactionExtraLine2->text().toUtf8().data();
  extraLines.line3 = ui->confirmTransactionExtraLine3->text().toUtf8().data();

  printer->confirmTransaction(id, ui->confirmTransactionCashIn->value(),
    ui->confirmTransactionTotal->value(),
    (TRANSACTION_DISCOUNT_TYPE)ui->confirmTransactionDiscountType->currentText().toInt(),
    ui->confirmTransactionDiscountValue->value(), extraLines);
}


void MainWindow::on_confirmTransactionWithPaymentForms1Button_clicked()
{
  Id id(ui->confirmPF1PrinterId->text().toUtf8().data(), ui->confirmPF1OperatorId->text().toUtf8().data());

  ExtraLines extraLines;
  extraLines.line1 = ui->confirmPF1ExtraLine1->text().toUtf8().data();
  extraLines.line2 = ui->confirmPF1ExtraLine2->text().toUtf8().data();
  extraLines.line3 = ui->confirmPF1ExtraLine3->text().toUtf8().data();

  PaymentFormsInfo1 info;

  info.cashFlag = (bool)ui->confirmPF1CashFlag->currentIndex();
  info.cardFlag = (bool)ui->confirmPF1CardFlag->currentIndex();
  info.chequeFlag = (bool)ui->confirmPF1ChequeFlag->currentIndex();
  info.couponFlag = (bool)ui->confirmPF1CouponFlag->currentIndex();
  info.depositCollectedFlag = (bool)ui->confirmPF1DepositCollectedFlag->currentIndex();
  info.depositReturnedFlag = (bool)ui->confirmPF1DepositReturnedFlag->currentIndex();
  info.changeFlag = (bool)ui->confirmPF1ChangeFlag->currentIndex();

  info.cashIn = ui->confirmPF1CashIn->value();
  info.cardIn = ui->confirmPF1CardIn->value();
  info.chequeIn = ui->confirmPF1ChequeIn->value();
  info.couponIn = ui->confirmPF1CouponIn->value();
  info.depositCollected = ui->confirmPF1DepositCollected->value();
  info.depositReturned = ui->confirmPF1DepositReturned->value();
  info.checkOut = ui->confirmPF1CheckOut->value();

  info.cardName = ui->confirmPF1CardName->text().toUtf8().data();
  info.chequeName = ui->confirmPF1ChequeName->text().toUtf8().data();
  info.couponName = ui->confirmPF1CouponName->text().toUtf8().data();

  printer->confirmTransactionWithPaymentForms1(id, info, ui->confirmPF1Total->value(),
    (TRANSACTION_DISCOUNT_TYPE)ui->confirmPF1DiscountType->currentText().toInt(),
    ui->confirmPF1DiscountValue->value(), extraLines);
}


void MainWindow::on_confirmTransactionWithPaymentForms2Button_clicked()
{
  Id id(ui->confirmPF2PrinterId->text().toUtf8().data(), ui->confirmPF2OperatorId->text().toUtf8().data());

  PaymentFormsInfo2 info;

  info.cashFlag = (bool)ui->confirmPF2CashFlag->currentIndex();
  info.changeFlag = (bool)ui->confirmPF2ChangeOutFlag->currentIndex();

  PaymentForm pf;

  pf.type = (PAYMENT_TYPE)ui->confirmPF2Type->currentText().toInt();
  pf.name = ui->confirmPF2Name->text().toUtf8().data();
  pf.amount = ui->confirmPF2Amount->value();;

  info.paymentForms.push_back(pf);

  printer->confirmTransactionWithPaymentForms2(id, info, ui->confirmPF2Total->value(), DT_0, 0.0,
    ui->confirmPF2SysNr->text().toUtf8().data(), (bool)ui->confirmPF2Summary->currentIndex(), ExtraLines::none());
}


void MainWindow::on_paymentFormServiceButton_clicked()
{
  printer->paymentFormService((SERVICE_TYPE)ui->paymentFormServiceType->currentText().toInt(),
    (PAYMENT_TYPE)ui->paymentFormServicePaymentType->currentText().toInt(),
    ui->paymentFormServiceAmount->value(), ui->paymentFormServiceName->text().toUtf8().data());
}


void MainWindow::on_addDiscountButton_clicked()
{
  printer->addDiscount((DISCOUNT_TYPE)ui->addDiscountType->currentText().toInt(),
    ui->addDiscountName->text().toUtf8().data(), ui->addDiscountValue->value());
}


void MainWindow::on_addVatDiscountButton_clicked()
{
  printer->addVatRateDiscount(ui->addVatDiscountVatRate->currentIndex(),
    (DISCOUNT_TYPE)ui->addVatDiscountType->currentText().toInt(),
    (DISCOUNT_DESCRIPTION_TYPE)ui->addVatDiscountDesc->currentText().toInt(),
    ui->addVatDiscountAmount->value(), ui->addVatDiscountValue->value(),
    ui->addVatDiscountName->text().toUtf8().data());
}


void MainWindow::on_addSubtotalDiscountButton_clicked()
{
  printer->addSubtotalDiscount((DISCOUNT_TYPE)ui->subtotalDiscountType->currentText().toInt(),
    (DISCOUNT_DESCRIPTION_TYPE)ui->subtotalDiscountDesc->currentText().toInt(),
    ui->subtotalDiscountSubtotal->value(), ui->subtotalDiscountValue->value(),
    ui->subtotalDiscountName->text().toUtf8().data());
}


void MainWindow::on_extraLineContainerReturnedButton_clicked()
{
  printer->extraLineContainerReturned(QString::number(ui->containerReturnedName->value()).toUtf8().data(),
    ui->containerReturnedQuantity->text().toUtf8().data(), ui->containerReturnedAmount->value());
}


void MainWindow::on_extraLineContainerReceivedButton_clicked()
{
  printer->extraLineContainerReceived(QString::number(ui->containerReceivedName->value()).toUtf8().data(),
    ui->containerReceivedQuantity->text().toUtf8().data(), ui->containerReceivedAmount->value());
}


void MainWindow::on_formsOfPaymentClearingButton_clicked()
{
  printer->formsOfPaymentClearing();
}


void MainWindow::on_extraLineButton_clicked()
{
  printer->extraLine((EXTRA_LINE_TYPE)ui->extraLineType->currentText().toInt(),
    ui->extraLineText->text().toUtf8().data());
}


void MainWindow::on_defineInfoLinesButton_clicked()
{
  ExtraLines extraLines;

  extraLines.line1 = ui->infoLine1->text().toUtf8().data();
  extraLines.line2 = ui->infoLine2->text().toUtf8().data();
  extraLines.line3 = ui->infoLine3->text().toUtf8().data();

  printer->defineInfoLines(extraLines);
}


void MainWindow::on_euroPaymentButton_clicked()
{
  printer->euroPayment(ui->euroExchange->value(), ui->euroAmount->value(), ui->euroCashIn->value(),
    ui->euroCheckEuro->value(), ui->euroCheckPln->value());
}


void MainWindow::on_finishButton_clicked()
{
  printer->finish((bool)ui->finishNextHeader->currentIndex());
}


void MainWindow::on_beginInvoiceButton_clicked()
{
  BeginInvoiceData data;

  data.items = ui->beginInvoiceItems->value();

  data.printCopy = (bool)ui->beginInvoicePrintCopy->currentIndex();
  data.topMargin = (bool)ui->beginInvoiceTopMargin->currentIndex();
  data.signature = (bool)ui->beginInvoiceSignature->currentIndex();

  data.additionalCopies = ui->beginInvoiceAdditionalCopies->value();

  data.invoiceNr = ui->beginInvoiceNr->text().toUtf8().data();

  data.nip = ui->beginInvoiceNip->text().toUtf8().data();

  data.timeout = ui->beginInvoiceTimeout->text().toUtf8().data();

  data.paymentForm = ui->beginInvoicePaymentFormName->text().toUtf8().data();

  data.client = ui->beginInvoiceClientName->text().toUtf8().data();
  data.seller = ui->beginInvoiceSellerName->text().toUtf8().data();

  data.systemNr = ui->beginInvoiceSysNr->text().toUtf8().data();

  if (!ui->beginInvoiceClientLine1->text().isEmpty()) data.clientLines.push_back(ui->beginInvoiceClientLine1->text().toUtf8().data());
  if (!ui->beginInvoiceClientLine2->text().isEmpty()) data.clientLines.push_back(ui->beginInvoiceClientLine2->text().toUtf8().data());
  if (!ui->beginInvoiceClientLine3->text().isEmpty()) data.clientLines.push_back(ui->beginInvoiceClientLine3->text().toUtf8().data());

  printer->beginInvoice(data);
}


void MainWindow::on_finishInvoiceButton_clicked()
{
  Id id(ui->finishInvoicePrinterId->text().toUtf8().data(), ui->finishInvoiceOperatorId->text().toUtf8().data());

  FinishInvoiceData data;

  data.payedFlag = (bool)ui->finishInvoicePayedFlag->currentIndex();
  data.payed = ui->finishInvoicePayed->text().toUtf8().data();

  data.client = (CLIENT_SELLER_OPTION)ui->finishInvoiceCustomer->currentText().toInt();
  data.seller = (CLIENT_SELLER_OPTION)ui->finishInvoiceSeller->currentText().toInt();

  data.cashIn = ui->finishInvoiceCashIn->value();
  data.total = ui->finishInvoiceTotal->value();

  data.discountValue = ui->finishInvoiceDiscountValue->value();

  data.clientName = ui->finishInvoiceCustomerName->text().toUtf8().data();
  data.sellerName = ui->finishInvoiceSellerName->text().toUtf8().data();

  data.extraLines.line1 = ui->finishInvoiceExtraLine1->text().toUtf8().data();
  data.extraLines.line2 = ui->finishInvoiceExtraLine2->text().toUtf8().data();
  data.extraLines.line3 = ui->finishInvoiceExtraLine3->text().toUtf8().data();

  printer->finishInvoice(id, data);
}


void MainWindow::on_setInvoiceOptionButton_clicked()
{
  InvoiceOptions data;

  data.additionalCopies = ui->invoiceOptionsAdditionalCopies->value();

  data.client = (CLIENT_SELLER_OPTION)ui->invoiceOptionsCustomer->currentText().toInt();
  data.seller = (CLIENT_SELLER_OPTION)ui->invoiceOptionsSeller->currentText().toInt();

  data.payedFlag = (bool)ui->invoiceOptionsPayedFlag->currentIndex();
  data.payed = ui->invoiceOptionsPayed->text().toUtf8().data();

  data.year = ui->invoiceOptionsYear->value();
  data.month = ui->invoiceOptionsMonth->value();
  data.day = ui->invoiceOptionsDay->value();

  if (ui->invoiceOptionsISO01->isChecked()) data.summaryOption |= ISO_01;
  if (ui->invoiceOptionsISO02->isChecked()) data.summaryOption |= ISO_02;
  if (ui->invoiceOptionsISO04->isChecked()) data.summaryOption |= ISO_04;
  if (ui->invoiceOptionsISO08->isChecked()) data.summaryOption |= ISO_08;
  if (ui->invoiceOptionsISO10->isChecked()) data.summaryOption |= ISO_10;
  if (ui->invoiceOptionsISO20->isChecked()) data.summaryOption |= ISO_20;

  if (ui->invoiceOptionsIO201->isChecked()) data.invoiceOptions2 |= IO2_01;
  if (ui->invoiceOptionsIO202->isChecked()) data.invoiceOptions2 |= IO2_02;
  if (ui->invoiceOptionsIO204->isChecked()) data.invoiceOptions2 |= IO2_04;
  if (ui->invoiceOptionsIO208->isChecked()) data.invoiceOptions2 |= IO2_08;
  if (ui->invoiceOptionsIO210->isChecked()) data.invoiceOptions2 |= IO2_10;
  if (ui->invoiceOptionsIO220->isChecked()) data.invoiceOptions2 |= IO2_20;
  if (ui->invoiceOptionsIO240->isChecked()) data.invoiceOptions2 |= IO2_40;
  if (ui->invoiceOptionsIO280->isChecked()) data.invoiceOptions2 |= IO2_80;

  if (ui->invoiceOptionsIO301->isChecked()) data.invoiceOptions3 |= IO3_01;
  if (ui->invoiceOptionsIO302->isChecked()) data.invoiceOptions3 |= IO3_02;
  if (ui->invoiceOptionsIO304->isChecked()) data.invoiceOptions3 |= IO3_04;
  if (ui->invoiceOptionsIO308->isChecked()) data.invoiceOptions3 |= IO3_08;
  if (ui->invoiceOptionsIO310->isChecked()) data.invoiceOptions3 |= IO3_10;

  data.clientIdType = (CLIENT_ID_TYPE)ui->invoiceOptionsCustomerIdType->currentText().toInt();

  data.timeout = ui->invoiceOptionsTimeout->text().toUtf8().data();

  data.paymentForm = ui->invoiceOptionsPaymentForm->text().toUtf8().data();

  data.clientName = ui->invoiceOptionsClientName->text().toUtf8().data();
  data.sellerName = ui->invoiceOptionsSellerName->text().toUtf8().data();

  data.systemNr = ui->invoiceOptionsSysNr->text().toUtf8().data();

  printer->setInvoiceOption(data);
}


void MainWindow::on_extraLinesInvoiceButton_clicked()
{
  printer->extraLinesInvoice((EXTRA_LINE_TYPE)ui->extraLineInvoiceType->currentText().toInt(),
    ui->extraLineInvoiceText->text().toUtf8().data());
}


void MainWindow::on_loginButton_clicked()
{
  Id id(ui->loginPrinterId->text().toUtf8().data(), ui->loginOperatorId->text().toUtf8().data());

  printer->login(id);
}


void MainWindow::on_logoutButton_clicked()
{
  Id id(ui->logoutPrinterId->text().toUtf8().data(), ui->logoutOperatorId->text().toUtf8().data());

  printer->logout(id);
}


void MainWindow::on_paymentToCashButton_clicked()
{
  Id id(ui->toCashPrinterId->text().toUtf8().data(), ui->toCashOperatorId->text().toUtf8().data());

  printer->paymentToCash(id, ui->toCashAmount->value(), (bool)ui->toCashEuro->currentIndex());
}


void MainWindow::on_paymentFromCashButton_clicked()
{
  Id id(ui->fromCashPrinterId->text().toUtf8().data(), ui->fromCashOperatorId->text().toUtf8().data());

  printer->withdrawalFromCash(id, ui->fromCashAmount->value(), (bool)ui->fromCashEuro->currentIndex());
}


void MainWindow::on_printCashStateButton_clicked()
{
  Id id(ui->printCashStatePrinterId->text().toUtf8().data(), ui->printCashStateOperatorId->text().toUtf8().data());

  printer->printCashState(id);
}


void MainWindow::on_printShiftReportButton_clicked()
{
  Id id(ui->shiftReportPrinterId->text().toUtf8().data(), ui->shiftReportOperatorId->text().toUtf8().data());

  printer->printShiftReport(id, (bool)ui->shiftReportReset->currentIndex(), ui->shiftReportName->text().toUtf8().data());
}


void MainWindow::on_printDailyReportButton_clicked()
{
  Id id(ui->dailyReportPrinterId->text().toUtf8().data(), ui->dailyReportOperatorId->text().toUtf8().data());

  printer->printDailyReport(id);
}


void MainWindow::on_printPeriodicalReportByDateButton_clicked()
{
  Id id(ui->periodicalByDatePrinterId->text().toUtf8().data(), ui->periodicalByDateOperatorId->text().toUtf8().data());

  printer->printPeriodicalReportByDate(id, ui->periodicalByDateFromYear->value(),
    ui->periodicalByDateFromMonth->value(), ui->periodicalByDateFromDay->value(),
    ui->periodicalByDateToYear->value(), ui->periodicalByDateToMonth->value(),
    ui->periodicalByDateToDay->value(), (PERIODICAL_REPORT_TYPE)ui->periodicalByDateType->currentText().toInt());
}


void MainWindow::on_printPeriodicalReportByNumberButton_clicked()
{
  Id id(ui->periodicalByNumberPrinterId->text().toUtf8().data(), ui->periodicalByNumberOperatorId->text().toUtf8().data());

  printer->printPeriodicalReportByNumber(id, ui->periodicalByNumberFrom->value(),
    ui->periodicalByNumberTo->value(), (PERIODICAL_REPORT_TYPE)ui->periodicalByNumberType->currentText().toInt());
}


void MainWindow::on_containerReturnButton_clicked()
{
  printer->containerReturn(ui->containerReturnNr->text().toUtf8().data());
}


void MainWindow::on_saleReceiptButton_clicked()
{
  Id id(ui->saleReceiptPrinterId->text().toUtf8().data(), ui->saleReceiptOperatorId->text().toUtf8().data());

  SaleReceiptData data;

  data.printId = (bool)ui->saleReceiptPrintId->currentIndex();

  data.printOption = (SALE_RECEIPT_OPTION)ui->saleReceiptPrintOptions->currentText().toInt();

  data.month = ui->saleReceiptMonth->value();
  data.year = ui->saleReceiptYear->value();

  data.amount = ui->saleReceiptAmount->value();

  data.receipt = ui->saleReceiptReceipt->text().toUtf8().data();

  data.clientName = ui->saleReceiptClient->text().toUtf8().data();

  data.terminal = ui->saleReceiptTerminal->text().toUtf8().data();

  data.cardName = ui->saleReceiptCardName->text().toUtf8().data();
  data.cardNr = ui->saleReceiptCardNr->text().toUtf8().data();

  data.authCode = ui->saleReceiptCode->text().toUtf8().data();

  printer->saleReceipt(id, data);
}


void MainWindow::on_returnOfArticleButton_clicked()
{
  Id id(ui->returnOfArticlePrinterId->text().toUtf8().data(), ui->returnOfArticleOperatorId->text().toUtf8().data());

  SaleReceiptData data;

  data.printId = (bool)ui->returnOfArticlePrintId->currentIndex();

  data.printOption = (SALE_RECEIPT_OPTION)ui->returnOfArticlePrintOptions->currentText().toInt();

  data.month = ui->returnOfArticleMonth->value();
  data.year = ui->returnOfArticleYear->value();

  data.amount = ui->returnOfArticleAmount->value();

  data.receipt = ui->returnOfArticleReceipt->text().toUtf8().data();

  data.clientName = ui->returnOfArticleClient->text().toUtf8().data();

  data.terminal = ui->returnOfArticleTerminal->text().toUtf8().data();

  data.cardName = ui->returnOfArticleCardName->text().toUtf8().data();
  data.cardNr = ui->returnOfArticleCardNr->text().toUtf8().data();

  data.authCode = ui->returnOfArticleCode->text().toUtf8().data();

  printer->returnOfArticle(id, data);
}


void MainWindow::on_beginNonFiscalButton_clicked()
{
  printer->beginNonFiscal(ui->beginNonFiscalPrintNr->value(), ui->beginNonFiscalHeaderNr->value());
}


void MainWindow::on_printNonFiscalButton_clicked()
{
  NonFiscalLine line;

  line.printNr = ui->nonFiscalPrintNr->value();

  line.lineNr = ui->nonFiscalLineNr->value();

  line.bold = (bool)ui->nonFiscalBold->currentIndex();
  line.inverse = (bool)ui->nonFiscalInverse->currentIndex();
  line.center = (bool)ui->nonFiscalCenter->currentIndex();

  line.font = ui->nonFiscalFont->currentText().toInt();

  line.attrs = (FONT_ATTRS)ui->nonFiscalAttrs->currentText().toInt();

  QStringList list = ui->nonFiscalLines->toPlainText().split("\n");
  for (int i = 0; i < list.size(); ++i)
  {
    line.lines.push_back(list[i].toUtf8().data());
  }

  printer->printNonFiscal(line);
}


void MainWindow::on_finishNonFiscalButton_clicked()
{
  ExtraLines extraLines;

  extraLines.line1 = ui->finishNonFiscalExtraLine1->text().toUtf8().data();
  extraLines.line2 = ui->finishNonFiscalExtraLine2->text().toUtf8().data();
  extraLines.line3 = ui->finishNonFiscalExtraLine3->text().toUtf8().data();

  printer->finishNonFiscal(ui->finishNonFiscalNr->value(),
    ui->finishNonFiscalSysNr->text().toUtf8().data(), extraLines);
}


void MainWindow::on_setCustomerIdButton_clicked()
{
  printer->setClientId((CLIENT_ID_TYPE)ui->setCustomerIdType->currentText().toInt(),
    ui->setCustomerIdName->text().toUtf8().data());
}


void MainWindow::on_feedButton_clicked()
{
  printer->paperFeed(ui->feedLines->value());
}


void MainWindow::on_debugGenerateErrorButton_clicked()
{
  printer->debugGenerateError();
}


void MainWindow::on_listView_clicked(const QModelIndex &index)
{
  ui->stackedWidget->setCurrentIndex(proxy->mapToSource(index).row());
}


void MainWindow::on_filterLineEdit_textChanged(const QString &arg1)
{
  proxy->setFilterWildcard(arg1);
}
