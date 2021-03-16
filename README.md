# Specifikacija
Instrukcije se pisu kao CSV `ADD; rax; 15`

Komentari se mogu naci bilo gdje, iza znaka # kao npr `ADD; rax; 15 #dodaje 0x15 na registar rax`

Memoriji se pristupa samo u blokovima po vjerovatno 8 bajtova.

Instrukcija; DEST; SRC

## Specifikacija broja registara
Mozete koristiti flag pri pokretanju `-rcount` _integer_ da postavite broj registara s kojima procesor raspolaze. npr `-rcount 5` znaci da ce jedini dostupni (i prihvatljivi) registri biti `rax`, `rbx`, `rcx`, `rdx` i `rex`.

Broj registara __mora__ biti izmedju 0 i 26 inkluzivno. Registri su imenovani redom `rax`, `rbx`, `rcx` itd odnosno `r('a'+i)x` za `i = 0..brojRegistara`.


## Konstante
Podrzane su decimalne konstante i heksadecimalne. Decimalne se pisu kao i svuda, dok su heksadecimalne konstante dosta zabavnije i jedini uslov je da imaju x na prvom ( ;) ) mjestu npr:
```
mov; rax; 0x10; #upise 16 u rax
mov; rax; 9x10; #ISTO jer je bitno samo da je x na poziciji [1]
```
## Dodaci
__Breakpoint__ se moze definisati na bilo kojoj liniji koja __POCINJE__ sa znakom `*`, nije bitno sta je iza toga, ta linija je onda breakpoint i ako je interpreter pokrenut u debug modu zaustavice se na toj liniji

__Ispis i upis__ se mogu vrsiti "instrukcijama" `$print`, `$println` i `$read` ali `$read` moze da cita samo cijele brojeve (hex sa formatom 0xAAAAA ili 0XAAAAA (vodeca nula nije neophodna, samo je vazan x na drugom mjestu, zabavno)).

__Labele__ su oznacene sa znakom _ ispred kao npr `_pocetak` ili `_kraj`

## Primjeri
Par primjera se nalazi u ./examples/ folderu.