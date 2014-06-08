mproto
======

Praca na zaliczenie


Protokół komunikacyjny w konfiguracji klient/serwer


Funkcje
=======

- Szyfrowanie
- Weryfikowanie sumy kontrolnej
- Dzielenie wiadomości na części o podanej wielkości


Opis i uruchomienie
============

Serwer: `mproto [-v]`
Klient: `mproto [-v] [-s] [-p size] [-m message] address`


Flaga `-v` sprawia, że wypisywane komunikaty będą bardziej szczegółowe. Tym bardziej szczegółowe im więcej razy zostanie podana flaga.


Serwer domyślnie nasłuchuje na adresie 0.0.0.0 na porcie 21212 (można zmienić to w config.h). Jego zadaniem jest odbieranie i wyświetlanie wiadomości.


Klient może wysłać wiadomości na dwa sposoby. Przez parametr `-m` lub interaktywnie: przesyłając każdy wpisany ciąg do naciśnięcia klawisza enter.
Flaga `-p` określa rozmiar pojedynczego fragmentu wysyłanego do serwera. Serwer musi zaakceptować podany rozmiar (domyślnie 10 znaków).


Wysyłanie wiadomości można zasymulować w celu sprawdzenia poprawności dziania klienta. Służy do tego flaga `-s`.


Szybki test
===========

By skompilować program uruchom:

    $ make


Następnie w jednej konsoli wpisz (znajdując się w katalogu z mproto):

    $ ./mproto


W drugiej konsoli lub w konsoli innego komputera wpisz:

	$ ./mproto -m 'Hej, mproto!' 127.0.0.1


W przypadku innego komputera zamień 127.0.0.1 na jego adres komputera na którym uruchomiono serwer.
