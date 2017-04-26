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


#ifndef __FP_COMMON_HPP__
#define __FP_COMMON_HPP__


#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <exception>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_common.hpp>
#include <boost/spirit/include/classic_core.hpp>


namespace fp
{

const float VAT_DISABLED = 101.0; ///< Wartość nieużywanej stawki VAT (POSNET).
const float VAT_EXEMPTED = 100.0; ///< Wartość zwolnionej stawki VAT (POSNET).


/// Tryb obsługi błędów.
/**
    @note Domyślnie po włączeniu zasilania drukarka jest w trybie EHM_0.
 */
enum ERROR_HANDLING_MODE
{
  EHM_0 = 0, ///< Wyświetlenie komunikatu na drukarce, kontynuowanie możliwe tylko po wciśnięciu 'OK'
  EHM_1 = 1, ///< Wystąpienie błędu nie jest sygnalizowane, należy sprawdzać status 'ręcznie'

  EHM_2 = 2, ///< Tak jak EHM_0, przy czym kod błędu jest automatycznie przesyłany do interfejsu
  EHM_3 = 3  ///< Tak jak EHM_1, przy czym kod błędu jest automatycznie przesyłany do interfejsu

}; // enum ERROR_HANDLING_MODE


/// Tryb wyświetlacza.
/**
    @note Nie widać, żeby te tryby się od siebie różniły.
 */
enum DISPLAY_MODE
{
  DM_3 = 3, ///< Data i czas
  DM_4 = 4  ///< Gotówka

}; // enum DISPLAY_MODE


/// Typ identyfikatora nabywcy.
enum CLIENT_ID_TYPE
{
  CIDT_NONE  = 0, ///< Brak identyfikatora nabywcy.

  CIDT_NIP   = 1, ///< NIP.
  CIDT_REGON = 2, ///< REGON.
  CIDT_PESEL = 3  ///< PESEL.

}; // enum CLIENT_ID_TYPE


/// Metoda rozliczania rabatu.
/**
    @note Proszę zobaczyć do dokumentacji w rozdziale 4. "Obliczenia wykonywane przez drukarkę
          fiskalną podczas drukowania paragonów".
 */
enum DISCOUNT_ALGORITHM
{
  DA_0 = 0, ///< Pierwsza metoda.
  DA_1 = 1  ///< Druga metoda.

}; // enum DISCOUNT_ALGORITHM


/// Typ rabatu/narzutu.
enum DISCOUNT_TYPE
{
  DT_0 = 0, ///< Brak.

  DT_1 = 1, ///< Rabat procentowy.
  DT_2 = 2, ///< Narzut procentowy.

  DT_3 = 3, ///< Rabat kwotowy.
  DT_4 = 4  ///< Narzut kwotowy.

}; // enum DISCOUNT_TYPE


/// Typ rabatu/narzutu.
enum ITEM_DISCOUNT_TYPE
{
  IDT_0 = 0, ///< Brak.

  IDT_1 = 1, ///< Rabat kwotowy.
  IDT_2 = 2, ///< Rabat procentowy.

  IDT_3 = 3, ///< Dopłata kwotowa.
  IDT_4 = 4  ///< Dopłata procentowa.

}; // enum ITEM_DISCOUNT_TYPE


/// Typ rabatu/narzutu dla całej transakcji.
enum TRANSACTION_DISCOUNT_TYPE
{
  TDT_0 = 0, ///< Brak rabatu.

  TDT_1 = 1, ///< Rabat procentowy.

  TDT_2 = 2  ///< Dopłata procentowa.

}; // enum TRANSACTION_DISCOUNT_TYPE


/// Opis rabatu.
enum DISCOUNT_DESCRIPTION_TYPE
{
  DDT_0  = 0,  ///< Brak.

  DDT_1  = 1,  ///< Specjalny.
  DDT_2  = 2,  ///< Okolicznościowy.
  DDT_3  = 3,  ///< Okazja.
  DDT_4  = 4,  ///< Świąteczny.
  DDT_5  = 5,  ///< Stały klient.
  DDT_6  = 6,  ///< Jubileuszowy.
  DDT_7  = 7,  ///< Urodzinowy.
  DDT_8  = 8,  ///< Dla pracownika.
  DDT_9  = 9,  ///< Promocja.
  DDT_10 = 10, ///< Nagroda.
  DDT_11 = 11, ///< Wyprzedaż.
  DDT_12 = 12, ///< Przecena.
  DDT_13 = 13, ///< Sezonowy.
  DDT_14 = 14, ///< Nocny.
  DDT_15 = 15, ///< Obsługa.

  DDT_16 = 16  ///< Definiowany przez użytkownika.

}; // enum DISCOUNT_DESCRIPTION_TYPE


/// Typ kaucji.
enum DEPOSIT_TYPE
{
  DT_6  = 6,  ///< Kaucja pobrana.
  DT_7  = 7,  ///< Storno kaucji pobranej.

  DT_10 = 10, ///< Kaucja wydana.
  DT_11 = 11  ///< Storno kaucji wydanej.

}; // enum DEPOSIT_TYPE


enum SERVICE_TYPE
{
  ST_1 = 1, ///< Kwota powoduje zarejestrowanie kwoty płatności, zwiększenie sumy dokonanych płatności w danym typie płatności.
  ST_2 = 2  ///< Kwota powoduje anulowanie kwoty płatności, zmniejszenie sumy dokonanych płatności w danym typie płatności.

}; // enum SERVICE_TYPE


enum PAYMENT_TYPE
{
  PT_0 = 0, ///< Gotówka.
  PT_1 = 1, ///< Karta.
  PT_2 = 2, ///< Czek.
  PT_3 = 3, ///< Bon.
  PT_4 = 4, ///< Inna.
  PT_5 = 5, ///< Kredyt.
  PT_6 = 6, ///< Konto klienta.
  PT_7 = 7, ///< Waluta (NOVITUS), voucher (POSNET).
  PT_8 = 8  ///< Przelew.

}; // enum PAYMENT_TYPE


/// Typ dodatkowych linii.
enum EXTRA_LINE_TYPE
{
  ELT_0   = 0,   ///< Numer transakcji.
  ELT_1   = 1,   ///< Punkty.
  ELT_2   = 2,   ///< Suma punktów.
  ELT_3   = 3,   ///< Numer rejestracyjny.
  ELT_4   = 4,   ///< Nazwisko.
  ELT_5   = 5,   ///< Karta.
  ELT_6   = 6,   ///< Numer karty.
  ELT_7   = 7,   ///< Ważna do.
  ELT_8   = 8,   ///< Kasjer.
  ELT_9   = 9,   ///< Nazwisko kasjera.
  ELT_10  = 10,  ///< Zaliczka.
  ELT_11  = 11,  ///< Waluta.
  ELT_12  = 12,  ///< Przelicznik.
  ELT_13  = 13,  ///< Numer zamówienia.
  ELT_14  = 14,  ///< Numer pracownika.
  ELT_15  = 15,  ///< Nazwisko pracownika.
  ELT_16  = 16,  ///< Konto przed transakcją.
  ELT_17  = 17,  ///< Przyznano.
  ELT_18  = 18,  ///< Wykorzystano.
  ELT_19  = 19,  ///< Konto po transakcji.
  ELT_20  = 20,  ///< Klient stały.
  ELT_21  = 21,  ///< Voucher.
  ELT_22  = 22,  ///< Wartość Voucher.
  ELT_23  = 23,  ///< Zapłata Voucher.
  ELT_24  = 24,  ///< Napis predefiniowany w pamięci RAM.
  ELT_25  = 25,  ///< Linia bez słowa kluczowego (15 spacji).

  // obsługiwane przez drukarki Deon.20, Bono, Delio E 3.10, Delio Apteka E 3.10

  ELT_26  = 26,  ///< Ilość sprzedanych towarów.
  ELT_27  = 27,  ///< Numer pracownika.
  ELT_28  = 28,  ///< Numer klienta.
  ELT_29  = 29,  ///< Udzielono łącznie rabatów.
  ELT_30  = 30,  ///< Numer.
  ELT_31  = 31,  ///< Kod.
  ELT_32  = 32,  ///< Nazwa.
  ELT_33  = 33,  ///< Opis.
  ELT_34  = 34,  ///< Liczba.
  ELT_35  = 35,  ///< Klient.
  ELT_36  = 36,  ///< Kwota.

  // obsługiwane przez drukarki Vega Mobil, Delio E 3.10

  ELT_80  = 80,  ///< Bilet numer.
  ELT_81  = 81,  ///< Kasa wydania.
  ELT_82  = 82,  ///< Od.
  ELT_83  = 83,  ///< Do.
  ELT_84  = 84,  ///< Przez.
  ELT_85  = 85,  ///< Klasa.
  ELT_86  = 86,  ///< Wyjazd w dniu.
  ELT_87  = 87,  ///< Powrót.
  ELT_88  = 88,  ///< Czas ważności.
  ELT_89  = 89,  ///< Ważny do.
  ELT_90  = 90,  ///< Ważny na poc.
  ELT_91  = 91,  ///< TAM ważny do.
  ELT_92  = 92,  ///< POW. ważny do.
  ELT_93  = 93,  ///< Km.
  ELT_94  = 94,  ///< Rodzaj biletu.
  ELT_95  = 95,  ///< Podstawa.
  ELT_96  = 96,  ///< Ulga.
  ELT_97  = 97,  ///< Liczba osób.
  ELT_98  = 98,  ///< Taryfa.
  ELT_99  = 99,  ///< PORT DOCELOWY.
  ELT_100 = 100, ///< PORT TRANZYTOWY.
  ELT_101 = 101, ///< NR KARTY POKŁADOWEJ.
  ELT_102 = 102, ///< NR LOTU.
  ELT_103 = 103, ///< NR BILETU.

  // obsługiwane przez drukarki apteczne

  ELT_120 = 120, ///< Bonifikata.
  ELT_121 = 121, ///< Do zapłaty po bonifikacie.
  ELT_122 = 122, ///< Klient.
  ELT_123 = 123, ///< Bonus.
  ELT_124 = 124, ///< Premia.

  // obsługiwane przez drukarki z obsługą faktur

  ELT_130 = 130, ///< Akcyza.
  ELT_131 = 131, ///< Informacja o fakturze.
  ELT_132 = 132, ///< Nr konta.
  ELT_133 = 133, ///< Nazwa banku.
  ELT_134 = 134, ///< Dodano punktów Vita.

  // obsługiwane przez drukarki Delio E 4.00

  ELT_140 = 140, ///< NIP.
  ELT_141 = 141, ///< REGON.
  ELT_142 = 142, ///< PESEL.
  ELT_143 = 143, ///< Program.
  ELT_144 = 144, ///< Klient.
  ELT_145 = 145, ///< Przystąp.
  ELT_146 = 146, ///< KLUB.
  ELT_147 = 147, ///< Pusta linia.
  ELT_148 = 148, ///< PKT.
  ELT_149 = 149, ///< KOD.
  ELT_150 = 150, ///< Program nr.
  ELT_151 = 151, ///< Bilet.
  ELT_152 = 152, ///< Myjnia.
  ELT_153 = 153, ///< WWW.
  ELT_154 = 154, ///< NIP Nabywcy.
  ELT_155 = 155, ///< REGON Nabywcy.
  ELT_156 = 156, ///< PESEL Nabywcy.
  ELT_157 = 157, ///< CLUB.
  ELT_158 = 158, ///< Numer karty VITAY.
  ELT_159 = 159  ///< Dodano punktów.

}; // enum EXTRA_LINE_TYPE


enum CASH_REGISTER_INFO_2_MODE
{
  CRI2M_22 = 22, ///< Odesłane informacje o totalizerach dotyczą bieżącego (otwartego) paragonu.
  CRI2M_23 = 23, ///< Odesłane informacje o totalizerach zawierają sumaryczne wartości od ostatniego raportu dobowego.
  CRI2M_99 = 99  ///< W polu gotówka odsyłana jest informacja o gotówce w walucie EURO, a nie w PLN.

}; // enum CASH_REGISTER_INFO_2_MODE


enum CASH_REGISTER_INFO_6_MODE
{
  CRI6M_0 = 0, ///< Odsyłane kwoty brutto.
  CRI6M_1 = 1, ///< Odsyłane kwoty netto.
  CRI6M_2 = 2  ///< Odsyłane kwoty VAT.

}; // enum CASH_REGISTER_INFO_6_MODE


enum CASH_REGISTER_INFO_7_MODE
{
  CRI7M_0 = 0, ///< Wartość brutto.
  CRI7M_1 = 1, ///< Wartość netto.
  CRI7M_2 = 2  ///< Wartość VAT.

}; // enum CASH_REGISTER_INFO_7_MODE


/// Typ raportu okresowego.
enum PERIODICAL_REPORT_TYPE
{
  PRT_0  = 0,  ///< Raport pełny (dokument fiskalny) z zakresu podanych dat.
  PRT_1  = 1,  ///< Podsumowanie sprzedaży (dokument niefiskalny) z zakresu podanych dat.
  PRT_6  = 6,  ///< Raport miesięczny pełny (dokument fiskalny).
  PRT_7  = 7,  ///< Miesięczne podsumowanie sprzedaży (dokument niefiskalny).
  PRT_96 = 96, ///< Raport rozliczeniowy pełny z zakresu podanych dat.
  PRT_97 = 97  ///< Podsumowanie raportu rozliczeniowego z zakresu podanych dat.

}; // enum PERIODICAL_REPORT_TYPE


enum CLIENT_SELLER_OPTION
{
  CSO_0 = 0, ///< Drukowany jest tylko blok informacyjny.
  CSO_1 = 1, ///< Jest drukowana nazwa kupującego/sprzedającego oraz blok informacyjny.
  CSO_2 = 2  ///< Brak informacji o kupującym/sprzedającym.

}; // enum CLIENT_SELLER_OPTION


enum INVOICE_SUMMARY_OPTION
{
  ISO_01 =  1, ///< Pominąć opis słowny kwoty do zapłaty.
  ISO_02 =  2, ///< Pominąć blok kwot brutto w rozliczeniu podatku.
  ISO_04 =  4, ///< Pogrubiona etykieta 'Nabywca'.
  ISO_08 =  8, ///< Pogrubiona etykieta 'Sprzedawca'.
  ISO_10 = 16, ///< Pogrubiony numer NIP nabywcy.
  ISO_20 = 32  ///< Pogrubiony numer NIP sprzedawcy.

}; // enum INVOICE_SUMMARY_OPTION


enum INVOICE_OPTION_2
{
  IO2_01 =   1, ///< Wydruk etykiety 'opis/symbol' w nagłówku faktury przed pozycjami faktury.
  IO2_02 =   2, ///< Wydru numeru pozycji w pozycji faktury.
  IO2_04 =   4, ///< Etykieta 'Do zapłaty' przed blokiem rozliczenia podatków.
  IO2_08 =   8, ///< Wydruk ilości groszy w postaci słownej.
  IO2_10 =  16, ///< Nie drukuj daty sprzedaży, jeżeli jest taka sama jak data wystawienia faktury.
  IO2_20 =  32, ///< Nie drukuj danych sprzedawcy.
  IO2_40 =  64, ///< Nie drukuj opisów do pozycji faktury (informacji opisujących zawartość pól w pozycji faktury).
  IO2_80 = 128  ///< Włączona obsługa płatności (tak jak w paragonie).

}; // enum INVOICE_OPTION_2


enum INVOICE_OPTION_3
{
  IO3_01 =  1, ///< Nie drukuj danych odbiorcy (nie obejmuje pola z numerem NIP/REGON/PESEL).
  IO3_02 =  2, ///< Drukuj napis 'Zapłacono gotówką'.
  IO3_04 =  4, ///< Pomiń etykietę 'Sprzedawca'.
  IO3_08 =  8, ///< Pomiń etykietę 'ORYGINAŁ' (działa tylko w przypadku braku kopii do wydrukowania).
  IO3_10 = 16  ///< Drukuj etykietę 'FAKTURA VAT' zamiast etykiety 'FAKTURA'.

}; // enum INVOICE_OPTION_3


enum SALE_RECEIPT_OPTION
{
  SRO_0 = 0, ///< Drukuj 2 kopie, między jadną a drugą trzeba nacisnąć klawisz.
  SRO_1 = 1, ///< Drukuj 2 kopie, jedna po drugiej (użyteczne w wersji z obcinaczem 'nie docinającym').
  SRO_2 = 2  ///< Drukuj 1 kopię.

}; // enum SALE_RECEIPT_OPTION


enum FONT_ATTRS
{
  FA_0 = 0, ///< Domyślne ustawienia.

  FA_1 = 1, ///< Czcionka powiększona, zmniejszona ilość znaków w linii o połowę.
  FA_2 = 2, ///< Czcionka podwyższona.
  FA_3 = 3, ///< Czcionka poszerzona, zmniejszona ilość znaków w linii o połowę.
  FA_4 = 4  ///< Inwers.

}; // enum FONT_ATTRS


/// Status ENQ.
struct EnqStatus
{
  bool fiscal;        ///< Drukarka jest w trybie fiskalnym.
  bool command;       ///< Ostatni rozkaz został wykonany poprawnie.
  bool transaction;   ///< Drukarka jest w trybie transakcji.
  bool transactionOk; ///< Ostatnia transakcja została zakończona poprawnie (flaga przechowywana w pamięći nieulotnej).

}; // struct EnqStatus


/// Status DLE.
struct DleStatus
{
  bool online; ///< Drukarka jest w stanie 'on-line'.
  bool paper;  ///< Brak papieru lub rozładowany akumulator.
  bool error;  ///< Błąd mechanizmu/sterownika.

}; // struct DleStatus


/// Kod błędu drukarki.
struct PrinterError
{
  PrinterError() : code(0) {}
  PrinterError(int c) : code(c) {}

  int code;

  std::string toString() const
  {
    return PrinterError::toString(code);
  }

  static std::string toString(int c)
  {
    switch (c)
    {

    case 0:    return std::string("Operacja wykonana pomyślnie"); // 0 oznacza brak błędu

    case 1:    return std::string("Nie zainicjowany zegar RTC");
    case 2:    return std::string("Nieprawidłowy bajt kontrolny");
    case 3:    return std::string("Nieprawidłowa ilość parametrów");
    case 4:    return std::string("Nieprawidłowy parametr");
    case 5:    return std::string("Błąd operacji z zegarem RTC");
    case 6:    return std::string("Błąd operacji z modułem fiskalnym");
    case 7:    return std::string("Nieprawidłowa data");
    case 8:    return std::string("Błąd operacji - niezerowe totalizery");
    case 9:    return std::string("Błąd operacji wejścia/wyjścia");
    case 10:   return std::string("Przekroczony zakres danych");
    case 11:   return std::string("Nieprawidłowa ilość stawek PTU");
    case 12:   return std::string("Nieprawidłowy nagłówek");
    case 13:   return std::string("Nie można refiskalizować urządzenia");
    case 14:   return std::string("Nie można zapisać nagłówka");
    case 15:   return std::string("Nieprawidłowe linie dodatkowe");
    case 16:   return std::string("Nieprawidłowa nazwa towaru");
    case 17:   return std::string("Nieprawidłowa ilość");
    case 18:   return std::string("Nieprawidłowa stawka PTU towaru");
    case 19:   return std::string("Nieprawidłowa cena towaru");
    case 20:   return std::string("Nieprawidłowa wartość towaru");
    case 21:   return std::string("Paragon nie został rozpoczęty");
    case 22:   return std::string("Błąd operacji storno");
    case 23:   return std::string("Nieprawidłowa ilość linii paragonu");
    case 24:   return std::string("Przepełnienie bufora wydruku");
    case 25:   return std::string("Nieprawidłowy tekst lub nazwa kasjera");
    case 26:   return std::string("Nieprawidłowa wartość płatności");
    case 27:   return std::string("Nieprawidłowa wartość całkowita");
    case 28:   return std::string("Przepełnienie totalizera sprzedaży");
    case 29:   return std::string("Próba zakończenia nie rozpoczętego paragonu");
    case 30:   return std::string("Nieprawidłowa wartość płatności 2");
    case 31:   return std::string("Przepełnienie stanu kasy");
    case 32:   return std::string("Ujemny stan kasy został zastąpiony zerowym");
    case 33:   return std::string("Nieprawidłowy tekst zmiany");
    case 34:   return std::string("Nieprawidłowa wartość lub tekst");
    case 35:   return std::string("Zerowe totalizery sprzedaży");
    case 36:   return std::string("Rekord już istnieje");
    case 37:   return std::string("Anulowane przez użytkownika");
    case 38:   return std::string("Nieprawidłowa nazwa");
    case 39:   return std::string("Nieprawidłowy symbol stawki VAT");
    case 40:   return std::string("Nie zaprogramowany nagłówek");
    case 41:   return std::string("Nieprawidłowy numer kasy");
    case 42:   return std::string("Nieprawidłowy numer kasjera");
    case 43:   return std::string("Nieprawidłowy numer paragonu");
    case 44:   return std::string("Nieprawidłowa nazwa klienta");
    case 45:   return std::string("Nieprawidłowy terminal");
    case 46:   return std::string("Nieprawidłowa nazwa karty kredytowej");
    case 47:   return std::string("Nieprawidłowy numer karty kredytowej");
    case 48:   return std::string("Nieprawidłowy miesiąc ważności karty kredytowej");
    case 49:   return std::string("Nieprawidłowy rok ważności karty kredytowej");
    case 50:   return std::string("Nieprawidłowy kod autoryzacyjny");
    case 51:   return std::string("Nieprawidłowa kwota");

    // błędy od 52 do 57 występują tylko w drukarkach aptecznych

    case 52:   return std::string("Niepusta tablica wycen");
    case 53:   return std::string("Wartość niezgodna z wyceną");
    case 54:   return std::string("Brak wyceny leku");
    case 55:   return std::string("Brak opisu leku");
    case 56:   return std::string("Błąd kwoty OPŁATA");
    case 57:   return std::string("Przepełnienie tablicy wycen");

    case 58:   return std::string("Paragon offline pełny");

    // zakres od 59 do 81 zarezerwowany (aktualnie nie używany)

    case 82:   return std::string("Niedozwolony rozkaz");
    case 83:   return std::string("Zła wartość kaucji");
    case 84:   return std::string("Przekroczona liczba wysłanych napisów");

    // zakres od 85 do 999 zarezerwowany (aktualnie nie używany)

    case 1000: return std::string("Błąd inicjalizacji");
    case 1001: return std::string("Uszkodzenie pamięci RAM");
    case 1002: return std::string("Paragon już jest rozpoczęty");
    case 1003: return std::string("Brak identyfikatora stawki PTU");
    case 1004: return std::string("Nieprawidłowy rabat");
    case 1005: return std::string("Nieprawidłowe dane");
    case 1006: return std::string("Drukarka nie jest w trybie fiskalnym");
    case 1007: return std::string("Nie zaprogramowane stawki PTU");
    case 1008: return std::string("Pamięć fiskalna pełna");
    case 1009: return std::string("Nieprawidłowa suma kontrolna pamięci RAM");
    case 1010: return std::string("Nieprawidłowa suma kontrolna bazy danych");
    case 1011: return std::string("Nieprawidłowa suma kontrolna nagłówka");
    case 1012: return std::string("Nieprawidłowa suma kontrolna nazwy kasjera");
    case 1013: return std::string("Nieprawidłowa suma kontrolna numeru kasy");
    case 1014: return std::string("Nie powiodło się uaktualnienie danych");
    case 1015: return std::string("Nie zaprogramowany numer unikatowy");
    case 1016: return std::string("Brak pamięci fiskalnej");
    case 1017: return std::string("Brak mechanizmu drukującego");
    case 1018: return std::string("Brak wyświetlacza");
    case 1019: return std::string("Pamięć fiskalna została wymieniona");
    case 1020: return std::string("Identyczne stawki PTU są już zaprogramowane");
    case 1021: return std::string("Urządzenie jest w trybie tylko do odczytu");
    case 1022: return std::string("Nierozpoznany rozkaz");
    case 1023: return std::string("Nieprawidłowy rozkaz");
    case 1024: return std::string("Nieprawidłowy zakres raportu");
    case 1025: return std::string("Brak danych raportu w podanym zakresie");
    case 1026: return std::string("Przepełnienie bufora transmisji");
    case 1027: return std::string("Niezakończony tryb fiskalny");
    case 1028: return std::string("Uszkodzenie pamięci fiskalnej");
    case 1029: return std::string("Przekroczony limit ograniczeń pamięci fiskalnej");
    case 1030: return std::string("Uszkodzona mapa pamięci fiskalnej");
    case 1031: return std::string("Rozkaz wysłany w niewłaściwym trybie");
    case 1032: return std::string("Nieprawidłowy wskaźnik ramki");
    case 1033: return std::string("Pamięć fiskalna jest zajęta");
    case 1034: return std::string("Drukarka fiskalna jest zajęta");

    // zakres od 1035 do 1036 zarezerwowany (aktualnie nie używany)

    case 1037: return std::string("Brak papieru");
    case 1038: return std::string("Błąd zapisu kopii elektronicznej");
    case 1039: return std::string("Błąd instalacji karty pamięci");
    case 1040: return std::string("Karta pamięci została zamknięta");
    case 1041: return std::string("Błąd weryfikacji kopii elektronicznej");
    case 1042: return std::string("Pamięć podręczna pełna");
    case 1043: return std::string("Nie zapisano kopii elektronicznej");
    case 1044: return std::string("Błąd wydruku kopii elektronicznej");
    case 1045: return std::string("Brak karty");
    case 1046: return std::string("Brak danych");
    case 1047: return std::string("Brak gotowości karty");
    case 1048: return std::string("Błąd zamknięcia karty");
    case 1049: return std::string("Błąd otwarcia karty");
    case 1050: return std::string("Błąd pliku id.txt");
    case 1051: return std::string("Błąd pliku no.txt");
    case 1052: return std::string("Błąd odtwarzania bazy plików");
    case 1053: return std::string("Błąd hasła karty pamięci");
    case 1054: return std::string("Brak dostępu");

    // poniższe 2 to nie pomyłka, tak jest w dokumentacji

    case 1055: return std::string("Błąd pamięci podręcznej");
    case 1056: return std::string("Błąd pamięci podręcznej");

    case 1057: return std::string("Błąd bazy kart kopii elektronicznej");
    case 1058: return std::string("Błąd bazy plików kopii elektronicznej");

    // zakres od 1059 do 9998 zarezerwowany (aktualnie nie używany)

    case 9999: return std::string("Błąd fatalny");

    default:   return std::string("Nieznany numer błędu");

    }
  }

}; // struct PrinterError


/// Identyfikator kasy/kasjera.
struct Id
{
  Id() : printerId(""), operatorId("") {}
  Id(const std::string &p, const std::string &o) : printerId(p), operatorId(o) {}

  std::string printerId;  ///< Numer kasy (maksymalnie 8 znaków).
  std::string operatorId; ///< Kasjer (maksymalnie 32 znaki).

  bool isEmpty() const { return printerId.empty() || operatorId.empty(); }

  static Id useDefault() { return Id(); }

}; // struct Id


/// Informacje o wersji oprogramowania.
struct VersionInfo
{
  std::string type;    ///< Typ (np. 'VENTO').
  std::string version; ///< Wersja (np. '1.00').

}; // struct VersionInfo


/// Ogólne informacje o urządzeniu.
struct DeviceInfo1
{
  std::string name;               ///< Nazwa urządzenia.

  std::string softwareVersion;    ///< Wersja programu.
  std::string printModuleVersion; ///< Wersja modułu drukującego.

  std::string systemName;         ///< Nazwa systemu.
  std::string systemVer;          ///< Wersja systemu.

  int displays;                   ///< Liczba wyświetlaczy.

  int printingWidth;              ///< Szerokość wydruku.

  int eCopy;                      ///< Kopia elektroniczna.

  int fiscalMemorySize;           ///< Rozmiar pamięci fiskalnej (w bajtach).

  DeviceInfo1() : displays(0), printingWidth(0), eCopy(0), fiscalMemorySize(0) {}

}; // struct DeviceInfo1


/// Szczegółowe informacje o urządzeniu.
struct DeviceInfo2
{
  int fiscalMemorySize;        ///< Rozmiar pamięci fiskalnej (w bajtach).
  int recordSize;              ///< Rozmiar rekordu (w bajtach).

  int mode;                    ///< Tryb działania (0 niefiskalny, 1 fiskalny).

  std::string uniqueNumber;    ///< Numer unikatowy.

  std::string nip;             ///< NIP.

  int maxRecordsCount;         ///< Maksymalna liczba rekordów.
  int recordsCount;            ///< Liczba rekordów.

  int maxDailyReportsCount;    ///< Maksymalna liczba raportów dobowych.
  int dailyReportsCount;       ///< Liczba raportów dobowych.

  int maxRamResetsCount;       ///< Maksymalna liczba zerowań RAM.
  int ramResetsCount;          ///< Liczba zerowań RAM.

  int maxVatRatesChangesCount; ///< Maksymalna liczba zmian stawek PTU
  int vatRatesChangesCount;    ///< Liczba zmian stawek PTU.

  int maxCurrencyChangesCount; ///< Maksymalna liczba zmian waluty.
  int currencyChangesCount;    ///< Liczba zmian waluty.

  DeviceInfo2() : fiscalMemorySize(0), recordSize(0), mode(0), maxRecordsCount(0), recordsCount(0),
    maxDailyReportsCount(0), dailyReportsCount(0), maxRamResetsCount(0), ramResetsCount(0),
    maxVatRatesChangesCount(0), vatRatesChangesCount(0), maxCurrencyChangesCount(0),
    currencyChangesCount(0) {}

}; // struct DeviceInfo2


/// Data/czas.
struct ClockInfo
{
  int year;   ///< Rok.
  int month;  ///< Miesiąc.
  int day;    ///< Dzień.

  int hour;   ///< Godzina.
  int minute; ///< Monuta.

  ClockInfo() : year(0), month(0), day(0), hour(0), minute(0) {}

}; // struct ClockInfo


/// Data/wiadomość.
struct ServiceDate
{
  int year;   ///< Rok.
  int month;  ///< Miesiąc.
  int day;    ///< Dzień.

  /// Wiadomość.
  std::string message;

  ServiceDate() : year(0), month(0), day(0) {}

}; // struct ServiceDate


/// Informacje kasowe (tryb 0-21).
struct CashRegisterInfo1
{
  int lastError;      ///< Kod ostatniego błędu.

  bool fiscal;        ///< Drukarka jest w trybie fiskalnym.
  bool transaction;   ///< Drukarka jest w trybie transakcji.
  bool transactionOk; ///< Ostatnia transakcja została zakończona poprawnie.

  int ramResets;      ///< Ilość zerowań RAM zapisanych w pamięci fiskalnej.

  int year;           ///< Data ostatniego zapisu do pamięci fiskalnej: rok.
  int month;          ///< Data ostatniego zapisu do pamięci fiskalnej: miesiąc.
  int day;            ///< Data ostatniego zapisu do pamięci fiskalnej: dzień.

  float vatA;         ///< Stawka podatkowa PTU A.
  float vatB;         ///< Stawka podatkowa PTU B.
  float vatC;         ///< Stawka podatkowa PTU C.
  float vatD;         ///< Stawka podatkowa PTU D.
  float vatE;         ///< Stawka podatkowa PTU E.
  float vatF;         ///< Stawka podatkowa PTU F.

  int reciepts;       ///< Ilość wydrukowanych paragonów fiskalnych (licznik jest zerowany
                      ///< w trakcie zerowania RAM).

  float totA;         ///< Stan totalizera dla stawki PTU A (kwota brutto).
  float totB;         ///< Stan totalizera dla stawki PTU B (kwota brutto).
  float totC;         ///< Stan totalizera dla stawki PTU C (kwota brutto).
  float totD;         ///< Stan totalizera dla stawki PTU D (kwota brutto).
  float totE;         ///< Stan totalizera dla stawki PTU E (kwota brutto).
  float totF;         ///< Stan totalizera dla stawki PTU F (kwota brutto).
  float totG;         ///< Stan totalizera dla stawki PTU G (kwota brutto).

  float cash;         ///< Stan gotówki w kasie (w PLN lub w EURO).

  std::string number; ///< Numer unikatowy w formacie ABCNNNNNNNN.

  CashRegisterInfo1() : lastError(0), fiscal(false), transaction(false), transactionOk(false),
    ramResets(0), year(0), month(0), day(0), vatA(0.0), vatB(0.0), vatC(0.0), vatD(0.0), vatE(0.0),
    vatF(0.0), totA(0.0), totB(0.0), totC(0.0), totD(0.0), totE(0.0), totF(0.0), totG(0.0),
    cash(0.0) {}

}; // struct CashRegisterInfo1


/// Informacje kasowe (tryby 22, 23, 99).
struct CashRegisterInfo2
{
  int lastError;      ///< Kod ostatniego błędu.

  bool fiscal;        ///< Drukarka jest w trybie fiskalnym.
  bool transaction;   ///< Drukarka jest w trybie transakcji.
  bool transactionOk; ///< Ostatnia transakcja została zakończona poprawnie.

  int resets;         ///< Ilość zerowań RAM zapisanych w pamięci fiskalnej.

  int year;           ///< Data ostatniego zapisu do pamięci fiskalnej: rok.
  int month;          ///< Data ostatniego zapisu do pamięci fiskalnej: miesiąc.
  int day;            ///< Data ostatniego zapisu do pamięci fiskalnej: dzień.

  float vatA;         ///< Stawka podatkowa PTU A.
  float vatB;         ///< Stawka podatkowa PTU B.
  float vatC;         ///< Stawka podatkowa PTU C.
  float vatD;         ///< Stawka podatkowa PTU D.
  float vatE;         ///< Stawka podatkowa PTU E.
  float vatF;         ///< Stawka podatkowa PTU F.
  float vatG;         ///< Stawka podatkowa PTU G.

  int reciepts;       ///< Ilość wydrukowanych paragonów fiskalnych (licznik jest zerowany
                      ///< w trakcie zerowania RAM).

  float totA;         ///< Stan totalizera dla stawki PTU A (kwota brutto).
  float totB;         ///< Stan totalizera dla stawki PTU B (kwota brutto).
  float totC;         ///< Stan totalizera dla stawki PTU C (kwota brutto).
  float totD;         ///< Stan totalizera dla stawki PTU D (kwota brutto).
  float totE;         ///< Stan totalizera dla stawki PTU E (kwota brutto).
  float totF;         ///< Stan totalizera dla stawki PTU F (kwota brutto).
  float totG;         ///< Stan totalizera dla stawki PTU G (kwota brutto).

  float cash;         ///< Stan gotówki w kasie (w PLN lub w EURO).

  std::string number; ///< Numer unikatowy w formacie ABCNNNNNNNN.

  CashRegisterInfo2() : lastError(0), fiscal(false), transaction(false), transactionOk(false),
    resets(0), year(0), month(0), day(0), vatA(0.0), vatB(0.0), vatC(0.0), vatD(0.0), vatE(0.0),
    vatF(0.0), vatG(0.0), totA(0.0), totB(0.0), totC(0.0), totD(0.0), totE(0.0), totF(0.0), totG(0.0),
    cash(0.0) {}

}; // struct CashRegisterInfo2


/// Informacje kasowe (tryb 24).
struct CashRegisterInfo3
{
  int year;        ///< Data ostatniego zapisu do pamięci fiskalnej: rok.
  int month;       ///< Data ostatniego zapisu do pamięci fiskalnej: miesiąc.
  int day;         ///< Data ostatniego zapisu do pamięci fiskalnej: dzień.

  int usedReports; ///< Ilość danych (raportów dobowych) zapisanych w pamięci fiskalnej.
  int freeReports; ///< Ilość wolnych (raportów dobowych) miejsc do zapisania w pamięci fiskalnej.

  int locked;      ///< Ilość towarów zablokowanych.

  float totA;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU A (kwota brutto).
  float totB;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU B (kwota brutto).
  float totC;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU C (kwota brutto).
  float totD;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU D (kwota brutto).
  float totE;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU E (kwota brutto).
  float totF;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU F (kwota brutto).
  float totG;      ///< Stan totalizera _ostatniego_ _paragonu_ dla stawki PTU G (kwota brutto).

  CashRegisterInfo3() : year(0), month(0), day(0), usedReports(0), freeReports(0), locked(),
    totA(0.0), totB(0.0), totC(0.0), totD(0.0), totE(0.0), totF(0.0), totG(0.0) {}

}; // struct CashRegisterInfo3


/// Informacje kasowe (tryb 50).
struct CashRegisterInfo4
{
  int documents; ///< Liczba dokumentów pomniejszona o liczbę faktur.
  int invoices;  ///< Ilość faktur.

  CashRegisterInfo4() : documents(0), invoices(0) {}

}; // struct CashRegisterInfo4


/// Informacje kasowe (tryb 90).
struct CashRegisterInfo5
{
  std::string label;     ///< Etykieta karty.
  std::string state;     ///< Stan karty ("O" otwarta, "Z" zamknięta, "N" nieznana, "W" wyjęta/brak, "B" błąd na karcie, "?" nieznany stan).

  int size;              ///< Rozmiar karty.
  int freeMem;           ///< Ilość wolnego miejsca.
  int files;             ///< ilość plików danych na karcie.

  int freeReports;       ///< Ilość wolnych raportów dobowych.
  int lastReportNr;      ///< Numer ostatniego raportu dobowego.

  std::string lastWrite; ///< Data i czas ostatniego zapisu na kartę (yyy-mm-dd hh:mm).

  CashRegisterInfo5() : size(0), freeMem(0), files(0), freeReports(0), lastReportNr(0) {}

}; // struct CashRegisterInfo5


/// Informacje kasowe (tryb 100).
struct CashRegisterInfo6
{
  int type;        ///< Rodzaj odsyłanych kwot: 0 brutto, 1 netto, 2 VAT

  int transaction; ///< Stan transakcji (0 brak transakcji, 1 paragon, 17 paragon w trybie blokowym off-line, 19 faktura VAT).

  float total;     ///< Suma wartości totalizarów.

  float totA;      ///< Stan totalizera dla stawki PTU A.
  float totB;      ///< Stan totalizera dla stawki PTU B.
  float totC;      ///< Stan totalizera dla stawki PTU C.
  float totD;      ///< Stan totalizera dla stawki PTU D.
  float totE;      ///< Stan totalizera dla stawki PTU E.
  float totF;      ///< Stan totalizera dla stawki PTU F.
  float totG;      ///< Stan totalizera dla stawki PTU G.

  CashRegisterInfo6() : type(0), transaction(0), total(0.0), totA(0.0), totB(0.0), totC(0.0),
    totD(0.0), totE(0.0), totF(0.0), totG(0.0) {}

}; // struct CashRegisterInfo6


/// Informacje kasowe (tryb 200).
struct CashRegisterInfo7
{
  float amount; ///< Kwota.

  CashRegisterInfo7() : amount(0.0) {}

}; // struct CashRegisterInfo7


/// Rekord pamięci fiskalnej
struct FiscalMemoryRecord
{
  enum RECORD_TYPE
  {
    RT_EMPTY,               ///< Pusty rekord, oznacza koniec pobierania danych.

    RT_DAILY_REPORT,        ///< Rekord raportu dobowego.
    RT_VAT_CHANGE_REPORT,   ///< Rekord zmian stawek VAT.
    RT_RAM_RESET_REPORT,    ///< Rekord zerowania pamięci RAM.
    RT_SELL_AFTER_RAM_RESET ///< Rekord rozpoczęcia sprzedaży po zerowaniu pamięci RAM.

  }; // enum RECORD_TYPE

  virtual RECORD_TYPE getType() const = 0;

  int year;   ///< Data wpisu: rok.
  int month;  ///< Data wpisu: miesiąc.
  int day;    ///< Data wpisu: dzień.

  int hour;   ///< Czas wpisu: godzina.
  int minute; ///< Czas wpisu: minuta.
  int second; ///< Czas wpisu: sekunda.

  FiscalMemoryRecord() : year(0), month(0), day(0), hour(0), minute(0), second(0) {}
  virtual ~FiscalMemoryRecord() {}

}; // struct FiscalMemoryRecord


/// Rekord raportu dobowego.
struct DailyReportRecord : public FiscalMemoryRecord
{
  virtual RECORD_TYPE getType() const { return FiscalMemoryRecord::RT_DAILY_REPORT; }

  int receipts;                 ///< Ilość paragonów.
  int cancelledReceipts;        ///< Ilość anulowanych paragonów.
  int databaseChanges;          ///< Ilość zmian w bazie towarowej.
  float cancelledReceiptsValue; ///< Wartość anulowanych paragonów.

  float totA;                   ///< Stan totalizera dla stawki PTU A.
  float totB;                   ///< Stan totalizera dla stawki PTU B.
  float totC;                   ///< Stan totalizera dla stawki PTU C.
  float totD;                   ///< Stan totalizera dla stawki PTU D.
  float totE;                   ///< Stan totalizera dla stawki PTU E.
  float totF;                   ///< Stan totalizera dla stawki PTU F.
  float totG;                   ///< Stan totalizera dla stawki PTU G.

  DailyReportRecord() : receipts(0), cancelledReceipts(0), databaseChanges(0), cancelledReceiptsValue(0.0),
    totA(0.0), totB(0.0), totC(0.0), totD(0.0), totE(0.0), totF(0.0), totG(0.0) {}
  virtual ~DailyReportRecord() {}

}; // struct DailyReportRecord


/// Rekord zmiany stawek VAT.
struct VatChangeRecord : public FiscalMemoryRecord
{
  virtual RECORD_TYPE getType() const { return FiscalMemoryRecord::RT_VAT_CHANGE_REPORT; }

  float vatA; ///< Stawka podatkowa PTU A.
  float vatB; ///< Stawka podatkowa PTU B.
  float vatC; ///< Stawka podatkowa PTU C.
  float vatD; ///< Stawka podatkowa PTU D.
  float vatE; ///< Stawka podatkowa PTU E.
  float vatF; ///< Stawka podatkowa PTU F.
  float vatG; ///< Stawka podatkowa PTU G.

  VatChangeRecord() : vatA(0.0), vatB(0.0), vatC(0.0), vatD(0.0), vatE(0.0), vatF(0.0), vatG(0.0) {}
  virtual ~VatChangeRecord() {}

}; // struct VatChangeRecord


/// Rekord zerowania pamięci RAM.
struct RamResetRecord : public FiscalMemoryRecord
{
  virtual RECORD_TYPE getType() const { return FiscalMemoryRecord::RT_RAM_RESET_REPORT; }

  int reason;      ///< Powód zerowania.
  int resetNumber; ///< Numer zerowania.

  RamResetRecord() : reason(0), resetNumber(0) {}
  virtual ~RamResetRecord() {}

}; // struct RamResetRecord


/// Rekord sprzedaży po zerowaniu pamięci RAM.
struct SellAfterRamResetRecord : public FiscalMemoryRecord
{
  virtual RECORD_TYPE getType() const { return FiscalMemoryRecord::RT_SELL_AFTER_RAM_RESET; }

  SellAfterRamResetRecord() {}
  virtual ~SellAfterRamResetRecord() {}

}; // struct SellAfterRamResetRecord


/// Dodatkowe linie.
struct ExtraLines
{
  std::string line1;
  std::string line2;
  std::string line3;

  bool isEmpty() const { return line1.empty() && line2.empty() && line3.empty(); }

  int count() const
  {
    if (!line3.empty()) return 3;
    if (!line2.empty()) return 2;
    if (!line1.empty()) return 1;

    return 0;
  }

  static ExtraLines none() { return ExtraLines(); }

}; // struct ExtraLines


/// Linia paragonu/faktury.
struct Item
{
  int line;                          ///< Numer linii (jeżeli 0, to operacja STORNO).
                                     ///< Na paragonie/fakturze numeracja musi być ciągła, to znaczy,
                                     ///< jeśli wystąpiła linia STORNO, to kolejna linia
                                     ///< powinna mieć kolejny numer, przykładowo 1,2,3,0,5,6.

  std::string name;                  ///< Nazwa towaru (od 2 do 40 znaków).
  std::string barcode;               ///< Kod kreskowy (od 1 do 31 znaków) (jeśli nie jest pusty, to drukowany zamiast opisu towaru).
                                     ///< Jeśli jest poprzedzony znakiem '#' to zostanie wydrukowany jako kod kreskowy PLU.
                                     ///< Jeśli jest poprzedzony znakiem '@' to zostanie wydrukowany jako kod kreskowy QR.
                                     ///< Jeśli nie jest poprzedzony znakiem '#' lub '@' to zostanie wydrukowany jako tekst ('K:' i podany kod).

  std::string description;           ///< Opis towaru (maksymalnie 160 znaków).

  std::string vat;                   ///< Stawka PTU (A, B, C, D, E, F lub G).

  std::string quantity;              ///< Ilość (dopuszczalne są dowolne inne znaki niż cyfry, drukarka wyodrębnia z tego napisu
                                     ///< maksymalnie 10 cyfrową liczbę z dowolnie ustawionym punktem dziesiętnym i drukuje
                                     ///< jedynie liczbę, nie mam pojęcia po jaką cholerę te akrobacje).

  float price;                       ///< Cena jednostkowa brutto, maksymalnie 10 cyfr, precyzja do 2 miejsc po przecinku.
  float gross;                       ///< Kwota brutto (quantity x price).

  ITEM_DISCOUNT_TYPE discountType;        ///< Rodzaj rabatu.
  DISCOUNT_DESCRIPTION_TYPE discountDesc; ///< Opis rabatu.
  float discountValue;               ///< Kwota lub procent rabatu/dopłaty.
  std::string discountName;          ///< Niestandardowa nazwa rabatu, jeśli discountDescription = DD_16. (maksymalnie 40 znaków)

}; // struct Item


/// Dane form płatności (1).
struct PaymentFormsInfo1
{
  bool cashFlag;             ///< Flaga 'Wpłata'.
  bool cardFlag;             ///< Flaga 'Karta'.
  bool chequeFlag;           ///< Flaga 'Czek'.
  bool couponFlag;           ///< Flaga 'Bon'.
  bool depositCollectedFlag; ///< Flaga 'Przyjęcie' (pobranie kaucji).
  bool depositReturnedFlag;  ///< Flaga 'Wydanie' (zwrócenie kaucji).
  bool changeFlag;           ///< Flaga 'Reszta'.

  float cashIn;              ///< Wpłata.
  float cardIn;              ///< Karta.
  float chequeIn;            ///< Czek.
  float couponIn;            ///< Bon.
  float depositCollected;    ///< Kaucje pobrane.
  float depositReturned;     ///< Kaucje zwrócone.
  float checkOut;            ///< Reszta.

  std::string cardName;      ///< Nazwa karty (maksymalnie 16 znaków).
  std::string chequeName;    ///< Nazwa czeku (maksymalnie 16 znaków).
  std::string couponName;    ///< Nazwa bonu (maksymalnie 16 znaków).

  PaymentFormsInfo1() : cashFlag(false), cardFlag(false), chequeFlag(false), couponFlag(false),
    depositCollectedFlag(false), depositReturnedFlag(false), changeFlag(false), cashIn(0.0), cardIn(0.0),
    chequeIn(0.0), couponIn(0.0), depositCollected(0.0), depositReturned(0.0), checkOut(0.0) {}

}; // class PaymentFormsInfo1


/// Forma płatności (używana w PaymentFormsInfo2).
struct PaymentForm
{
  PAYMENT_TYPE type; ///< Typ formy płatności.
  std::string name;  ///< Nazwa formy płatności.
  float amount;      ///< Kwota formy płatności.

}; // struct PaymentForm


/// Kaucja pobrana/wydana (używana w PaymentFormsInfo2).
struct Deposit
{
  std::string nr;
  std::string quantity;
  float amount;

}; // struct Deposit


/// Dane form płatności (2).
struct PaymentFormsInfo2
{
  bool cashFlag;                         ///< Flaga 'Wpłata'.
  bool changeFlag;                       ///< Flaga 'Reszta'.

  float cashIn;                          ///< Wpłata.
  float changeOut;                       ///< Reszta.

  std::vector<PaymentForm> paymentForms; ///< Formy płatności (maksymalnie 16).

  std::vector<Deposit> depositCollected;   ///< Kaucje pobrane (maksymalnie 32).
  std::vector<Deposit> depositReturned;    ///< Kaucje zwrócone (maksymalnie 32).

  PaymentFormsInfo2() : cashFlag(false), changeFlag(false), cashIn(0.0), changeOut(0.0) {}

}; // class PaymentFormsInfo2


/// Dane rozpoczęcia faktury.
struct BeginInvoiceData
{
  int items;                            ///< Ilość pozycji <0;255>

  bool printCopy;                       ///< Oryginał/kopia (drukowane sa dwa dokumenty, oba z dodatkowym opisem).
  bool topMargin;                       ///< Górny margines (5 linii).
  bool signature;                       ///< Miejsce na podpis.

  int additionalCopies;                 ///< Liczba dodatkowych kopii <0;9> (lub wartość 255, która oznacza 'tylko oryginał').

  std::string invoiceNr;                ///< Numer faktury (maksymalnie 15 znaków).

  std::string nip;                      ///< NIP (dokładnie 13 znaków).

  std::string timeout;                  ///< Termin płatności (opcjonalny, maksymalnie 16 znaków).

  std::string paymentForm;              ///< Nazwa formy płatności (opcjonalna, maksymalnie 20 znaków).
                                        ///< Nie można użyć nazwy 'INNA' lub 'INNE'.
                                        ///< Podanie formy płatności w rozpoczęciu faktury dodaje wartość paragonu do płatności.
                                        ///< Wskazane jest wysyłać płatność wylko w 1 miejscu.

  std::string client;                   ///< Nazwa kupującego (opcjonalna, maksymalnie 26 znaków).
  std::string seller;                   ///< Nazwa wystawiającego (opjonalna, maksymalnie 26 znaków).

  std::string systemNr;                 ///< Numer systemowy, rozpoczyna się znakiem # (maksymalnie 30 znaków).

  std::vector<std::string> clientLines; ///< Linie danych odbiorcy (maksymalnie 8). Linie powinny zawierać dane odbiorcy faktury.

  BeginInvoiceData() : items(0), printCopy(false), topMargin(false), signature(false), additionalCopies(0) {}

}; // struct BeginInvoiceData


/// Dane zakończenia faktury.
struct FinishInvoiceData
{
  bool payedFlag;              ///< Czy drukować napis 'zapłacono'.
  std::string payed;           ///< Napis 'zapłacono'.

  CLIENT_SELLER_OPTION client; ///< Opcje dla kupującego.
  CLIENT_SELLER_OPTION seller; ///< Opcje dla sprzedającego.

  float cashIn;                ///< Wpłata.
  float total;                 ///< Razem.

  float discountValue;         ///< Rabat procentowy lub kwotowy.

  std::string clientName;      ///< Nazwa kupującego (maksymalnie 26 znaków).
  std::string sellerName;      ///< Nazwa sprzedającego (maksymalnie 26 znaków).

  ExtraLines extraLines;       ///< Dodatkowe linie w stopce. Możliwość użycia tylko 20 różnych tekstów
                               ///< pomiędzy raportami dobowymi.

  FinishInvoiceData() : payedFlag(false), client(CSO_0), seller(CSO_0), cashIn(0.0), total(0.0), discountValue(0.0) {}

}; // struct FinishInvoiceData


/// Opcje faktury.
struct InvoiceOptions
{
  int additionalCopies;        ///< Liczba dodatkowych kopii <0;9> (lub wartość 255, która oznacza 'tylko oryginał').

  CLIENT_SELLER_OPTION client; ///< Opcje dla kupującego.
  CLIENT_SELLER_OPTION seller; ///< Opcje dla sprzedającego.

  bool payedFlag;              ///< Czy drukować napis 'zapłacono'.
  std::string payed;           ///< Napis 'zapłacono'.

  int year;                    ///< Data sprzedaży: rok.
  int month;                   ///< Data sprzedaży: miesiąc.
  int day;                     ///< Data sprzedaży: dzień.

  int summaryOption;           ///< Opcja wydruku podsumowania (INVOICE_SUMMARY_OPTION).

  int invoiceOptions2;         ///< Opcje wydruku 2 (INVOICE_OPTION_2).
  int invoiceOptions3;         ///< Opcje wydruku 3 (INVOICE_OPTION_3).

  CLIENT_ID_TYPE clientIdType; ///< Typ identyfikatora klienta.

  std::string timeout;         ///< Termin płatności (maksymalnie 16 znaków) (opcjonalny napis).

  std::string paymentForm;     ///< Nazwa formy płatności (opcjonalna, maksymalnie 20 znaków).
                               ///< Nie można użyć nazwy 'INNA' lub 'INNE'.
                               ///< Podanie formy płatności w rozpoczęciu faktury dodaje wartość paragonu do płatności.
                               ///< Wskazane jest wysyłać płatność wylko w 1 miejscu.

  std::string clientName;      ///< Nazwa kupującego (maksymalnie 26 znaków).
  std::string sellerName;      ///< Nazwa sprzedającego (maksymalnie 26 znaków).

  std::string systemNr;        ///< Numer systemowy (maksymalnie 10 znaków).

  InvoiceOptions() : additionalCopies(0), client(CSO_0), seller(CSO_0), payedFlag(false),
    year(0), month(0), day(0), summaryOption(0), invoiceOptions2(0), invoiceOptions3(0),
    clientIdType(CIDT_NONE) {}

}; // struct InvoiceOptions


/// Dane pokwitowania sprzedaży/transakcji uznania.
struct SaleReceiptData
{
  bool printId;                    ///< Drukowanie numeru kasy i kasjera.

  SALE_RECEIPT_OPTION printOption; ///< Opcje wydruku.

  int month;                       ///< Miesiąc, do którego ważna jest karta.
  int year;                        ///< Rok, do którego ważna jest karta.

  float amount;                    ///< Kwota płacona kartą kredytową.

  std::string receipt;             ///< Napis zawierający numer paragonu, za który zapłata
                                   ///< następuje przy użyciu karty kredytowej.

  std::string clientName;          ///< Nazwa klienta (maksymalnie 15 znaków).

  std::string terminal;            ///< Nazwa terminala (maksymalnie 8 znaków).

  std::string cardName;            ///< Nazwa karty kredytowej (maksymalnie 16 znaków).
  std::string cardNr;              ///< Numer karty kredytowej (maksymalnie 20 znaków).

  std::string authCode;            ///< Kod autoryzacji (maksymalnie 9 znaków).

  SaleReceiptData() : printId(false), printOption(SRO_0), month(0), year(0), amount(0.0) {}

}; // struct SaleReceiptData


/// Dane linii wydruku niefiskalnego.
struct NonFiscalLine
{
  int printNr;                    ///< Numer wydruku/raportu, którego linia ma być wydrukowana (w zakresie <2;255>,
                                  ///< gdy 254 to w linii będzie drukowany kod kreskowy, który posłany jest w argumencie).

  int lineNr;                     ///< Numer linii, która ma być wydrukowana (liczenie zaczynamy od 0, pomijając te, które są nagłówkami).
                                  ///< 249 gdy linia ma być kodem QR
                                  ///< 255 gdy linia ma być pusta
                                  ///< 254 gdy linia ma być podkreśleniem
                                  ///< 253 gdy linia 'nr ostatniego paragonu'
                                  ///< 250 gdy linia ma być grafiką (w 1 argumencie podajemy indeks grafiki 0..5)

  bool bold;                      ///< Formatowanie tekstu: pogrubienie (zmniejsz o połowę ilość znaków mieszczących się w linii).
  bool inverse;                   ///< Formatowanie tekstu: inwers.
  bool center;                    ///< Formatowanie tekstu: wyśrodkowanie.

  int font;                       ///< Numer czcionki  z zakresu <0;1> (2 czcionki do wyboru).

  FONT_ATTRS attrs;               ///< Dodatkowe atrybuty czcionki.

  std::vector<std::string> lines; ///< Argumenty (ilość linii jest dowolna).
                                  ///< Dowolny ciąg znaków ze zbiorów a..z, A..Z, 0..9, ąćęłóńśżźĄĆĘŁÓŚŻŹ,
                                  ///< ~@#$%^&*()_-+=[{]}\|;:’”,<.>/?. Argumenty numeryczne akceptują: cyfry, pierwsze 3 litery,
                                  ///< znaki specjalne, argumenty tekstowe akceptują: litery, pierwsze 3 cyfry, znaki specjalne.
                                  ///< Argumenty alfanumeryczne akceptują wszystkie znaki.

  NonFiscalLine() : printNr(0), lineNr(0), bold(false), inverse(false), center(false), font(0), attrs(FA_0) {}

}; // struct NonFiscalLine


} // namespace fp


#endif // __FP_COMMON_HPP__
