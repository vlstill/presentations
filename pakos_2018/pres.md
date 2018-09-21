---
vim: spell spelllang=cs tw=80 fo+=t tabstop=2 shiftwidth=2
title: "Užitečné triky s Linuxem"
author:
    - Vladimír Štill
header-includes:
    - \input{defs}
lang: czech
date: 21\. 9\. 2018
...

## Základy `bash`

- nejobvyklejší linuxový shell
- prostředí příkazové řádky, skriptování

. . .

- chování shellu lze upravit pomocí `~/.bashrc`
  - export proměnných (`export VAR=val`{.bash})
  - nastavení promptu (`PS1`)
  - aliasy (`alias ls='ls --color=auto'`{.bash})
  - FI (hala/učebny): načítání modulů se software
  - mnoho dalšího

## Speciální proměnné I

- `PATH` -- při pokusu o spuštění binárky bez zadání cesty se ji pokusí najít v
  cestách zapsaných v `PATH`
  - cesty odděleny `:`
  - `echo $PATH`{.bash} $\rightarrow$ `/usr/bin:/home/xstill/bin`

      . . .

    - nejprve hledá v `/usr/bin` následně v `/home/xstill/bin`

    . . .

  - z bezpečnostních důvodů by cesty měly být absolutní
  - je důležité zachovat původní cesty:
    - `export PATH=$HOME/bin:$PATH`{.bash}
    - `export PATH=$PATH:$HOME/bin`{.bash}

## Speciální proměnné II

- `EDITOR` -- defaultní editor textu, použitý například pro `git` commit
  message
  - `export EDITOR=vim`{.bash}

. . .

- nezapomínejte na klíčové slovo `export` v `~/.bashrc`
  - zajistí, že se hodnoty dědí do spuštěných programů

## Užitečné nástroje -- `ssh`, `scp`

```{.bash}
$ ssh xstill@arke.fi.muni.cz
```

. . .

- co když potřebuji na veřejně nedostupný počítač?
- `~/.ssh/config`:

  ```{.haskell}
  Host nymfe* luna*
    Hostname %h.fi.muni.cz
    ProxyCommand ssh xstill@aisa.fi.muni.cz nc %h %p
  ```

  . . .

  - při pokusu o přihlášení na `nymfe*` nebo `luna*`
  - přidá k hostname `.fi.muni.cz`
  - tuneluje `ssh` přes aisu
  - dále užitečné: `User`{.haskell}

. . .

- kopírování souborů přes SSH:

  ```{.bash}
  $ scp -r data aisa:work/
  ```

  . . .

  - zkopíruje složku `data` do `work` na aise
  - `-r` znamená rekurzivně

## Užitečné nástroje -- `sshfs`

- připojení vzdáleného souborového systému přes SSH
- jednoduché, bezpečné
- pozor, může být pomalé, vést ke ztrátě dat při odpojení

```{.bash}
$ mkdir pakos2018
$ sshfs arke.fi.muni.cz:/var/tmp/pakos2018 pakos2018
$ cd pakos2018
```
. . .

- odpojení: `fusermount -u pakos2018`{.bash}

## Základy skriptování I

- `bash` je zároveň skriptovací jazyk
- `if`, cykly, funkce, …
- spoustu funkcí zajišťují externí příkazy/programy
- primárně pracuje se stringy

```{.bash}
if [[ "$X" = "yes" ]]; then
    echo "X is yes"
fi
```

  . . .

- uvozovky okolo `$X` jsou nutné pro správné řešení případů kdy je `$X` prázdné
  nebo nedefinované
- středník lze nahradit novým řádkem a naopak

## Základy skriptování II

- všechny konstrukty lze používat i z příkazové řádky

```{.bash}
$ for i in *.h; do mv $i include/; done
```

- pro všechny soubory končící na `.h`
- přesune soubor do složky `include` (která musí existovat)

## Základy skriptování III

**přesměrování**

- výstup příkazu lze přesměrovat, například do souboru
  - `$ echo "foo" > test.txt`{.bash}
- `>` přepíše výstupní soubor (`>>` přidává na konec)
- přesměrovává jen standardní výstup
- standardní chybový výstup:
  - `$ foo 2> err.txt`{.bash}
- obojí:
  - `$ foo >& both.txt`{.bash}

**řetězení**

`$ foo | bar`{.bash}

- pošle výstup z `foo` na vstup `bar`

## Užitečné utility -- `grep`

- vyhledání řádků podle regexu: `grep foo FILE`{.bash}

. . .

- vyhledávání ve složce: `grep -rn '\(struct\|class\) generator' .`{.bash}
  - vyhledává rekurzivně (`-r`)
  - vypisuje čísla řádků (`-n`)
  - v aktuální složce (`.`)

. . .

- vyhledání souborů, které obsahují text: `grep -rl REGEX DIR`{.bash}

. . .

- zobrazení řádků před (`grep -A NUM`{.bash})/po (`grep -B NUM`{.bash})/okolo (`grep
  -C NUM`{.bash})

. . .

\bigskip

```.bash
$ git status --porcelain --ignored | grep '^!!'
```

- vypíše všechny ignorované soubory v git repozitáři

## Užitečné utility -- `cut`

- rozdělení/odstranění částí výstupu po řádcích
- na strukturovaný výstup (tabulka s oddělovači, …)


```.bash
$ git status --porcelain --ignored | grep '^!!' | cut -c4-
```

- odstraní první 3 znaky z výstupy (vypíše vše od znaku 4)
- další volby:
  - `-f` -- rozsah polí (místo `-c`, oddělených oddělovačem)
  - `-d` -- oddělovač pro `-f`

## Užitečné utility -- `sed` I

- textová substituce na základě regulárního výrazu
- pracuje po řádcích

```{.bash}
$ for i in *.h; do mv $i $(echo $i| sed 's/[.]h$/.hpp/'); done
```

. . .

- přejmenuje `.h` soubory na `.hpp`
- `$(...)` je dosazení textového výstupu příkazu uvnitř závorek
- `'s/REGEX/REPLACE/'` `sed` výraz, který způsobí, že se všechny výskyty, které
  odpovídají regulárnímu výrazu `REGEX` nahradí za `REPLACE`
- `[.]h$` je regex popisující `.h` na konci řádku/řetězce
    - `[…]` -- popisuje jeden ze znaků v závorkách
    - `.` je normálně zástupný znak \uv{cokoli}, proto jej obalíme do `[…]`
    - `$` popisuje konec řádku

## Užitečné utility -- `sed` II

- `sed` je možné aplikovat na soubory
- upravený obsah vypisuje na standardní výstup
  - přepínač `-i`/`--in-place` způsobí modifikaci souboru
  - pozor na chybné změny -- verzujte (git)!

. . .

```{.bash}
$ sed -i 's/\(ib\|IB\)002/\1102/g' driver
```

- pracuje se souborem `driver`, změní ho
- nahradí všechny výskyty `IB002` za `IB102` a `ib002` za `ib102`
- `\(…\)` je zachycený podvýraz, `\1` se odkazuje na první takový podvýraz
- `g` zajišťuje, že se nahradí všechny výskyty na řádku, ne jen první

## Užitečné utility -- `find`

- vyhledávání souborů podle kritérií
- spouštění příkazů nad vyhledanými soubory

```{.bash}
$ find -name '*.sh' ! -executable
```

. . .

- vypíše názvy všech shell skriptů, které nejsou spustitelné

. . .

```{.bash}
$ find \( -type f -a ! -executable \) \
        -exec setfacl -m u:xbarnat:r {} \;
$ find \( -type d -o -executable \) \
        -exec setfacl -m u:xbarnat:rx {} \;
```

. . .

- přidá ke všem souborům rekurzivně práva uživateli `xbarnat`
  - pro spustitelné soubory a složky budou pro něj mít práva `rx` (číst a
    spustit/vstoupit)
  - nespustitelné soubory budou mít právo pro čtení
  - `exec`: `{}` se nahradí cestou k souboru, musí být ukončeno `\;`
  - `-a` je logický and, `-o` je logický or

  . . .

- (umělý příklad, `setfacl -m u:xbarnat:rx --no-mask --recursive .` udělá totéž)
