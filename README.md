# Specifikacija
Instrukcije se pisu kao CSV `ADD; rax; 15`

Komentari se mogu naci bilo gdje, iza znaka # kao npr `ADD; rax; 15 #dodaje 0x15 na registar rax`

Memoriji se pristupa samo u blokovima po vjerovatno 4 bajta.

## Specifikacija broja registara
Mozete koristiti flag pri pokretanju `-rcount` _integer_ da postavite broj registara s kojima procesor raspolaze. npr `-rcount 5` znaci da ce jedini dostupni (i prihvatljivi) registri biti `rax`, `rbx`, `rcx`, `rdx` i `rex`.

Broj registara __mora__ biti izmedju 0 i 26 inkluzivno. Registri su imenovani redom `rax`, `rbx`, `rcx` itd odnosno `r('a'+i)x` za `i = 0..brojRegistara`.


## Konstante
Podrzane su decimalne konstante i heksadecimalne. Decimalne se pisu kao i svuda, dok su heksadecimalne konstante dosta zabavnije i jedini uslov je da imaju x na prvom ( ;) ) mjestu npr:
```
mov; rax; 0x10; #upise 16 u rax
mov; rax; 9x10; #ISTO jer je bitno samo da je x na poziciji [1]
```

# Podrzane instrukcije

## ADD
Instrukcija `add` ili `ADD` prihvata 2 operanda kao

`add; DST; SRC`

Gdje su:
* DST - Destination - moze biti registar ili adresa u memoriji (vidi [adresiranje](#adresiranje))
* SRC - Source - moze biti registar, memorija ili heksadecimalna konstanta.

# Adresiranje