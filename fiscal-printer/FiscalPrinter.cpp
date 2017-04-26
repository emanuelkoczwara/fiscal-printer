/*
 * Copyright (c) 2014 Emanuel Koczwara Software <emanuel.koczwara@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This library and documentation is based on resources provided
 * by NOVITUS (http://www.novitus.pl/). Protocol description
 * can be downloaded from website for free.
 *
 */


#include <fiscal-printer/FiscalPrinter.hpp>


using namespace fp;
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::spirit::classic;


FiscalPrinter::FiscalPrinter() : io(NULL), port(NULL)
{
}


FiscalPrinter::~FiscalPrinter()
{
  close();
}


void FiscalPrinter::open(const string &device, int baudRate)
{
  if (io == NULL && port == NULL)
  {
    io = new io_service();

    port = new serial_port(*io, device);

    port->set_option<>(serial_port::baud_rate(baudRate));
    port->set_option<>(serial_port::parity(serial_port::parity::none));
    port->set_option<>(serial_port::stop_bits(serial_port::stop_bits::one));
    port->set_option<>(serial_port::flow_control(serial_port::flow_control::software));
    port->set_option<>(serial_port::character_size(8));
  }
}


void FiscalPrinter::close()
{
  if (port != NULL)
  {
    if (port->is_open())
    {
      port->close();
    }
  }

  delete port;
  port = NULL;

  delete io;
  io = NULL;
}


void FiscalPrinter::bell()
{
  writeOneByte('\a');
}


void FiscalPrinter::cancel()
{
  writeOneByte('\x18');
}


EnqStatus FiscalPrinter::getEnqStatus()
{
  writeOneByte('\x05');

  EnqStatus status;

  char result = 0;

  while (true)
  {
    result = readOneByte();

    if (result >= 0x60 && result <= 0x6F)
    {
      status.fiscal        = result & (char)0x08;
      status.command       = result & (char)0x04;
      status.transaction   = result & (char)0x02;
      status.transactionOk = result & (char)0x01;

      break;
    }
  }

  return status;
}


DleStatus FiscalPrinter::getDleStatus()
{
  writeOneByte('\x10');

  DleStatus status;

  char result = 0;

  while (true)
  {
    result = readOneByte();

    if (result >= 0x70 && result <= 0x77)
    {
      status.online = result & (char)0x04;
      status.paper  = result & (char)0x02;
      status.error  = result & (char)0x01;

      break;
    }
  }

  return status;
}


void FiscalPrinter::setErrorHandlingMode(ERROR_HANDLING_MODE mode)
{
  vector<int> intParams;

  intParams.push_back((int)mode);

  execute(intParams, "#e", true);
}


PrinterError FiscalPrinter::getLastError()
{
  vector<int> intParams;

  intParams.push_back(0); // parametr ignorowany

  execute(intParams, "#n", false);

  int code = 0;

  parse(read().c_str(),

    str_p("1#E") >> int_p[assign_a(code)]

    );

  return PrinterError(code);
}


CashRegisterInfo1 FiscalPrinter::getCashRegisterInfo1()
{
  vector<int> intParams;

  intParams.push_back(0);

  execute(intParams, "#s", false);

  string result = read();

  CashRegisterInfo1 info;

  vector<float> vat;
  vector<float> tot;

  int len;

  len = parse(result.c_str(),

    str_p("1#X")                        >>

    int_p[assign_a(info.lastError)]     >> ch_p(';') >>

    int_p[assign_a(info.fiscal)]        >> ch_p(';') >>
    int_p[assign_a(info.transaction)]   >> ch_p(';') >>
    int_p[assign_a(info.transactionOk)] >> ch_p(';') >>

    int_p                               >> ch_p(';') >>

    int_p[assign_a(info.ramResets)]     >> ch_p(';') >>

    int_p[assign_a(info.year)]          >> ch_p(';') >>
    int_p[assign_a(info.month)]         >> ch_p(';') >>
    int_p[assign_a(info.day)]           >> ch_p('/')

    ).length;

  result = result.substr(len);

  len = parse(result.c_str(),

    +(strict_real_p[push_back_a(vat)] >> ch_p('/')) >>
    int_p[assign_a(info.reciepts)]    >> ch_p('/')

    ).length;

  result = result.substr(len);

  len = parse(result.c_str(),

    repeat_p(vat.size() + 1)[strict_real_p[push_back_a(tot)] >> ch_p('/')]

    ).length;

  result = result.substr(len);

  len = parse(result.c_str(),

    strict_real_p[assign_a(info.cash)] >> ch_p('/') >>
    (*print_p)[assign_a(info.number)]

    ).length;

  if (vat.size() > 0) info.vatA = vat[0];
  if (vat.size() > 1) info.vatB = vat[1];
  if (vat.size() > 2) info.vatC = vat[2];
  if (vat.size() > 3) info.vatD = vat[3];
  if (vat.size() > 4) info.vatE = vat[4];
  if (vat.size() > 5) info.vatF = vat[5];

  if (tot.size() > 0) info.totA = tot[0];
  if (tot.size() > 1) info.totB = tot[1];
  if (tot.size() > 2) info.totC = tot[2];
  if (tot.size() > 3) info.totD = tot[3];
  if (tot.size() > 4) info.totE = tot[4];
  if (tot.size() > 5) info.totF = tot[5];
  if (tot.size() > 6) info.totG = tot[6];

  return info;
}


CashRegisterInfo2 FiscalPrinter::getCashRegisterInfo2(CASH_REGISTER_INFO_2_MODE mode, bool invoices)
{
  vector<int> intParams;

  intParams.push_back(243);
  intParams.push_back((int)invoices);

  execute(intParams, "$r", true);

  intParams.clear();

  intParams.push_back((int)mode);

  execute(intParams, "#s", false);

  string result = read();

  CashRegisterInfo2 info;

  parse(result.c_str(),

    str_p("2#X")                        >>

    int_p[assign_a(info.lastError)]     >> ch_p(';') >>

    int_p[assign_a(info.fiscal)]        >> ch_p(';') >>
    int_p[assign_a(info.transaction)]   >> ch_p(';') >>
    int_p[assign_a(info.transactionOk)] >> ch_p(';') >>

    int_p                               >> ch_p(';') >>

    int_p[assign_a(info.resets)]        >> ch_p(';') >>

    int_p[assign_a(info.year)]          >> ch_p(';') >>
    int_p[assign_a(info.month)]         >> ch_p(';') >>
    int_p[assign_a(info.day)]           >> ch_p('/') >>

    strict_real_p[assign_a(info.vatA)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatB)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatC)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatD)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatE)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatF)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.vatG)]  >> ch_p('/') >>

    int_p[assign_a(info.reciepts)]      >> ch_p('/') >>

    strict_real_p[assign_a(info.totA)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totB)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totC)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totD)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totE)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totF)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totG)]  >> ch_p('/') >>

    strict_real_p[assign_a(info.cash)]  >> ch_p('/') >>
    (*print_p)[assign_a(info.number)]

    );

  return info;
}


CashRegisterInfo3 FiscalPrinter::getCashRegisterInfo3()
{
  vector<int> intParams;

  intParams.push_back(24);

  execute(intParams, "#s", false);

  string result = read();

  CashRegisterInfo3 info;

  parse(result.c_str(),

    str_p("3#X")                       >>

    int_p[assign_a(info.year)]         >> ch_p(';') >>
    int_p[assign_a(info.month)]        >> ch_p(';') >>
    int_p[assign_a(info.day)]          >> ch_p(';') >>

    int_p[assign_a(info.usedReports)]  >> ch_p(';') >>
    int_p[assign_a(info.freeReports)]  >> ch_p(';') >>
    int_p[assign_a(info.locked)]       >> ch_p(';') >>

    strict_real_p[assign_a(info.totA)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totB)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totC)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totD)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totE)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totF)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totG)] >> ch_p('/')

    );

  return info;
}


CashRegisterInfo4 FiscalPrinter::getCashRegisterInfo4()
{
  vector<int> intParams;

  intParams.push_back(50);

  execute(intParams, "#s", false);

  string result = read();

  CashRegisterInfo4 info;

  parse(result.c_str(),

    str_p("50#X")                   >>

    int_p[assign_a(info.documents)] >> ch_p('/') >>
    int_p[assign_a(info.invoices)]  >> ch_p('/') >>

    int_p                           >> ch_p('/') >>
    int_p                           >> ch_p('/') >>
    int_p                           >> ch_p('/')

    );

  return info;
}


CashRegisterInfo5 FiscalPrinter::getCashRegisterInfo5()
{
  vector<int> intParams;

  intParams.push_back(90);

  execute(intParams, "#s", false);

  string result = read();

  CashRegisterInfo5 info;

  parse(result.c_str(),

    str_p("90#X") >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.label)]     >> ch_p('\r') >>
    (*(anychar_p - ch_p('\r')))[assign_a(info.state)]     >> ch_p('\r') >>
    int_p[assign_a(info.size)]                            >> ch_p('\r') >>
    int_p[assign_a(info.freeMem)]                         >> ch_p('\r') >>
    int_p[assign_a(info.files)]                           >> ch_p('\r') >>
    int_p[assign_a(info.freeReports)]                     >> ch_p('\r') >>
    int_p[assign_a(info.lastReportNr)]                    >> ch_p('\r') >>
    (*(anychar_p - ch_p('\r')))[assign_a(info.lastWrite)] >> ch_p('\r')

    );

  return info;
}


CashRegisterInfo6 FiscalPrinter::getCashRegisterInfo6(CASH_REGISTER_INFO_6_MODE mode)
{
  vector<int> intParams;

  intParams.push_back(100);
  intParams.push_back((int)mode);

  execute(intParams, "#s", true);

  string result = read();

  CashRegisterInfo6 info;

  parse(result.c_str(),

    int_p                               >> ch_p(';') >>
    int_p[assign_a(info.type)]          >> ch_p(';') >>
    int_p[assign_a(info.transaction)]   >>

    str_p("#X")                         >>

    strict_real_p[assign_a(info.total)] >> ch_p('/') >>
    strict_real_p[assign_a(info.totA)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totB)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totC)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totD)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totE)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totF)]  >> ch_p('/') >>
    strict_real_p[assign_a(info.totG)]  >> ch_p('/') >>

    int_p                               >> ch_p('/') >>
    int_p                               >> ch_p('/') >>
    int_p                               >> ch_p('/')

    );

  return info;
}


CashRegisterInfo7 FiscalPrinter::getCashRegisterInfo7(int item, CASH_REGISTER_INFO_7_MODE mode)
{
  vector<int> intParams;

  intParams.push_back(200);
  intParams.push_back((int)mode);
  intParams.push_back(item);

  execute(intParams, "#s", true);

  string result = read();

  CashRegisterInfo7 info;

  parse(result.c_str(),

    int_p                                >>

    str_p("#X")                          >>

    strict_real_p[assign_a(info.amount)] >> ch_p('/')

    );

  return info;
}


VersionInfo FiscalPrinter::getVersionInfo()
{
  execute("#v", false);

  string result = read();

  VersionInfo info;

  parse(result.c_str(),

    str_p("1#R")                                    >>

    (*(anychar_p - ch_p('/')))[assign_a(info.type)] >> ch_p('/') >>
    (*print_p)[assign_a(info.version)]

    );

  return info;
}


DeviceInfo1 FiscalPrinter::getDeviceInfo1()
{
  vector<int> intParams;

  intParams.push_back(0);

  execute(intParams, "$i", false);

  string result = read();

  DeviceInfo1 info;

  parse(result.c_str(),

    int_p                                                          >>

    str_p("$I")                                                    >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.name)]               >> ch_p('\r') >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.softwareVersion)]    >> ch_p('\r') >>
    (*(anychar_p - ch_p('\r')))[assign_a(info.printModuleVersion)] >> ch_p('\r') >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.systemName)]         >> ch_p('\r') >>
    (*(anychar_p - ch_p('\r')))[assign_a(info.systemVer)]          >> ch_p('\r') >>

    int_p[assign_a(info.displays)]                                 >> ch_p('\r') >>

    int_p[assign_a(info.printingWidth)]                            >> ch_p('\r') >>

    int_p[assign_a(info.eCopy)]                                    >> ch_p('\r') >>

    int_p[assign_a(info.fiscalMemorySize)]                         >> ch_p('\r')

    );

  return info;
}


DeviceInfo2 FiscalPrinter::getDeviceInfo2()
{
  vector<int> intParams;

  intParams.push_back(1);

  execute(intParams, "$i", false);

  string result = read();

  DeviceInfo2 info;

  parse(result.c_str(),

    int_p                                                    >>

    str_p("$I")                                              >>

    int_p[assign_a(info.fiscalMemorySize)]                   >> ch_p('\r') >>
    int_p[assign_a(info.recordSize)]                         >> ch_p('\r') >>

    int_p[assign_a(info.mode)]                               >> ch_p('\r') >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.uniqueNumber)] >> ch_p('\r') >>

    (*(anychar_p - ch_p('\r')))[assign_a(info.nip)]          >> ch_p('\r') >>

    int_p[assign_a(info.maxRecordsCount)]                    >> ch_p('\r') >>
    int_p[assign_a(info.recordsCount)]                       >> ch_p('\r') >>

    int_p[assign_a(info.maxDailyReportsCount)]               >> ch_p('\r') >>
    int_p[assign_a(info.dailyReportsCount)]                  >> ch_p('\r') >>

    int_p[assign_a(info.maxRamResetsCount)]                  >> ch_p('\r') >>
    int_p[assign_a(info.ramResetsCount)]                     >> ch_p('\r') >>

    int_p[assign_a(info.maxVatRatesChangesCount)]            >> ch_p('\r') >>
    int_p[assign_a(info.vatRatesChangesCount)]               >> ch_p('\r') >>

    int_p[assign_a(info.maxCurrencyChangesCount)]            >> ch_p('\r') >>
    int_p[assign_a(info.currencyChangesCount)]               >> ch_p('\r')

    );

  return info;
}


void FiscalPrinter::beginFiscalMemoryReadByDate(int year, int month, int day, int hour, int minute, int second)
{
  vector<int> intParams;

  intParams.push_back(25);

  intParams.push_back(year);
  intParams.push_back(month);
  intParams.push_back(day);

  intParams.push_back(hour);
  intParams.push_back(minute);
  intParams.push_back(second);

  execute(intParams, "#s", false);
}


void FiscalPrinter::beginFiscalMemoryReadByRow(long row)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(26);

  stringParams.push_back(fromLong(row) + "/");

  execute(intParams, "#s", stringParams, false);
}


FiscalMemoryRecord *FiscalPrinter::getFiscalMemoryRecord()
{
  vector<int> intParams;

  intParams.push_back(27);

  execute(intParams, "#s", false);

  string result = read();

  string recordTypeStr;

  int len;

  len = parse(result.c_str(),

      str_p("10#X")[assign_a(recordTypeStr)]
    | str_p("11#X")[assign_a(recordTypeStr)]
    | str_p("12#X")[assign_a(recordTypeStr)]
    | str_p("13#X")[assign_a(recordTypeStr)]
    | str_p("25#X")[assign_a(recordTypeStr)]

    ).length;

  result = result.substr(len);

  FiscalMemoryRecord::RECORD_TYPE recordType;

  if      (recordTypeStr == "10#X") recordType = FiscalMemoryRecord::RT_DAILY_REPORT;
  else if (recordTypeStr == "11#X") recordType = FiscalMemoryRecord::RT_VAT_CHANGE_REPORT;
  else if (recordTypeStr == "12#X") recordType = FiscalMemoryRecord::RT_RAM_RESET_REPORT;
  else if (recordTypeStr == "13#X") recordType = FiscalMemoryRecord::RT_SELL_AFTER_RAM_RESET;
  else                              recordType = FiscalMemoryRecord::RT_EMPTY;

  switch (recordType)
  {
  case FiscalMemoryRecord::RT_DAILY_REPORT:
  {
    DailyReportRecord *record = new DailyReportRecord();

    parse(result.c_str(),

      int_p[assign_a(record->year)]                           >> ch_p(';') >>
      int_p[assign_a(record->month)]                          >> ch_p(';') >>
      int_p[assign_a(record->day)]                            >> ch_p(';') >>
      int_p[assign_a(record->hour)]                           >> ch_p(';') >>
      int_p[assign_a(record->minute)]                         >> ch_p(';') >>
      int_p[assign_a(record->second)]                         >> ch_p(';') >>

      int_p[assign_a(record->receipts)]                       >> ch_p(';') >>
      int_p[assign_a(record->cancelledReceipts)]              >> ch_p(';') >>
      int_p[assign_a(record->databaseChanges)]                >> ch_p(';') >>

      strict_real_p[assign_a(record->cancelledReceiptsValue)] >> ch_p('/') >>

      strict_real_p[assign_a(record->totA)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totB)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totC)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totD)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totE)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totF)]                   >> ch_p('/') >>
      strict_real_p[assign_a(record->totG)]                   >> ch_p('/')

    );

    return record;
  }

  case FiscalMemoryRecord::RT_VAT_CHANGE_REPORT:
  {
    VatChangeRecord *record = new VatChangeRecord();

    parse(result.c_str(),

      int_p[assign_a(record->year)]         >> ch_p(';') >>
      int_p[assign_a(record->month)]        >> ch_p(';') >>
      int_p[assign_a(record->day)]          >> ch_p(';') >>
      int_p[assign_a(record->hour)]         >> ch_p(';') >>
      int_p[assign_a(record->minute)]       >> ch_p(';') >>
      int_p[assign_a(record->second)]       >> ch_p(';') >>

      int_p                                 >> ch_p(';') >>
      int_p                                 >> ch_p(';') >>
      int_p                                 >> ch_p(';') >>
      int_p                                 >> ch_p('/') >>

      strict_real_p[assign_a(record->vatA)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatB)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatC)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatD)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatE)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatF)] >> ch_p('/') >>
      strict_real_p[assign_a(record->vatG)] >> ch_p('/')

    );

    return record;
  }

  case FiscalMemoryRecord::RT_RAM_RESET_REPORT:
  {
    RamResetRecord *record = new RamResetRecord();

    parse(result.c_str(),

      int_p[assign_a(record->year)]        >> ch_p(';') >>
      int_p[assign_a(record->month)]       >> ch_p(';') >>
      int_p[assign_a(record->day)]         >> ch_p(';') >>
      int_p[assign_a(record->hour)]        >> ch_p(';') >>
      int_p[assign_a(record->minute)]      >> ch_p(';') >>
      int_p[assign_a(record->second)]      >> ch_p(';') >>

      int_p[assign_a(record->reason)]      >> ch_p(';') >>
      int_p[assign_a(record->resetNumber)] >> ch_p(';') >>
      int_p                                >> ch_p(';') >>
      int_p                                >> ch_p('/') >>

      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/')

    );

    return record;
  }

  case FiscalMemoryRecord::RT_SELL_AFTER_RAM_RESET:
  {
    SellAfterRamResetRecord *record = new SellAfterRamResetRecord();

    parse(result.c_str(),

      int_p[assign_a(record->year)]        >> ch_p(';') >>
      int_p[assign_a(record->month)]       >> ch_p(';') >>
      int_p[assign_a(record->day)]         >> ch_p(';') >>
      int_p[assign_a(record->hour)]        >> ch_p(';') >>
      int_p[assign_a(record->minute)]      >> ch_p(';') >>
      int_p[assign_a(record->second)]      >> ch_p(';') >>

      int_p                                >> ch_p(';') >>
      int_p                                >> ch_p(';') >>
      int_p                                >> ch_p(';') >>
      int_p                                >> ch_p('/') >>

      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/') >>
      int_p                                >> ch_p('/')

    );

    return record;
  }

  case FiscalMemoryRecord::RT_EMPTY:
  default:
  {
    return NULL;
  }
  }
}


void FiscalPrinter::setClock(const Id &id, int year, int month, int day, int hour, int minute, int second)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(year);
  intParams.push_back(month);
  intParams.push_back(day);

  intParams.push_back(hour);
  intParams.push_back(minute);
  intParams.push_back(second);

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  execute(intParams, "$c", stringParams, true);
}


ClockInfo FiscalPrinter::getClock()
{
  vector<int> intParams;

  intParams.push_back(0); // parametr ignorowany

  execute(intParams, "#c", false);

  string result = read();

  ClockInfo info;

  parse(result.c_str(),

    str_p("1#C")                 >>

    int_p[assign_a(info.year)]   >> ch_p(';') >>
    int_p[assign_a(info.month)]  >> ch_p(';') >>
    int_p[assign_a(info.day)]    >> ch_p(';') >>
    int_p[assign_a(info.hour)]   >> ch_p(';') >>
    int_p[assign_a(info.minute)] >> ch_p(';') >>

    int_p

  );

  return info;
}


void FiscalPrinter::setVatRates(const Id &id, int count, float a, float b, float c, float d, float e, float f, float g)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(count);

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  switch (count)
  {
  case 1:
    stringParams.push_back(fromFloat(a) + "/");
    break;

  case 2:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    break;

  case 3:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    break;

  case 4:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    stringParams.push_back(fromFloat(d) + "/");
    break;

  case 5:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    stringParams.push_back(fromFloat(d) + "/");
    stringParams.push_back(fromFloat(e) + "/");
    break;

  case 6:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    stringParams.push_back(fromFloat(d) + "/");
    stringParams.push_back(fromFloat(e) + "/");
    stringParams.push_back(fromFloat(f) + "/");
    break;

  case 7:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    stringParams.push_back(fromFloat(d) + "/");
    stringParams.push_back(fromFloat(e) + "/");
    stringParams.push_back(fromFloat(f) + "/");
    stringParams.push_back(fromFloat(g) + "/");
    break;

  case 0:
  default:
    stringParams.push_back(fromFloat(a) + "/");
    stringParams.push_back(fromFloat(b) + "/");
    stringParams.push_back(fromFloat(c) + "/");
    stringParams.push_back(fromFloat(d) + "/");
    break;
  }

  execute(intParams, "$p", stringParams, true);
}


void FiscalPrinter::setHeader(const Id &id, const string &header)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0); // ustawienie nagłówka

  stringParams.push_back(header + (char)0xff);

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  execute(intParams, "$f", stringParams, true);
}


string FiscalPrinter::getHeader()
{
  execute("^u", true);

  string header;

  parse(read().c_str(),

    str_p("1#U") >> (*anychar_p )[assign_a(header)]

    );

  header = header.substr(0, header.size() - 4);

  return header;
}


void FiscalPrinter::openDrawer()
{
  vector<int> intParams;

  intParams.push_back(1);

  execute(intParams, "$d", false);
}


void FiscalPrinter::setDisplayMessage(const string &message)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(2);

  stringParams.push_back(message);

  execute(intParams, "$d", stringParams, false);
}


void FiscalPrinter::setDisplayMode(DISPLAY_MODE mode)
{
  vector<int> intParams;

  intParams.push_back((int)mode);

  execute(intParams, "$d", false);
}


void FiscalPrinter::setDiscountAlgorithm(DISCOUNT_ALGORITHM mode)
{
  vector<int> intParams;

  intParams.push_back((int)mode);

  execute(intParams, "$r", true);
}


ServiceDate FiscalPrinter::getServiceCheckDate()
{
  vector<int> intParams;

  intParams.push_back(11);

  execute(intParams, "^t", true);

  string result = read();

  ServiceDate info;

  parse(result.c_str(),

    int_p                                >>

    str_p("^t")                          >>

    int_p[assign_a(info.year)]           >> ch_p('/') >>
    int_p[assign_a(info.month)]          >> ch_p('/') >>
    int_p[assign_a(info.day)]            >> ch_p('/') >>

    !((*print_p)[assign_a(info.message)] >> ch_p('\r'))

    );

  return info;
}


ServiceDate FiscalPrinter::getServiceLockDate()
{
  vector<int> intParams;

  intParams.push_back(12);

  execute(intParams, "^t", true);

  string result = read();

  ServiceDate info;

  parse(result.c_str(),

    int_p                                >>

    str_p("^t")                          >>

    int_p[assign_a(info.year)]           >> ch_p('/')  >>
    int_p[assign_a(info.month)]          >> ch_p('/')  >>
    int_p[assign_a(info.day)]            >> ch_p('/')  >>

    *print_p                             >> ch_p('\r') >>

    (*print_p)[assign_a(info.message)]   >> ch_p('\r')

    );

  return info;
}


void FiscalPrinter::beginTransaction(int items, const ExtraLines &extraLines,
  CLIENT_ID_TYPE clientIdType, const string &clientId)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(items);

  if (extraLines.isEmpty() && clientIdType == CIDT_NONE) // pierwsza wersja rozkazu
  {
    execute(intParams, "$h", true);
  }
  else if (!extraLines.isEmpty() && clientIdType == CIDT_NONE) // druga wersja rozkazu (z liniami dodatkowymi)
  {
    intParams.push_back(extraLines.count());

    switch (extraLines.count())
    {
    case 1:
      stringParams.push_back(extraLines.line1 + "\r");
      break;

    case 2:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      break;

    case 3:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      stringParams.push_back(extraLines.line3 + "\r");
      break;

    default:
      break;
    }

    execute(intParams, "$h", stringParams, true);
  }
  else // trzecia wersja (pełna)
  {
    intParams.push_back(extraLines.count());

    intParams.push_back(0); // stała

    intParams.push_back((int)clientIdType);

    switch (extraLines.count())
    {
    case 1:
      stringParams.push_back(extraLines.line1 + "\r");
      break;

    case 2:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      break;

    case 3:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      stringParams.push_back(extraLines.line3 + "\r");
      break;

    default:
      break;
    }

    stringParams.push_back(clientId + "\r");

    execute(intParams, "$h", stringParams, true);
  }
}


void FiscalPrinter::printReceiptLine(const Item &item)
{
  vector<int> intParams;
  vector<string> stringParams;

  if (!item.barcode.empty()) // pozycja z kodem PLU (możliwy rabat z opisem)
  {
    intParams.push_back(item.line);

    intParams.push_back((int)item.discountType);
    intParams.push_back((int)item.discountDesc);

    stringParams.push_back(item.name + "\r");
    stringParams.push_back(item.barcode + "\r");
    stringParams.push_back(item.quantity + "\r");

    stringParams.push_back(item.vat + "/");
    stringParams.push_back(fromFloat(item.price) + "/");
    stringParams.push_back(fromFloat(item.gross) + "/");
    stringParams.push_back(fromFloat(item.discountValue) + "/");

    stringParams.push_back(item.discountName + "\r");

    execute(intParams, "^l", stringParams, true);
  }
  else if (!item.description.empty()) // pozycja z opisem towaru (możliwy rabat z opisem)
  {
    intParams.push_back(item.line);

    intParams.push_back((int)item.discountType);
    intParams.push_back((int)item.discountDesc);

    intParams.push_back(1);

    stringParams.push_back(item.name + "\r");
    stringParams.push_back(item.quantity + "\r");

    stringParams.push_back(item.vat + "/");
    stringParams.push_back(fromFloat(item.price) + "/");
    stringParams.push_back(fromFloat(item.gross) + "/");

    if (item.discountType != IDT_0)
    {
      stringParams.push_back(fromFloat(item.discountValue) + "/");

      if (item.discountName.empty())
      {
        stringParams.push_back("brak\r");
      }
      else
      {
        stringParams.push_back(item.discountName + "\r");
      }
    }

    stringParams.push_back(item.description + "\r");

    execute(intParams, "$l", stringParams, true);
  }
  else if (!item.discountName.empty()) // pozycja z opisem rabatu
  {
    intParams.push_back(item.line);

    intParams.push_back((int)item.discountType);
    intParams.push_back((int)item.discountDesc);

    stringParams.push_back(item.name + "\r");
    stringParams.push_back(item.quantity + "\r");

    stringParams.push_back(item.vat + "/");
    stringParams.push_back(fromFloat(item.price) + "/");
    stringParams.push_back(fromFloat(item.gross) + "/");
    stringParams.push_back(fromFloat(item.discountValue) + "/");

    stringParams.push_back(item.discountName + "\r");

    execute(intParams, "$l", stringParams, true);
  }
  else if (item.discountType != IDT_0) // pozycja z rabatem bez opisu
  {
    intParams.push_back(item.line);

    intParams.push_back((int)item.discountType);

    stringParams.push_back(item.name + "\r");
    stringParams.push_back(item.quantity + "\r");
    stringParams.push_back(item.vat + "/");
    stringParams.push_back(fromFloat(item.price) + "/");
    stringParams.push_back(fromFloat(item.gross) + "/");

    stringParams.push_back(fromFloat(item.discountValue) + "/");

    execute(intParams, "$l", stringParams, true);
  }
  else // pozycja bez rabatu
  {
    intParams.push_back(item.line);

    stringParams.push_back(item.name + "\r");
    stringParams.push_back(item.quantity + "\r");
    stringParams.push_back(item.vat + "/");
    stringParams.push_back(fromFloat(item.price) + "/");
    stringParams.push_back(fromFloat(item.gross) + "/");

    execute(intParams, "$l", stringParams, true);
  }
}


void FiscalPrinter::printDepositLine(DEPOSIT_TYPE type, const std::string &nr, string quantity, float price)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)type);

  stringParams.push_back(nr + "\r");

  stringParams.push_back(quantity + "\r");

  stringParams.push_back("P/"); // stała

  stringParams.push_back(fromFloat(price) + "/");

  stringParams.push_back(fromFloat(0.0) + "/"); // argument ignorowany

  execute(intParams, "$l", stringParams, true);
}


void FiscalPrinter::depositCollected(float amount, int nr, string quantity)
{
  vector<string> stringParams;

  stringParams.push_back(fromFloat(amount) + "/");

  if (nr != 0 && !quantity.empty())
  {
    stringParams.push_back(fromInt(nr) + "\r");
    stringParams.push_back(quantity + "\r");
  }

  execute("6$d", stringParams, true);
}


void FiscalPrinter::correctDepositCollected(float amount, int nr, string quantity)
{
  vector<string> stringParams;

  stringParams.push_back(fromFloat(amount) + "/");

  if (nr != 0 && !quantity.empty())
  {
    stringParams.push_back(fromInt(nr) + "\r");
    stringParams.push_back(quantity + "\r");
  }

  execute("7$d", stringParams, true);
}


void FiscalPrinter::depositReturned(float amount, int nr, string quantity)
{
  vector<string> stringParams;

  stringParams.push_back(fromFloat(amount) + "/");

  if (nr != 0 && !quantity.empty())
  {
    stringParams.push_back(fromInt(nr) + "\r");
    stringParams.push_back(quantity + "\r");
  }

  execute("10$d", stringParams, true);
}


void FiscalPrinter::correctDepositReturned(float amount, int nr, string quantity)
{
  vector<string> stringParams;

  stringParams.push_back(fromFloat(amount) + "/");

  if (nr != 0 && !quantity.empty())
  {
    stringParams.push_back(fromInt(nr) + "\r");
    stringParams.push_back(quantity + "\r");
  }

  execute("11$d", stringParams, true);
}


void FiscalPrinter::cancelTransaction(const Id &id)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0);

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  execute(intParams, "$e", stringParams, true);
}


void FiscalPrinter::confirmTransaction(const Id &id, float cashIn, float total,
  TRANSACTION_DISCOUNT_TYPE discountType, float discountValue, const ExtraLines &extraLines)
{
  vector<int> intParams;
  vector<string> stringParams;

  if (discountType != TDT_0)
  {
    intParams.push_back(1); // zatwierdzenie

    intParams.push_back(extraLines.count());

    intParams.push_back(0); // zakończenie

    intParams.push_back((int)discountType);

    intParams.push_back(1); // stała wartość

    if (id.isEmpty())
    {
      stringParams.push_back("000\r");
    }
    else
    {
      stringParams.push_back(id.printerId + id.operatorId + "\r");
    }

    switch (extraLines.count())
    {
    case 1:
      stringParams.push_back(extraLines.line1 + "\r");
      break;

    case 2:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      break;

    case 3:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      stringParams.push_back(extraLines.line3 + "\r");
      break;

    default:
      break;
    }

    stringParams.push_back(fromFloat(cashIn) + "/");
    stringParams.push_back(fromFloat(total) + "/");
    stringParams.push_back(fromFloat(discountValue) + "/");

    execute(intParams, "$e", stringParams, true);
  }
  else if (!extraLines.isEmpty())
  {
    intParams.push_back(1); // zatwierdzenie

    intParams.push_back(0); // rabat 0

    intParams.push_back(extraLines.count());

    intParams.push_back(0); // zakończenie

    if (id.isEmpty())
    {
      stringParams.push_back("000\r");
    }
    else
    {
      stringParams.push_back(id.printerId + id.operatorId + "\r");
    }

    switch (extraLines.count())
    {
    case 1:
      stringParams.push_back(extraLines.line1 + "\r");
      break;

    case 2:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      break;

    case 3:
      stringParams.push_back(extraLines.line1 + "\r");
      stringParams.push_back(extraLines.line2 + "\r");
      stringParams.push_back(extraLines.line3 + "\r");
      break;

    default:
      break;
    }

    stringParams.push_back(fromFloat(cashIn) + "/");
    stringParams.push_back(fromFloat(total) + "/");

    execute(intParams, "$e", stringParams, true);
  }
  else
  {
    intParams.push_back(1); // zatwierdzenie

    if (id.isEmpty())
    {
      stringParams.push_back("000\r");
    }
    else
    {
      stringParams.push_back(id.printerId + id.operatorId + "\r");
    }

    stringParams.push_back(fromFloat(cashIn) + "/");
    stringParams.push_back(fromFloat(total) + "/");

    execute(intParams, "$e", stringParams, true);
  }
}


void FiscalPrinter::confirmTransactionWithPaymentForms1(const Id &id, const PaymentFormsInfo1 &info,
  float total, TRANSACTION_DISCOUNT_TYPE discountType, float discountValue, const ExtraLines &extraLines)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(extraLines.count());

  intParams.push_back(0); // zakończenie

  intParams.push_back(0); // parametr ignorowany

  intParams.push_back((int)discountType);

  intParams.push_back((int)info.cashFlag);
  intParams.push_back((int)info.cardFlag);
  intParams.push_back((int)info.chequeFlag);
  intParams.push_back((int)info.couponFlag);
  intParams.push_back((int)info.depositCollectedFlag);
  intParams.push_back((int)info.depositReturnedFlag);
  intParams.push_back((int)info.changeFlag);

  if (id.isEmpty())
  {
    stringParams.push_back("000\r");
  }
  else
  {
    stringParams.push_back(id.printerId + id.operatorId + "\r");
  }

  switch (extraLines.count())
  {
  case 1:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back("\r\r\r\r");
    break;

  case 2:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    stringParams.push_back("\r\r\r");
    break;

  case 3:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    stringParams.push_back(extraLines.line3 + "\r");
    stringParams.push_back("\r\r");
    break;

  default:
    stringParams.push_back("\r\r\r\r\r");
    break;
  }

  stringParams.push_back(info.cardName + "\r");
  stringParams.push_back(info.chequeName + "\r");
  stringParams.push_back(info.couponName + "\r");

  stringParams.push_back(fromFloat(total) + "/");
  stringParams.push_back(fromFloat(discountValue) + "/");

  stringParams.push_back(fromFloat(info.cashIn) + "/");
  stringParams.push_back(fromFloat(info.cardIn) + "/");
  stringParams.push_back(fromFloat(info.chequeIn) + "/");
  stringParams.push_back(fromFloat(info.couponIn) + "/");

  stringParams.push_back(fromFloat(info.depositCollected) + "/");
  stringParams.push_back(fromFloat(info.depositReturned) + "/");

  stringParams.push_back(fromFloat(info.checkOut) + "/");

  execute(intParams, "$x", stringParams, true);
}


void FiscalPrinter::confirmTransactionWithPaymentForms2(const Id &id, const PaymentFormsInfo2 &info,
  float total, DISCOUNT_TYPE discountType, float discountValue, string sysNr, bool summary,
  const ExtraLines &extraLines)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(extraLines.count());

  intParams.push_back(0); // zakończenie

  intParams.push_back((int)summary);

  intParams.push_back(0); // znak DSP (ignorowane)

  intParams.push_back((int)discountType);

  intParams.push_back((int)info.depositCollected.size());
  intParams.push_back((int)info.depositReturned.size());

  intParams.push_back(sysNr.size() > 0 ? 1 : 0);

  intParams.push_back((int)info.paymentForms.size());

  intParams.push_back((int)info.changeFlag);
  intParams.push_back((int)info.cashFlag);

  for (size_t i = 0; i < info.paymentForms.size(); ++i)
  {
    intParams.push_back((int)info.paymentForms[i].type);
  }

  stringParams.push_back(id.printerId + "\r");
  stringParams.push_back(id.operatorId + "\r");

  stringParams.push_back(sysNr + "\r");

  switch (extraLines.count())
  {
  case 1:
    stringParams.push_back(extraLines.line1 + "\r");
    break;

  case 2:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    break;

  case 3:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    stringParams.push_back(extraLines.line3 + "\r");
    break;

  default:
    break;
  }

  for (size_t i = 0; i < info.paymentForms.size(); ++i)
  {
    stringParams.push_back(info.paymentForms[i].name + "\r");
  }

  for (size_t i = 0; i < info.depositCollected.size(); ++i)
  {
    stringParams.push_back(info.depositCollected[i].nr + "\r");
  }

  for (size_t i = 0; i < info.depositCollected.size(); ++i)
  {
    stringParams.push_back(info.depositCollected[i].quantity + "\r");
  }

  for (size_t i = 0; i < info.depositReturned.size(); ++i)
  {
    stringParams.push_back(info.depositReturned[i].nr + "\r");
  }

  for (size_t i = 0; i < info.depositReturned.size(); ++i)
  {
    stringParams.push_back(info.depositReturned[i].quantity + "\r");
  }

  stringParams.push_back(fromFloat(total) + "/");

  stringParams.push_back("0/"); // DSP (ignorowane)

  stringParams.push_back(fromFloat(discountValue) + "/");

  stringParams.push_back(fromFloat(info.cashIn) + "/");

  for (size_t i = 0; i < info.paymentForms.size(); ++i)
  {
    stringParams.push_back(fromFloat(info.paymentForms[i].amount) + "/");
  }

  stringParams.push_back(fromFloat(info.changeOut) + "/");

  for (size_t i = 0; i < info.depositCollected.size(); ++i)
  {
    stringParams.push_back(fromFloat(info.depositCollected[i].amount) + "/");
  }

  for (size_t i = 0; i < info.depositReturned.size(); ++i)
  {
    stringParams.push_back(fromFloat(info.depositReturned[i].amount) + "/");
  }

  execute(intParams, "$y", stringParams, true);
}


void FiscalPrinter::paymentFormService(SERVICE_TYPE serviceType,
 PAYMENT_TYPE paymentType, float amount, const string &name)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)serviceType);
  intParams.push_back((int)paymentType);

  stringParams.push_back(fromFloat(amount) + "/");
  stringParams.push_back(name + "\r");

  execute(intParams, "$b", stringParams, true);
}


void FiscalPrinter::addDiscount(DISCOUNT_TYPE discountType, const string &name, float value)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)discountType);

  stringParams.push_back(name + "\r");
  stringParams.push_back(fromFloat(value) + "/");

  execute(intParams, "$n", stringParams, true);
}


void FiscalPrinter::addVatRateDiscount(int vat, DISCOUNT_TYPE discountType,  DISCOUNT_DESCRIPTION_TYPE discountDescription,
  float amount, float discountValue, const string &discountName)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(vat);
  intParams.push_back((int)discountType);
  intParams.push_back((int)discountDescription);

  stringParams.push_back(fromFloat(amount) + "/");
  stringParams.push_back(fromFloat(discountValue) + "/");

  stringParams.push_back(discountName + "\r");

  execute(intParams, "$L", stringParams, true);
}


void FiscalPrinter::addSubtotalDiscount(DISCOUNT_TYPE discountType, DISCOUNT_DESCRIPTION_TYPE discountDescription,
  float subtotal, float discount, string discountName)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)discountType);
  intParams.push_back((int)discountDescription);

  stringParams.push_back(fromFloat(subtotal) + "/");
  stringParams.push_back(fromFloat(discount) + "/");

  stringParams.push_back(discountName + "\r");

  execute(intParams, "$Y", stringParams, true);
}


void FiscalPrinter::extraLineContainerReturned(string name, const string &quantity, float amount)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(8); // stała

  stringParams.push_back(name + "\r");
  stringParams.push_back(quantity + "\r");

  stringParams.push_back(fromFloat(amount) + "/");

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::extraLineContainerReceived(string name, const string &quantity, float amount)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(4); // stała

  stringParams.push_back(name + "\r");
  stringParams.push_back(quantity + "\r");

  stringParams.push_back(fromFloat(amount) + "/");

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::formsOfPaymentClearing()
{
  vector<int> intParams;

  intParams.push_back(12); // stała

  execute(intParams, "$z", true);
}


void FiscalPrinter::extraLine(EXTRA_LINE_TYPE footerType, const string &text)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(20); // stała

  intParams.push_back((int)footerType);

  stringParams.push_back(text + "\r");

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::defineInfoLines(const ExtraLines &lines)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(24); // stała

  intParams.push_back(lines.count());

  switch (lines.count())
  {
  case 1:
    stringParams.push_back(lines.line1 + "\r");
    break;

  case 2:
    stringParams.push_back(lines.line1 + "\r");
    stringParams.push_back(lines.line2 + "\r");
    break;

  case 3:
    stringParams.push_back(lines.line1 + "\r");
    stringParams.push_back(lines.line2 + "\r");
    stringParams.push_back(lines.line3 + "\r");
    break;

  default:
    break;
  }

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::euroPayment(float exchange, float amount, float cashIn, float checkEuro, float checkPln)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(99); // stała
  intParams.push_back(5); // stała

  stringParams.push_back(fromFloat(exchange) + "\r");
  stringParams.push_back(fromFloat(amount) + "\r");
  stringParams.push_back(fromFloat(cashIn) + "\r");
  stringParams.push_back(fromFloat(checkEuro) + "\r");
  stringParams.push_back(fromFloat(checkPln) + "\r");

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::finish(bool nextHeader)
{
  vector<int> intParams;

  intParams.push_back(28); // stała

  intParams.push_back(nextHeader ? 2 : 0);

  execute(intParams, "$z", true);
}


void FiscalPrinter::beginInvoice(const BeginInvoiceData &data)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(data.items);
  intParams.push_back(data.clientLines.size());

  intParams.push_back(1); // stała

  intParams.push_back((int)data.printCopy);
  intParams.push_back((int)data.topMargin);

  intParams.push_back(0); // parametr ignorowany

  intParams.push_back(data.additionalCopies);

  intParams.push_back(0); // parametr ignorowany
  intParams.push_back(0); // parametr ignorowany

  intParams.push_back((int)data.signature);

  stringParams.push_back(data.invoiceNr + "\r");

  for (size_t i = 0; i < data.clientLines.size(); ++i)
  {
    stringParams.push_back(data.clientLines[i] + "\r");
  }

  stringParams.push_back(data.nip + "\r");

  stringParams.push_back(data.timeout + "\r");

  stringParams.push_back(data.paymentForm + "\r");

  stringParams.push_back(data.client + "\r");
  stringParams.push_back(data.seller + "\r");

  stringParams.push_back(data.systemNr + "\r");

  execute(intParams, "$h", stringParams, true);
}


void FiscalPrinter::finishInvoice(const Id &id, const FinishInvoiceData &data)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(1); // zatwierdzenie

  intParams.push_back(0); // parametr ignorowany

  intParams.push_back(data.extraLines.count());

  intParams.push_back(0); // parametr ignorowany
  intParams.push_back(0); // parametr ignorowany

  intParams.push_back(1); // stała wartość

  intParams.push_back(data.payedFlag);

  intParams.push_back((int)data.client);
  intParams.push_back((int)data.seller);

  if (id.isEmpty())
  {
    stringParams.push_back("000\r");
  }
  else
  {
    stringParams.push_back(id.printerId + id.operatorId + "\r");
  }

  switch (data.extraLines.count())
  {
  case 1:
    stringParams.push_back(data.extraLines.line1 + "\r");
    break;

  case 2:
    stringParams.push_back(data.extraLines.line1 + "\r");
    stringParams.push_back(data.extraLines.line2 + "\r");
    break;

  case 3:
    stringParams.push_back(data.extraLines.line1 + "\r");
    stringParams.push_back(data.extraLines.line2 + "\r");
    stringParams.push_back(data.extraLines.line3 + "\r");
    break;

  default:
    break;
  }

  stringParams.push_back(data.payed + "\r");

  stringParams.push_back(data.clientName + "\r");
  stringParams.push_back(data.sellerName + "\r");

  stringParams.push_back(fromFloat(data.cashIn) + "/");
  stringParams.push_back(fromFloat(data.total) + "/");

  stringParams.push_back(fromFloat(data.discountValue) + "/");

  execute(intParams, "$e", stringParams, true);
}


void FiscalPrinter::setInvoiceOption(const InvoiceOptions &options)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(options.additionalCopies);

  intParams.push_back((int)options.client);
  intParams.push_back((int)options.seller);

  intParams.push_back((int)options.payedFlag);

  intParams.push_back(options.year);
  intParams.push_back(options.month);
  intParams.push_back(options.day);

  intParams.push_back(options.summaryOption);

  intParams.push_back(options.invoiceOptions2);

  intParams.push_back((int)options.clientIdType);

  intParams.push_back(options.invoiceOptions3);


  stringParams.push_back(options.timeout + "\r");
  stringParams.push_back(options.paymentForm + "\r");
  stringParams.push_back(options.clientName + "\r");
  stringParams.push_back(options.sellerName + "\r");
  stringParams.push_back(options.systemNr + "\r");

  execute(intParams, "@c", stringParams, true);
}


void FiscalPrinter::extraLinesInvoice(EXTRA_LINE_TYPE type, const string &text)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(20); // stała

  intParams.push_back((int)type);

  stringParams.push_back(text + "\r");

  execute(intParams, "$z", stringParams, true);
}


void FiscalPrinter::login(const Id &id)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0); // parametr ignorowany

  stringParams.push_back(id.operatorId + "\r");
  stringParams.push_back(id.printerId + "\r");

  execute(intParams, "#p", stringParams, true);
}


void FiscalPrinter::logout(const Id &id)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0); // parametr ignorowany

  stringParams.push_back(id.operatorId + "\r");
  stringParams.push_back(id.printerId + "\r");

  execute(intParams, "#q", stringParams, true);
}


void FiscalPrinter::paymentToCash(const Id &id, float cashIn, bool euro)
{
  vector<int> intParams;
  vector<string> stringParams;

  if (euro)
  {
    intParams.push_back(99);
  }
  else
  {
    intParams.push_back(0);
  }

  stringParams.push_back(fromFloat(cashIn) + "/");

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  execute(intParams, "#i", stringParams, true);
}


void FiscalPrinter::withdrawalFromCash(const Id &id, float cashOut, bool euro)
{
  vector<int> intParams;
  vector<string> stringParams;

  if (euro)
  {
    intParams.push_back(99);
  }
  else
  {
    intParams.push_back(0);
  }

  stringParams.push_back(fromFloat(cashOut) + "/");

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");
  }

  execute(intParams, "#d", stringParams, true);
}


void FiscalPrinter::printCashState(const Id &id)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0); // parametr ignorowany

  if (!id.isEmpty())
  {
    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");

    execute(intParams, "#t", stringParams, true);
  }
  else
  {
    execute(intParams, "#t", true);
  }
}


void FiscalPrinter::printShiftReport(const Id &id, bool reset, const string &shift)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)!reset);

  stringParams.push_back(shift + "\r");

  stringParams.push_back(id.operatorId + "\r");

  if (!id.printerId.empty())
  {
    stringParams.push_back(id.printerId + "\r");
  }

  execute(intParams, "#k", stringParams, true);
}


void FiscalPrinter::printDailyReport(const Id &id)
{
  if (id.isEmpty())
  {
    execute("#r", true);
  }
  else
  {
    vector<string> stringParams;

    stringParams.push_back(id.printerId + "\r");
    stringParams.push_back(id.operatorId + "\r");

    execute("#r", stringParams, true);
  }
}


void FiscalPrinter::printPeriodicalReportByDate(const Id &id, int fromYear, int fromMonth,
  int fromDay, int toYear, int toMonth, int toDay, PERIODICAL_REPORT_TYPE type)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(fromYear);
  intParams.push_back(fromMonth);
  intParams.push_back(fromDay);

  intParams.push_back(toYear);
  intParams.push_back(toMonth);
  intParams.push_back(toDay);

  intParams.push_back((int)type);

  if (!id.isEmpty())
  {
    stringParams.push_back(id.operatorId + "\r");
    stringParams.push_back(id.printerId + "\r");
  }

  execute(intParams, "#o", stringParams, true);
}


void FiscalPrinter::printPeriodicalReportByNumber(const Id &id, long fromNr, long toNr,
  PERIODICAL_REPORT_TYPE type)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back((int)type);

  stringParams.push_back(fromLong(fromNr) + "/");
  stringParams.push_back(fromLong(toNr) + "/");

  if (!id.isEmpty())
  {
    stringParams.push_back(id.operatorId + "\r");
    stringParams.push_back(id.printerId + "\r");
  }

  execute(intParams, "#o", stringParams, true);
}


void FiscalPrinter::containerReturn(const string &text)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(0); // parametr ignorowany

  stringParams.push_back(text + "\r");

  execute(intParams, "#w", stringParams, true);
}


void FiscalPrinter::saleReceipt(const Id &id, const SaleReceiptData &data)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(data.printId);
  intParams.push_back((int)data.printOption);

  stringParams.push_back(id.printerId + "\r");
  stringParams.push_back(id.operatorId + "\r");

  stringParams.push_back(data.receipt + "\r");

  stringParams.push_back(data.clientName + "\r");
  stringParams.push_back(data.terminal + "\r");
  stringParams.push_back(data.cardName + "\r");
  stringParams.push_back(data.cardNr + "\r");

  stringParams.push_back(fromInt(data.month) + "\r");
  stringParams.push_back(fromInt(data.year) + "\r");

  stringParams.push_back(data.authCode + "\r");

  stringParams.push_back(fromFloat(data.amount) + "/");

  execute(intParams, "#g", stringParams, true);
}


void FiscalPrinter::returnOfArticle(const Id &id, const SaleReceiptData &data)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(data.printId);
  intParams.push_back((int)data.printOption);

  stringParams.push_back(id.printerId + "\r");
  stringParams.push_back(id.operatorId + "\r");

  stringParams.push_back(data.receipt + "\r");

  stringParams.push_back(data.clientName + "\r");
  stringParams.push_back(data.terminal + "\r");
  stringParams.push_back(data.cardName + "\r");
  stringParams.push_back(data.cardNr + "\r");

  stringParams.push_back(fromInt(data.month) + "\r");
  stringParams.push_back(fromInt(data.year) + "\r");

  stringParams.push_back(data.authCode + "\r");

  stringParams.push_back(fromFloat(data.amount) + "/");

  execute(intParams, "#h", stringParams, true);
}


void FiscalPrinter::beginNonFiscal(int printNr, int headerNr)
{
  vector<int> intParams;

  intParams.push_back(0); // stała

  intParams.push_back(printNr);
  intParams.push_back(headerNr);

  execute(intParams, "$w", true);
}


void FiscalPrinter::printNonFiscal(const NonFiscalLine &line)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(line.printNr);
  intParams.push_back(line.lineNr);

  intParams.push_back((int)line.bold);
  intParams.push_back((int)line.inverse);

  intParams.push_back(line.font);

  intParams.push_back((int)line.center);

  intParams.push_back((int)line.attrs);

  for (size_t i = 0; i < line.lines.size(); ++i)
  {
    stringParams.push_back(line.lines[i] + "\r");
  }

  execute(intParams, "$w", stringParams, true);
}


void FiscalPrinter::finishNonFiscal(int printNr, const string &sysNr, const ExtraLines &extraLines)
{
  vector<int> intParams;
  vector<string> stringParams;

  intParams.push_back(1); // stała

  intParams.push_back(printNr);
  intParams.push_back(sysNr.empty() ? 0 : 1);
  intParams.push_back(extraLines.count());

  stringParams.push_back(sysNr + "\r");

  switch (extraLines.count())
  {
  case 1:
    stringParams.push_back(extraLines.line1 + "\r");
    break;

  case 2:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    break;

  case 3:
    stringParams.push_back(extraLines.line1 + "\r");
    stringParams.push_back(extraLines.line2 + "\r");
    stringParams.push_back(extraLines.line3 + "\r");
    break;

  default:
    break;
  }

  execute(intParams, "$w", stringParams, true);
}


void FiscalPrinter::setClientId(CLIENT_ID_TYPE clientIdType, const string &clientId)
{
  if (clientIdType != CIDT_NONE)
  {
    vector<int> intParams;
    vector<string> stringParams;

    intParams.push_back(100); // stała

    intParams.push_back((int)clientIdType);

    stringParams.push_back(clientId + "\r");

    execute(intParams, "$z", true);
  }
}


void FiscalPrinter::descriptorsReport()
{
  vector<int> intParams;

  intParams.push_back(1); // parametr ignorowany

  execute(intParams, "@d", true);
}


void FiscalPrinter::paperFeed(int lines)
{
  vector<int> intParams;

  intParams.push_back(lines);

  execute(intParams, "#l", true);
}


void FiscalPrinter::debugGenerateError()
{
  execute("#l", false);
}


string FiscalPrinter::fromFloat(float number)
{
  stringstream stream;
  stream << number;
  return stream.str();
}


string FiscalPrinter::fromInt(int number)
{
  return lexical_cast<string>(number);
}


string FiscalPrinter::fromLong(long number)
{
  return lexical_cast<string>(number);
}


std::string FiscalPrinter::toMazovia(const std::string &text)
{
  string result;

  for (size_t i = 0; i < text.size(); ++i)
  {
    switch ((char)text[i])
    {

    case (char)0xc3: // kod kontrolny utf8
    {
      if (i + 1 < text.size())
      {
        switch ((char)text[i + 1])
        {

        case (char)0x93: result.push_back((char)0xa3); break; // Ó
        case (char)0xb3: result.push_back((char)0xa2); break; // ó

        default:
          break; // pomijamy
        }

        ++i;
      }

      break;
    }

    case (char)0xc4: // kod kontrolny utf8
    {
      if (i + 1 < text.size())
      {
        switch ((char)text[i + 1])
        {

        case (char)0x84: result.push_back((char)0x8f); break; // Ą
        case (char)0x86: result.push_back((char)0x95); break; // Ć
        case (char)0x98: result.push_back((char)0x90); break; // Ę
        case (char)0x85: result.push_back((char)0x86); break; // ą
        case (char)0x87: result.push_back((char)0x8d); break; // ć
        case (char)0x99: result.push_back((char)0x91); break; // ę

        default:
          break; // pomijamy
        }

        ++i;
      }

      break;
    }

    case (char)0xc5: // kod kontrolny utf8
    {
      if (i + 1 < text.size())
      {
        switch ((char)text[i + 1])
        {

        case (char)0x81: result.push_back((char)0x9c); break; // Ł
        case (char)0x83: result.push_back((char)0xa5); break; // Ń
        case (char)0x9a: result.push_back((char)0x98); break; // Ś
        case (char)0xb9: result.push_back((char)0xa0); break; // Ź
        case (char)0xbb: result.push_back((char)0xa1); break; // Ż
        case (char)0x82: result.push_back((char)0x92); break; // ł
        case (char)0x84: result.push_back((char)0xa4); break; // ń
        case (char)0x9b: result.push_back((char)0x9e); break; // ś
        case (char)0xba: result.push_back((char)0xa6); break; // ź
        case (char)0xbc: result.push_back((char)0xa7); break; // ż

        default:
          break; // pomijamy
        }

        ++i;
      }

      break;
    }

    default: // inny znak uft8
      result.push_back(text[i]);
      break;

    }
  }

  return result;
}


void FiscalPrinter::execute(const vector<int> &intParams, const string &cmd,
  const vector<string> &stringParams, bool appendCtrlByte)
{
  string content = "";

  if (intParams.size())
  {
    for (size_t i = 0; i < intParams.size(); ++i)
    {
      content += lexical_cast<string>(intParams[i]) + ";";
    }

    content = content.substr(0, content.size() - 1); // usunięcie ostatniego znaku ';'
  }

  content += cmd;

  for (size_t i = 0; i < stringParams.size(); ++i)
  {
    content += toMazovia(stringParams[i]);
  }

  if (appendCtrlByte)
  {
    content += calculateCtrlByte(content);
  }

  write(string("\x1bP") + content + string("\x1b\\"));
}


void FiscalPrinter::execute(const string &cmd, bool appendCtrlByte)
{
  string content = "";

  content += cmd;

  if (appendCtrlByte)
  {
    content += calculateCtrlByte(content);
  }

  write(string("\x1bP") + content + string("\x1b\\"));
}


void FiscalPrinter::execute(const vector<int> &intParams, const string &cmd, bool appendCtrlByte)
{
  string content = "";

  if (intParams.size())
  {
    for (size_t i = 0; i < intParams.size(); ++i)
    {
      content += lexical_cast<string>(intParams[i]) + ";";
    }

    content = content.substr(0, content.size() - 1); // usunięcie ostatniego znaku ';'
  }

  content += cmd;

  if (appendCtrlByte)
  {
    content += calculateCtrlByte(content);
  }

  write(string("\x1bP") + content + string("\x1b\\"));
}


void FiscalPrinter::execute(const string &cmd, const vector<string> &stringParams, bool appendCtrlByte)
{
  string content = "";

  content += cmd;

  for (size_t i = 0; i < stringParams.size(); ++i)
  {
    content += toMazovia(stringParams[i]);
  }

  if (appendCtrlByte)
  {
    content += calculateCtrlByte(content);
  }

  write(string("\x1bP") + content + string("\x1b\\"));
}


string fp::FiscalPrinter::calculateCtrlByte(const string &str)
{
  char byte = 0xff;

  for (string::const_iterator i = str.begin(); i != str.end(); i++)
  {
    byte ^= lexical_cast<char>(*i);
  }

  stringstream stream;
  stream << std::uppercase << std::hex << static_cast<unsigned int>(byte);

  string s = stream.str();

  if (s.size() > 2)
  {
    return s.substr(s.size() - 2);
  }

  return s;
}


string FiscalPrinter::read()
{
  if (port && io)
  {
    if (port->is_open())
    {
      char c = 0;
      string result = "";
      while (true)
      {
        boost::asio::read(*port, boost::asio::buffer(&c, 1));
        result.append(lexical_cast<string>(c));
        if (result.size() >=2)
        {
          if (c == '\\' && result[result.size() - 2] == (char)0x1b)
          {
            break;
          }
        }
      }

      size_t pos = result.find("\x1bP");
      if (pos != string::npos)
      {
        result = result.substr(pos + 2);
      }

      // ta linia jest zbedna po zaaplikowaniu latki
      // result = boost::regex_replace(result, boost::regex("1`"), "");

#ifdef DEBUG_FISCAL_PRINTER
      std::cout << ">>> " << result << std::endl;
#endif

      return result;
    }
  }

  return string();
}


char FiscalPrinter::readOneByte()
{
  if (port && io)
  {
    if (port->is_open())
    {
      char result = 0;
      boost::asio::read(*port, boost::asio::buffer(&result, 1));

      return result;
    }
  }

  return 0;
}


void FiscalPrinter::write(const string &str)
{
  if (port && io)
  {
    if (port->is_open())
    {
      boost::asio::write(*port, boost::asio::buffer(str));

#ifdef DEBUG_FISCAL_PRINTER
      std::cout << "<<< " << str << std::endl;
#endif

      return;
    }
  }
}


void FiscalPrinter::writeOneByte(char c)
{
  if (port && io)
  {
    if (port->is_open())
    {
      boost::asio::write(*port, boost::asio::buffer(&c, 1));

      return;
    }
  }
}
