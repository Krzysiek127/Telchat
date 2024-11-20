#ifndef PL_H
#define PL_H

// works for only 64 bit architechture
#if defined(__MINGW64__) || defined(_WIN64) || defined(_M_X64) || defined(__amd64__)

#define jeżeli                  if
#define w_przeciwnym_wypadku    else
#define dla                     for
#define dopuki                  while
#define zwruć                   return
#define przełacznik             switch
#define przypadek               case
#define domyślny                default
#define zniszcz                 break
#define kontynuuj               continue
#define struktura               struct
#define wykonaj                 do
#define licznik                 enum
#define rozmiar                 sizeof
#define imie_typa               typename
#define identyfikator_typa      typeid
#define idź_do                  goto
#define stały                   const
#define wlinjowy                inline
#define statyczny               static
#define eteryczny               volatile
#define unia                    union
#define definicja_typa          typedef
#define rejestr                 register
#define nieoznaczony            unsigned
#define oznaczony               signed
#define długi                   long
#define krótki                  short
#define główna                  main
#define zewnętrzny              extern

definicja_typa int      całkowity;
definicja_typa float    zmiennoprzecinkowy;
definicja_typa double   podwójny;
definicja_typa char     znakowy;
definicja_typa void     otchłań;


#ifdef __cplusplus

#define wyrównaj_jako       alignas
#define wyrównaj_dla        alignof
#define i                   and
#define i_równe             and_eq
#define złż                 asm
#define automatyczny        auto
#define bitowe_i            bitand
#define bitowe              bitor
#define złap                catch
#define klasa               class
#define bitowe_nie          compl
#define koncept             concept
#define stałe_wyrażenie     constexpr
#define stały_odlew         const_cast
#define deklaruj_typa       decltype
#define usuń                delete
#define dynamiczny_odlew    dynamic_cast
#define lubierzny           explicit
#define wyeksportuj         export
#define fałsz               false
#define przyjaciel          friend
#define wzruszalny          mutable
#define przestrzen_nazw     namespace
#define nowy                new
#define nie_wyjątkuj        noexcept
#define nie                 not
#define nie_równe           not_eq
#define wskaźnik_pusty      nullptr
// operator
#define lub                 or
#define lub_równe           or_eq
#define prywatny            private
#define chroniony           protected
#define publiczny           public
#define reinterpretowany_odlew reinterpret_cast
#define wymaga              requires
#define statycznie_zapewnij static_assert
#define statyczny_odlew     static_cast
#define szablon             template
#define ten                 this
#define lokalny_wątek       thread_local
#define rzuć                throw
#define prawda              true
#define spróbuj             try
#define użyj                using
#define wirtualny           virtual
#define ekskluzywne_lub     xor
#define ekskluzywne_lub_równe xor_eq

definicja_typa bool logiczny;

#else
#define ograniczony restrict
#endif

#endif
#endif