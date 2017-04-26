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


#ifndef __FP_FISCALPRINTER_HPP__
#define __FP_FISCALPRINTER_HPP__


#include <fiscal-printer/Common.hpp>


namespace fp
{


/// Obsługa drukarek fiskalnych firmy NOVITUS/POSNET obsługujących protokół POSNET.
/**
    @note Na podstawie opisu protokołu komunikacyjnego drukarek fiskalnych
          opublikowanego przez firmę NOVITUS, wersja 3.1.014.

    @note Przetestowano z drukarką BONO E 1.10.
 */
class FiscalPrinter
{

public:

  FiscalPrinter();
  ~FiscalPrinter();

  /// Otwórz port.
  /**
      @param device Nazwa urządzenia (n.p. "/dev/ttyS0" lub "/dev/ttyUSB0")

      @note Urządzenie jest otwierane w trybie 'do odczytu' i 'do zapisu'. Użytkownik
            musi mieć pełne uprawnienia do urządzenia (n.p. użytkownik powinien należeć
            do grupy 'dialout').

      @note Jeśli drukarka jest podłączona przez port USB, należy załadować moduł jądra
            obsługujący konwerter/adapter RS232/USB. Jeśli urządzenie zostanie poprawnie
            rozpoznane, wtedy pojawi się plik urządzenia /dev/ttyUSB0 (lub podobny).
  */
  void open(const std::string &device, int baudRate = 9600);

  /// Zamknij port.
  /**
      @note Metoda wołana w destruktorze.
   */
  void close();

  /// Sygnał dźwiękowy (BEL).
  void bell();

  /// Przerwanie interpretacji rozkazu (CAN).
  void cancel();

  /// Żądanie wysłania statusu (ENQ).
  /**
      @note Wykonywane synchronicznie (względem głównej pętli w drukarce).

      @see FiscalPrinter::getDleStatus

      @note Metoda czeka na odpowiedź drukarki.
   */
  EnqStatus getEnqStatus();

  /// Żądanie wysłania statusu (DLE).
  /**
      @note Wykonywanie asynchronicznie (względem głównej pętli w drukarce).

      @see FiscalPrinter::getEnqStatus

      @note Metoda czeka na odpowiedź drukarki.
   */
  DleStatus getDleStatus();

  /// Ustawienie trybu obsługi błędów (#e).
  /**
      @param mode Tryb obsługi błędów

      @note Domyślnie drukarka jest w trybie EHM_0.
   */
  void setErrorHandlingMode(fp::ERROR_HANDLING_MODE mode);

  /// Żądanie odesłania kodu błędu ostatniego rozkazu (#n).
  /**
      @note Według dokumentacji status błędu nie jest resetowany po wykonaniu tej operacji,
            ale wpraktyce wychodzi na to, że jednak jest.

      @note Absolutnie niezawodna procedura testowania poprawności wykonania rozkazu powinna
            testować również bit CMD odpowiedzi na kod ENQ, który powinno się wysyłać po każdym
            rozkazie.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::PrinterError getLastError();

  /// Żądanie odesłania informacji kasowych (1) (#s).
  /**
      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo1 getCashRegisterInfo1();

  /// Żądanie odesłania informacji kasowych (2) (#s).
  /**
      @param mode Tryb
      @param invoices Zwrócone zostaną dane totalizerów faktur (domyślnie dane totalizerów paragonów).

      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo2 getCashRegisterInfo2(fp::CASH_REGISTER_INFO_2_MODE mode, bool invoices);

  /// Żądanie odesłania informacji kasowych (3) (#s).
  /**
      Szczegółowe informacje na temat zajętości pamięci fiskalnej, wartość ostatniego
      paragonu i ilość zablokowanych towarów.

      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo3 getCashRegisterInfo3();

  /// Żądanie odesłania informacji kasowych (4) (#s).
  /**
      Informacje kasowe: numer ostatniego wydruku i ilość wydrukowanych faktur.

      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo4 getCashRegisterInfo4();

  /// Żądanie odesłania informacji kasowych (5) (#s).
  /**
      Informacje odnośnie stanu nośnika pamięci podręcznej (drukarki z obsługą kopii elektronicznej).

      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.

      @todo Drukarka nie odpowiada. Sprawdzić to po fiskalizacji/inicjalizacji karty pamięci.
   */
  fp::CashRegisterInfo5 getCashRegisterInfo5();

  /// Żądanie odesłania informacji kasowych (6) (#s).
  /**
      Informacje o wartości totalizerów i stanie transakcji.

      @param mode Tryb

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo6 getCashRegisterInfo6(fp::CASH_REGISTER_INFO_6_MODE mode);

  /// Żądanie odesłania informacji kasowych (7) (#s).
  /**
      Informacje odnośnie stanu wybranej pozycji faktury VAT (drukarki z obsługą faktur).

      @param item Numer pozycji na fakturze
      @param mode Tryb

      @note Kod błędu ostatniego rozkazu jest zerowany.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::CashRegisterInfo7 getCashRegisterInfo7(int item, fp::CASH_REGISTER_INFO_7_MODE mode);

  /// Żądanie odesłania typu i wersji oprogramowania (#v).
  /**
      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::VersionInfo getVersionInfo();

  /// Żądanie odesłania informacji o urządzeniu (1) ($i).
  /**
      Odsyła informacje ogólne.

      @note Metoda czeka na odpowiedź drukarki.

      @todo Drukarka nie odpowiada. Sprawdzić to po fiskalizacji/inicjalizacji karty pamięci.
   */
  fp::DeviceInfo1 getDeviceInfo1();

  /// Żądanie odesłania informacji o urządzeniu (2) ($i).
  /**
      Odsyła informacje szczegółowe.

      @note Metoda czeka na odpowiedź drukarki.

      @todo Drukarka nie odpowiada. Sprawdzić to po fiskalizacji/inicjalizacji karty pamięci.
   */
  fp::DeviceInfo2 getDeviceInfo2();

  /// Rozpoczęcie odczytu zawartości pamięci fiskalnej (rozpoczynając od daty) (#s).
  /**
      @param year Rok <0;99>
      @param month Miesiąc <1;12>
      @param day Dzień <1;31>
      @param hour Godzina <0;23>
      @param minute Minuta <0;59>
      @param second Sekunda <0;59>
   */
  void beginFiscalMemoryReadByDate(int year, int month, int day, int hour, int minute, int second);

  /// Rozpoczęcie odczytu zawartości pamięci fiskalnej (rozpoczynając od podanego numeru rekordu) (#s).
  /**
      @param row Numer rekordu
   */
  void beginFiscalMemoryReadByRow(long row);

  /// Żądanie odesłania bieżącego rekordu (#s).
  /**
      @note Zwracany zostaje wskaźnik na obiekt, który został utworzony na stercie. Należy
            zwolnić pamięć operatorem 'delete'.

      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::FiscalMemoryRecord *getFiscalMemoryRecord();

  /// Ustawienie zegara ($c).
  /**
      @param id Identyfikator kasy/kasjera
      @param year Rok <0;99>
      @param month Miesiąc <1;12>
      @param day Dzień <1;31>
      @param hour Godzina <0;23>
      @param minute Minuta <0;59>
      @param second Sekunda <0;59>

      @note W trybie fiskalnym czas można zmienić tylko 1 raz po raporcie dobowym
      @note W trybie fiskalnym czas można zmienić maksymalnie o godzinę.
   */
  void setClock(const fp::Id &id, int year, int month, int day, int hour, int minute, int second);

  /// Żądanie odesłania czasu ustawionego w drukarce (#c).
  /**
      @note Metoda czeka na odpowiedź drukarki.
   */
  fp::ClockInfo getClock();

  /// Zmiana stawek PTU ($p).
  /**
      @param id Identyfikator kasy/kasjera
      @param count Ilość stawek <0;7> (jeśli 0, to programowane są 4 stawki, jeśli <0;6>, to stawka G domyślnie programowana jako zwolniona)
      @param a Nowa stawka PTU A
      @param b Nowa stawka PTU B
      @param c Nowa stawka PTU C
      @param d Nowa stawka PTU D
      @param e Nowa stawka PTU E
      @param f Nowa stawka PTU F
      @param g Nowa stawka PTU G

      @note W trybie szkoleniowym stawki są zapisywane do pamięci RAM.

      @note W przypadku błędu wykonania (błąd zapisu do EPROM) dalsza praca drukarki zostanie zawieszona,
            tzn. drukarka zezwala jedynie na wykonanie raportów okresowych (przechodzi w stan tylko do odczytu).

      @note Ta operacja wymaga potwierdzenia z klawiatury.

      @see VAT_DISABLED
      @see VAT_EXEMPTED
   */
  void setVatRates(const fp::Id &id, int count, float a, float b, float c, float d, float e, float f, float g);

  /// Programowanie nagłówka ($f).
  /**
      @param id Identyfikator kasy/kasjera
      @param header Nagłówek (maksymalnie 200 znaków)

      @note Nagłówek może zawierać znaki kontrolne CR i LF. Może chyba zwierać też jakieś inne,
            które służą np. do pogrubienia tekstu, zwiększenia czcionki itp. ale brak o nich w dokumentacji.

      @warning To chyba robi serwis podczas fiskalizacji?

      @warning Uważać, ten sam rozkaz z parametrem 1 służy do fiskalizacji drukarki.
   */
  void setHeader(const fp::Id &id, const std::string &header);

  /// Żądanie odesłania ustawionego nagłówka (^u).
  /**
      @note Zwracany nagłówek bez znaków kontrolnych.

      @note Metoda czeka na odpowiedź drukarki.
   */
  std::string getHeader();

  /// Otwarcie szuflady ($d).
  /**
      @warning Przetestować!
   */
  void openDrawer();

  /// Wysłanie napisu do wyświetlacza ($d).
  /**
      @param message Wiadomość wysłana do wyświetlacza

      @note Podczas wystawiania paragonu wyświetlacz jest obsługiwany automatycznie. W tym trybie
            nie należy wysyłać niczego do wyświetlacza.

      @warning Wyświetlacz operatora reaguje prawidłowo. Wyświetlacz klienta pokazuje za każdym
               razem ciąg znaków, który został wysłany po pierwszym raporcie dobowym. Pierwszy
               wysłany ciąg znaków zaraz po raporcie dobowym będzie również wydrukowany na następnym
               raporcie dobowym (z dokumentacji POSNET).
   */
  void setDisplayMessage(const std::string &message);

  /// Ustawienie trybu wyświetlacza ($d).
  /**
      @param mode Tryb wyświetlacza

      @note Nie widać, żeby te tryby się od siebie różniły.
   */
  void setDisplayMode(fp::DISPLAY_MODE mode);

  /// Ustawienie metody rozliczania rabatu ($r).
  /**
      @param mode Metoda/tryb rozliczania rabatu

      @note Proszę zobaczyć do dokumentacji w rozdziale 4. "Obliczenia wykonywane przez drukarkę
            fiskalną podczas drukowania paragonów".
  */
  void setDiscountAlgorithm(fp::DISCOUNT_ALGORITHM mode);

  /// Żądanie odesłania daty przeglądu okresowego (^t).
  /**
      @note Metoda czeka na odpowiedź drukarki.

      @todo Drukarka nie odpowiada. Sprawdzić to po fiskalizacji/ustawieniu daty przez serwis.
   */
  fp::ServiceDate getServiceCheckDate();

  /// Żądanie odesłania daty blokady serwisowej (^t).
  /**
      @note Metoda czeka na odpowiedź drukarki.

      @todo Drukarka nie odpowiada. Sprawdzić to po fiskalizacji/ustawieniu daty przez serwis.
   */
  fp::ServiceDate getServiceLockDate();

  /// Rozpoczęcie transakcji ($h).
  /**
      @param items Ilość pozycji (jeżeli 0, to paragon ON-LINE)
      @param extraLines Dodatkowe linie w stopce paragonu
      @param clientIdType Typ identyfikatora nabywcy
      @param clientId Identyfikator nabywcy (maksymalnie 13 znaków)

      @note Drukowanie linii dodatkowych jest uzależnione od konfiguracji drukarki. Drukarka
            może pobierać dane linii dodatkowych z rozkazu rozpoczęcia transakcji lub z
            rozkazu zakończenia transakcji, w zależności od ustawień.

      @note Wysłane dodatkowe linie po raporcie dobowym (pierwszy paragon po raporcie) są zapamiętywane.
            Jeśli podczas drukowania kolejnych paragonów zdefiniowano linie dodatkowe, to muszą
            one być identyczne, w przeciwnym razie zostaną zignorowane.

      @note W konfiguracji drukarki można włączyć traktowanie linii dodatkowej jako numer systemowy.
            Tak ustawiona linia (linia traktowana jako numer systemowy) może być na każdym paragonie inna.

      @note Nie udaje się wydrukować identyfikatora nabywcy. Drukarka nie zgłasza błędu, a na paragonie nie drukuje identyfikatora.
   */
  void beginTransaction(int items, const fp::ExtraLines &extraLines, fp::CLIENT_ID_TYPE clientIdType,
    const std::string &clientId);

  /// Drukuj linię paragonu ($l).
  /**
      @param item Linia paragonu

      @note Rozkaz musi być wykonany w trybie transakcji.

      @note Kod QR drukuje się jako tekst, drukarka nie rozpoznaje '@' na początku.
  */
  void printReceiptLine(const fp::Item &item);

  /// Obsługa kaucji w linii paragonu ($l).
  /**
      @param type Rodzaj kaucji
      @param nr Numer opakowania (z zakresu <0;9999>, czyli maksymalnie 4 znaki)
      @param quantity Ilość (argument opcjonalny, podobnie jak Item::quantity)
      @param price Cena opakowania

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void printDepositLine(fp::DEPOSIT_TYPE type, const std::string &nr, std::string quantity, float price);

  /// Kaucja pobrana (za sprzedaż opakowania) ($d).
  /**
      @param amount Kwota kaucji (za pobrane opakowania)
      @param nr Numer opakowania (opcjonalny, wartość z zakresu <1;127>, pominięty jeśli 0)
      @param quantity Ilość (argument opcjonalny, podobnie jak Item::quantity)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void depositCollected(float amount, int nr, std::string quantity);

  /// Storno kaucji pobranej ($d).
  /**
      @param amount Kwota kaucji (za pobrane opakowania)
      @param nr Numer opakowania (z zakresu <0;9999>, czyli maksymalnie 4 znaki)
      @param quantity Ilość (argument opcjonalny, podobnie jak Item::quantity)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void correctDepositCollected(float amount, int nr, std::string quantity);

  /// Kaucja wydana (za zwrot opakowania) ($d).
  /**
      @param amount Kwota kaucji (za pobrane opakowania)
      @param nr Numer opakowania (z zakresu <0;9999>, czyli maksymalnie 4 znaki)
      @param quantity Ilość (argument opcjonalny, podobnie jak Item::quantity)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void depositReturned(float amount, int nr, std::string quantity);

  /// Storno kaucji wydanej ($d).
  /**
      @param price Kwota kaucji (za pobrane opakowania)
      @param nr Numer opakowania (z zakresu <0;9999>, czyli maksymalnie 4 znaki)
      @param quantity Ilość (argument opcjonalny, podobnie jak Item::quantity)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void correctDepositReturned(float amount, int nr, std::string quantity);

  /// Anulowanie transakcji ($e).
  /**
      @param id Identyfikator kasy/kasjera
   */
  void cancelTransaction(const fp::Id &id);

  /// Standardowe zatwierdzenie transakcji ($e).
  /**
      @param id Identyfikator kasy/kasjera
      @param cashIn Kwota wpłacona przez klienta (jeżeli 0, to napisy 'wpłata/reszta' nie będą drukowane)
      @param total Łączna należność, dla kontroli poprzednich transferów
      @param discountType Rodzaj rabatu
      @param discountValue Wartość rabatu (opcjonalny rabat procentowy dla całej transakcji, w zakresie <0;99>)
      @param extraLines Linie dodatkowe

      @note Drukowanie linii dodatkowych jest uzależnione od konfiguracji drukarki. Drukarka
            może pobierać dane linii dodatkowych z rozkazu rozpoczęcia transakcji lub z
            rozkazu zakończenia transakcji, w zależności od ustawień.

      @note Wysłane dodatkowe linie po raporcie dobowym (pierwszy paragon po raporcie) są zapamiętywane.
            Jeśli podczas drukowania kolejnych paragonów zdefiniowano linie dodatkowe, to muszą
            one być identyczne, w przeciwnym razie zostaną zignorowane.

      @note W konfiguracji drukarki można włączyć traktowanie linii dodatkowej jako numer systemowy.
            Tak ustawiona linia (linia traktowana jako numer systemowy) może być na każdym paragonie inna.

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void confirmTransaction(const fp::Id &id, float cashIn, float total, fp::TRANSACTION_DISCOUNT_TYPE discountType,
    float discountValue, const fp::ExtraLines &extraLines);

  /// Zatwierdzenie transakcji z formami płatności (1) ($x).
  /**
      @param id Identyfikator kasy/kasjera
      @param info Dane form płatności
      @param total Łączna należność, dla kontroli poprzednich transferów
      @param discountType Rodzaj rabatu
      @param discountValue Wartość rabatu
      @param extraLines Linie dodatkowe

      @note Drukowanie linii dodatkowych jest uzależnione od konfiguracji drukarki. Drukarka
            może pobierać dane linii dodatkowych z rozkazu rozpoczęcia transakcji lub z
            rozkazu zakończenia transakcji, w zależności od ustawień.

      @note Wysłane dodatkowe linie po raporcie dobowym (pierwszy paragon po raporcie) są zapamiętywane.
            Jeśli podczas drukowania kolejnych paragonów zdefiniowano linie dodatkowe, to muszą
            one być identyczne, w przeciwnym razie zostaną zignorowane.

      @note W konfiguracji drukarki można włączyć traktowanie linii dodatkowej jako numer systemowy.
            Tak ustawiona linia (linia traktowana jako numer systemowy) może być na każdym paragonie inna.

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void confirmTransactionWithPaymentForms1(const fp::Id &id, const fp::PaymentFormsInfo1 &info, float total,
    fp::TRANSACTION_DISCOUNT_TYPE discountType, float discountValue, const fp::ExtraLines &extraLines);

  /// Zatwierdzenie transakcji z formami płatności (2) ($y).
  /**
      @param id Identyfikator kasy/kasjera
      @param info Dane form płatności
      @param total Łączna należność, dla kontroli poprzednich transferów
      @param discountType Rodzaj rabatu
      @param discountValue Wartość rabatu
      @param sysNr Numer systemowy
      @param summary Jeżeli możliwe w jednej grupie to drukuj skrócone podsumowanie.
      @param extraLines Linie dodatkowe

      @note Drukowanie linii dodatkowych jest uzależnione od konfiguracji drukarki. Drukarka
            może pobierać dane linii dodatkowych z rozkazu rozpoczęcia transakcji lub z
            rozkazu zakończenia transakcji, w zależności od ustawień.

      @note Wysłane dodatkowe linie po raporcie dobowym (pierwszy paragon po raporcie) są zapamiętywane.
            Jeśli podczas drukowania kolejnych paragonów zdefiniowano linie dodatkowe, to muszą
            one być identyczne, w przeciwnym razie zostaną zignorowane.

      @note W konfiguracji drukarki można włączyć traktowanie linii dodatkowej jako numer systemowy.
            Tak ustawiona linia (linia traktowana jako numer systemowy) może być na każdym paragonie inna.

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void confirmTransactionWithPaymentForms2(const fp::Id &id, const fp::PaymentFormsInfo2 &info,
    float total, DISCOUNT_TYPE discountType, float discountValue, std::string sysNr, bool summary,
    const fp::ExtraLines &extraLines);

  /// Obsługa form płatności ($b).
  /**
      @param serviceType Akcja
      @param paymentType Typ płatności
      @param amount Kwota formy płatności
      @param name Nazwa formy płatności (maksymalnie 15 znaków)
   */
  void paymentFormService(fp::SERVICE_TYPE serviceType, fp::PAYMENT_TYPE paymentType,
    float amount, const std::string &name);

  /// Rabat/narzut w trakcie transakcji ($n).
  /**
      @param type Rodzaj rabatu/narzutu
      @param name Nazwa rabatu (maksymalnie 20 znaków)
      @param discount Procent lub kwota

      @note Rozkaz powinien być wysłany _przed_ rozkazem FiscalPrinter::paymentFormService ($b)
            i _przed_ rozkazem FiscalPrinter::confirmTransactionWithPaymentForms2 ($y).
   */
  void addDiscount(fp::DISCOUNT_TYPE type, const std::string &name, float discount);

  /// Rabat/narzut do grupy towarów w danej stawce PTU ($L).
  /**
      @param vat Numer stawki PTU (z zakresu <0;6>)
      @param discountType Rodzaj rabatu/narzutu
      @param discountDescription Opis rabatu
      @param amount Wartość sprzedaży w danej stawce przed rabatem/narzutem
      @param discountValue Kwota lub procent rabatu/dopłaty
      @param discountName Nazwa rabatu (maksymalnie 20 znaków)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void addVatRateDiscount(int vat, fp::DISCOUNT_TYPE discountType, fp::DISCOUNT_DESCRIPTION_TYPE discountDescription,
    float amount, float discountValue, const std::string &discountName);

  /// Rabat/narzut do paragonu od podsumy ($Y).
  /**
      @param type Rodzaj rabatu/narzutu
      @param description Opis rabatu
      @param subtotal Wartość sprzedaży bez kaucji przed rabatem/narzutem
      @param discount Kwota lub procent rabatu/dopłaty
      @param name Nazwa rabatu (maksymalnie 20 znaków)

      @note Rozkaz musi być wykonany w trybie transakcji.
   */
  void addSubtotalDiscount(fp::DISCOUNT_TYPE type, fp::DISCOUNT_DESCRIPTION_TYPE description,
    float subtotal, float discount, std::string name);

  /// Dodatkowe linie na paragonie: zwrot opakowań ($z).
  /**
      @param name Numer opakowania (opcjonalny, z zakresu <0;65535>, czyli maksymalnie 5 znaków)
      @param quantity Napis zawierający ilość zwracanych opakowań (opcjonalny, maksymalnie 8 znaków)
      @param amount Kwota

      @note Kwota musi się zgadzać z kwotą dosłanych w trakcie transakcji opakowań.

      @note Rozkaz musi być wysłany _po_ rozkazie FiscalPrinter::confirmTransactionWithPaymentForms2 ($y).
   */
  void extraLineContainerReturned(std::string name, const std::string &quantity, float amount);

  /// Dodatkowe linie na paragonie: przyjęcie opakowań ($z).
  /**
      @param name Numer opakowania (opcjonalny, z zakresu <0;65535>, czyli maksymalnie 5 znaków)
      @param quantity Napis zawierający ilość zwracanych opakowań (opcjonalny, maksymalnie 8 znaków)
      @param amount Kwota

      @note Kwota musi się zgadzać z kwotą dosłanych w trakcie transakcji opakowań.

      @note Rozkaz musi być wysłany _po_ rozkazie FiscalPrinter::confirmTransactionWithPaymentForms2 ($y).
   */
  void extraLineContainerReceived(std::string name, const std::string &quantity, float amount);

  /// Rozliczenie form płatności ($z).
  /**
      @note Drukowane są formy płatności z danych zgromadzonych podczas transakcji, jak również kwota reszty.

      @note W sekwencji FiscalPrinter::confirmTransactionWithPaymentForms2 ($y) nie należy przesyłać form płatności.

      @note Metoda musi być wywołana _po_ fp::FiscalPrinter::confirmTransactionWithPaymentForms2 ($y)

      @note Należy podczas transakcji wysłać formy płatności za pomocą fp::FiscalPrinter::paymentFormService ($b)
   */
  void formsOfPaymentClearing();

  /// Linie dodatkowych stopek paragonu ($z).
  /**
      @param type Rodzaj stopki
      @param text Tekst (maksymalnie 25 znaków)

      @note Na jednym paragonie można wydrukować do 20 dodatkowych linii

      @note Rozkaz musi być wysłany _po_ fp::FiscalPrinter::confirmTransactionWithPaymentForms2 ($y)
   */
  void extraLine(fp::EXTRA_LINE_TYPE type, const std::string &text);

  /// Definicja linii informacyjnych ($z).
  /**
      Drukowane są linie typu 'Zapraszamy do ponownych odwiedzin'.

      @param lines Linie (każda linia może mieć maksymalnie 48 znaków)

      @note Możliwe jest zdefiniowanie tylko jednego zestawu linii informacyjnych pomiędzy raportami dobowymi.

      @note Tak zdefiniowane linie informacyjne są drukowane po odebraniu rozkazu fp::FiscalPrinter::finish ($z) kończącego wydruk.

      @note Rozkaz musi być wysłany _po_ fp::FiscalPrinter::confirmTransactionWithPaymentForms2 ($y)
   */
  void defineInfoLines(const fp::ExtraLines &lines);

  /// Płatność w EURO ($z).
  /**
      @param exchange Kurs EURO
      @param amount Wartość paragonu w EURO
      @param cashIn Wpłata w EURO
      @param checkEuro Reszta w EURO
      @param checkPln Reszta w PLN

      @note Rozkaz wysyłany jest w trybie transakcji.

      @note Rozkaz musi być wysłany _przed_ fp::FiscalPrinter::confirmTransactionWithPaymentForms2 ($y)
   */
  void euroPayment(float exchange, float amount, float cashIn, float checkEuro, float checkPln);

  /// Koniec wydruku ($z).
  /**
      @param nextHeader Wydrukowanie nagłówka nowego paragonu

      @note Rozkaz musi być wysłany _po_ fp::FiscalPrinter::confirmTransactionWithPaymentForms2 ($y)
   */
  void finish(bool nextHeader);

  /// Rozpoczęcie faktury ($h).
  /**
      @param data Dane rozpoczęcia faktury

      @note Wygląda na to, że nazwa klienta i nazwa sprzedawcy są ignorowane.
   */
  void beginInvoice(const fp::BeginInvoiceData &data);

  /// Zakończenie faktury ($e).
  /**
      @param id Identyfikator kasy/kasjera
      @param data Dane zakończenia faktury

      @note Operacja może wymagać potwierdzenia z klawiatury.

      @note Wygląda na to, że nazwa klienta i nazwa sprzedawcy są ignorowane.
   */
  void finishInvoice(const fp::Id &id, const fp::FinishInvoiceData &data);

  /// Rozkaz ustawienia opcji faktury (@c).
  /**
      @param options Opcje faktury
   */
  void setInvoiceOption(const fp::InvoiceOptions &options);

  /// Linie dodatkowe stopek faktury ($z).
  /**
      @param type Rodzaj stopki
      @param text Napis (maksymalnie 25 znaków)

      @note Na jednym paragonie można wydrukować 20 dodatkowych linii.

      @note Rozkaz musi być wysłany po rozkazie $y.
   */
  void extraLinesInvoice(fp::EXTRA_LINE_TYPE type, const std::string &text);

  /// Zalogowanie kasjera (#p).
  /**
      @param id Identyfikator kasy/kasjera
   */
  void login(const fp::Id &id);

  /// Wylogowanie kasjera (#q).
  /**
      @param id Identyfikator kasy/kasjera
   */
  void logout(const fp::Id &id);

  /// Wpłata do kasy (#i).
  /**
      @param id Identyfikator kasy/kasjera
      @param cashIn
      @param euro Wpłata w walucie EURO
   */
  void paymentToCash(const fp::Id &id, float cashIn, bool euro);

  /// Wypłata z kasy (#d).
  /**
      @param id Identyfikator kasy/kasjera
      @param cashOut
      @param euro Wypłata w walucie EURO
   */
  void withdrawalFromCash(const fp::Id &id, float cashOut, bool euro);

  /// Drukowanie stanu kasy (#t).
  /**
      @param id Identyfikator kasy/kasjera
   */
  void printCashState(const fp::Id &id);

  /// Drukowanie raportu kasy/zmiany (#k).
  /**
      @param id Identyfikator kasy/kasjera (w tym przypadku _zawsze_ trzeba podać nazwę kasjera, numer kasymożna pominąć, jeżeli wcześniej było logowanie)
      @param reset Raport zerujący
      @param shift Nazwa zmiany (maksymalnie 8 znaków)
   */
  void printShiftReport(const fp::Id &id, bool reset, const std::string &shift);

  /// Drukowanie raportu dobowego (#r).
  /**
      @param id Identyfikator kasy/kasjera

      @note Operacja wymaga potwierdzenia z klawiatury.
   */
  void printDailyReport(const fp::Id &id);

  /// Drukowanie raportu okresowego (zakres określony za pomocą dat) (#o).
  /**
     @param id Identyfikator kasy/kasjera
     @param fromYear Data początkowa: rok
     @param fromMonth Data początkowa: miesiąc
     @param fromDay Data początkowa: dzień
     @param toYear Data końcowa: rok
     @param toMonth Data końcowa: miesiąc
     @param toDay Data końcowa: dzień
     @param type Rodzaj raportu
   */
  void printPeriodicalReportByDate(const fp::Id &id, int fromYear, int fromMonth, int fromDay,
    int toYear, int toMonth, int toDay, fp::PERIODICAL_REPORT_TYPE type);

  /// Drukowanie raportu okresowego (zakres określony za pomocą numerów) (#o).
  /**
      @param id Identyfikator kasy/kasjera
      @param fromNr
      @param toNr
      @param type Rodzaj raportu
   */
  void printPeriodicalReportByNumber(const fp::Id &id, long fromNr, long toNr, fp::PERIODICAL_REPORT_TYPE type);

  /// Zwrot opakowania (#w).
  /**
      @param text Napis zawierający nazwę i wartość zwracanego opakowania (maksymalnie 20 znaków)
   */
  void containerReturn(const std::string &text);

  /// Pokwitowanie sprzedaży (#g).
  /**
      @param id Identyfikator kasy/kasjera (trzeba podać, nawet jeśli było logowanie kasjera)
      @param data Dane pokwitowania sprzedaży
   */
  void saleReceipt(const fp::Id &id, const fp::SaleReceiptData &data);

  /// Transakcja uznania (zwrot towaru) (#h).
  /**
      @param id Identyfikator kasy/kasjera (trzeba podać, nawet jeśli było logowanie kasjera)
      @param data Dane transakcji uznania
   */
  void returnOfArticle(const fp::Id &id, const fp::SaleReceiptData &data);

  /// Wydruk niefiskalny: rozpoczęcie wydruku ($w).
  /**
      @param printNr Numer wydruku (z dokumentacji)
      @param headerNr Numer nagłówka (z dokumentacji)

      @note Lista dostępnych wydruków, nagłówków oraz numery linii znajdują się w dokumentacji.
   */
  void beginNonFiscal(int printNr, int headerNr);

  /// Wydruk niefiskalny: linia wydruku ($w).
  /**
      @param line Dane linii

      @note Lista dostępnych wydruków, nagłówków oraz numery linii znajdują się w dokumentacji.
   */
  void printNonFiscal(const fp::NonFiscalLine &line);

  /// Wydruk niefiskalny: zakończenie wydruku ($w)
  /**
      @param printNr Numer wydruku (z dokumentacji)
      @param sysNr Numer systemowy
      @param extraLines Dodatkowe linie

      @note Lista dostępnych wydruków, nagłówków oraz numery linii znajdują się w dokumentacji.
   */
  void finishNonFiscal(int printNr, const std::string &sysNr, const fp::ExtraLines &extraLines);

  /// Identyfikator nabywcy ($z).
  /**
      @param clientIdType Typ identyfikatora nabywcy
      @param clientId Numer identyfikacyjny nabywcy (maksymalnie 13 znaków)

      @note Rozkaz należy wysłać _po_ rozpoczęciu lub _przed_ zakończeniem paragonu.

      @note Wygląda na to, że rozkaz jest ignorowany.
   */
  void setClientId(fp::CLIENT_ID_TYPE clientIdType, const std::string &clientId);

  /// Drukowanie raportu deskryptorów (@d).
  /**
      @warning Ta operacja zużywa _bardzo_ _dużo_ papieru.
   */
  void descriptorsReport();

  /// Wysuw papieru (#l).
  /**
      @param lines Ilość linii (maksymalnie 20)
   */
  void paperFeed(int lines);

  /// Wygenerowanie błędu.
  /**
      Do drukarki zostaje wysłane _błędne_ polecenie.
   */
  void debugGenerateError();

private:

  std::string fromFloat(float number);
  std::string fromInt(int number);
  std::string fromLong(long number);

  std::string toMazovia(const std::string &text);

  void execute(const std::vector<int> &intParams, const std::string &cmd, const std::vector<std::string> &stringParams, bool appendCtrlByte);
  void execute(const std::string &cmd, bool appendCtrlByte);
  void execute(const std::vector<int> &intParams, const std::string &cmd, bool appendCtrlByte);
  void execute(const std::string &cmd, const std::vector<std::string> &stringParams, bool appendCtrlByte);

  std::string calculateCtrlByte(const std::string &str);

  std::string read();
  char readOneByte();

  void write(const std::string &str);
  void writeOneByte(char c);

  boost::asio::io_service *io;
  boost::asio::serial_port *port;

}; // class FiscalPrinter


} // namespace fp


#endif // __FP_FISCALPRINTER_HPP__
