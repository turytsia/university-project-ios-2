# Synchronization in Linux
## Mini description
Molekuly vody jsou vytvářeny ze dvou atomů `vodíku` a jednoho atomu `kyslíku`. V systému jsou tři typy
procesů: (0) hlavní proces, (1) kyslík a (2) vodík. Po vytvoření procesů se procesy reprezentující
kyslíky a vodíky řadí do dvou front—jedna pro kyslíky a druhá pro vodíky. Ze začátku fronty vždy
vystoupí jeden kyslík a dva vodíky a vytvoří molekulu. V jednu chvíli je možné vytvářet pouze jednu
molekulu. Po jejím vytvoření je prostor uvolněn dalším atomům pro vytvoření další molekuly. Procesy,
které vytvořily molekulu následně končí. Ve chvíli, kdy již není k dispozici dostatek atomů kyslíku
nebo vodíku pro další molekulu (a ani žádné další již nebudou hlavním procesem vytvořeny) jsou
všechny zbývající atomy kyslíku a vodíku uvolněny z front a procesy jsou ukončeny.
## How to use
```bash
./proj2 NO NH TI TB
```
- NO: Počet kyslíků
- NH: Počet vodíků
- TI: Maximální čas milisekundách, po který atom kyslíku/vodíku po svém vytvoření čeká, než se
zařadí do fronty na vytváření molekul. 0<=TI<=1000
- TB: Maximální čas v milisekundách nutný pro vytvoření jedné molekuly. 0<=TB<=1000
## Output Example
```bash
1: H 1: started
2: H 3: started
3: O 1: started
4: O 1: going to queue
5: H 2: started
6: H 2: going to queue
7: H 1: going to queue
8: O 3: started
9: O 3: going to queue
10: H 5: started
11: H 4: started
12: O 2: started
13: H 1: creating molecule 1
14: H 4: going to queue
15: O 1: creating molecule 1
16: H 2: creating molecule 1
17: H 5: going to queue
18: H 2: molecule 1 created
19: H 1: molecule 1 created
20: O 1: molecule 1 created
21: H 3: going to queue
22: O 3: creating molecule 2
23: O 2: going to queue
24: H 4: creating molecule 2
25: H 5: creating molecule 2
26: O 3: molecule 2 created
27: H 4: molecule 2 created
28: H 5: molecule 2 created
29: H 3: not enough O or H
30: O 2: not enough H
```

## Results
`- / 15`

## Project is for Operation System courses
You can learn more abou this course [here](https://www.fit.vut.cz/study/course/244864/.cs)
