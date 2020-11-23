Treść:
Bufor 9 elementowy FIFO. Dwóch konsumentów i dwóch producentów. Producent produkują liczby całkowite. Producent A może zapisywać elementy do bufora jedynie gdy ich suma w buforze będzie mniejsza niż 20. Odczyt (usunięcie) jest dozwolone, gdy liczba elementów w buforze jest większa niż 3.

Rozwiązanie:
- Semafory:
        
1. mutex - ograniczenie dostępu do SK do 1 procesu jednocześnie
2. A - semafor blokujący wykonanie producenta A
3. B - semafor blokujący wykonanie producenta B
4. cons - semafor blokujący konsumentów

- Zmienne globalne:

1. A_waits, B_waits - czy producent A/B oczekuje na wybudzenie po sprawdzeniu warunków
2. a,b - pomocnicze zmienne do testowania stanu semaforów

Na początku konsumenci czekają na odpowiednią liczbę elementów w buforze (>3). Jeden z producentów wchodzi do SK i dodaje nowy element. Jeśli suma przekracza 20, producent A przekazuje SK producentowi B albo konsumentom (w zależności od liczności bufora). Producent B może wybudzić producenta A albo konsumenta i przekazać SK. Konsument może wybudzić jednego z producentów, przy czym ze względu na mniejsze prawdopodobieństwo wybudzenia producenta A warunek jego wybudzenia sprawdzany jest jako pierwszy.

