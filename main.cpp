#include <iostream>
#include "monitor.h"
#include <pthread.h>
#include <queue>     
#include <unistd.h>
#include <time.h>

#define	MIN_CONSUMER	3	// Minimalna liczba elementów w buforze do wybudzenia konsumentów
#define	MAX_BUFFER_SIZE	9	// Maksymalna pojemność bufora
#define MAX_SUM_PROD_A  20  // Maksymalna suma elementów w buforze dla producenta A
   

using namespace std;


           
Semaphore mutex(1);	// Dostęp do SK
Semaphore A(0); // Semafor producenta A
Semaphore B(0);		// Semafor producenta B
Semaphore cons(0);	// Semafor konsumentów

bool A_waits = false;
bool B_waits = false;

int a = 0;
int b = 0;


                                                            
queue<int> bufor;


int suma = 0;

void* prodA(void* ptr)
{
    while(true)
    {
        usleep(rand()%1000000);
	    int n = rand();
	    mutex.p();
        cout<<"PRODUCENT A W SEKCJI KRYTYCZNEJ\n";
    	if(suma >= MAX_SUM_PROD_A || bufor.size() >= MAX_BUFFER_SIZE)
    	{
            A_waits = true;
            mutex.v();
            cout<<"PRODUCENT A OPUSZCZA SK I ZASYPIA\na: "<<a<<" b: "<<b<<endl;
            A.p();
            a--;
            A_waits = false;
            cout<<"PRODUCENT A POWRACA DO SK\na: "<<a<<" b: "<<b<<endl;
     
	    }
        cout<<"PRODUCENT A DODAJE "<<n<<" DO BUFORA\n";
        bufor.push(n);
        suma += n;
        cout<<"        SUMA ELEMENTOW: "<<suma<<endl<<"        LICZBA EL: "<<bufor.size()<<endl;
        if(bufor.size() > MIN_CONSUMER) 
        {
            cout<<"BUDZENIE KONSUMENTOW\n";
            cons.v();
        }
        cout<<"PRODUCENT A PODNOSI MUTEX I WYCHODZI Z SK\n";
        mutex.v();
    }
}

void* prodB(void* ptr)
{
    while(true)
    {
        usleep(rand()%1000000);
	    int n = rand();
	    mutex.p();
        cout<<"PRODUCENT B W SEKCJI KRYTYCZNEJ\n";
	    if(bufor.size() >= MAX_BUFFER_SIZE)
	    {
            B_waits = true;
            mutex.v();
            cout<<"PRODUCENT B OPUSZCZA SK I ZASYPIA\na: "<<a<<" b: "<<b<<endl;
        
            B.p();
            b--;
            B_waits = false;
            cout<<"PRODUCENT B POWRACA DO SK\na: "<<a<<" b: "<<b<<endl;
        
	    }
        cout<<"PRODUCENT B DODAJE "<<n<<" DO BUFORA\n";
        bufor.push(n);
        suma += n;
        cout<<"        SUMA ELEMENTOW: "<<suma<<endl<<"        LICZBA EL: "<<bufor.size()<<endl;

        if(bufor.size() > MIN_CONSUMER) 
        {
            cout<<"PRODUCENT B budzi KONSUMENTOW\n";
            cons.v();
        }
        if(A_waits && suma < MAX_SUM_PROD_A && bufor.size() < MAX_BUFFER_SIZE) 
        {
            cout<<"PRODUCENT B BUDZI PRODUCENTA A\n";
            a++;
            cout<<"a: "<<a<<" b: "<<b<<endl;
            A.v();
        }
        else
        {
            cout<<"PRODUCENT B PODNOSI MUTEX I WYCHODZI Z SK\n";
            mutex.v();
        }
    }
}

void* consumer(void* ptr)
{
    int nr = *((int*) ptr);
    while(true)
    {
        usleep(rand()%1000000);
        cout<<"KONSUMENT "<<nr<<" BUDZI SIE I CHCE WEJSC\n";
        cons.p();
        cout<<"KONSUMENT "<<nr<<" PRZECHODZI PRZEZ CONS\n";
        mutex.p();
        cout<<"KONSUMENT "<<nr<<" WCHODZI DO SK\n";
        suma -= bufor.front();
        bufor.pop();
        cout<<"        SUMA ELEMENTOW: "<<suma<<endl<<"        LICZBA EL: "<<bufor.size()<<endl;
        
        if(A_waits && suma <= MAX_SUM_PROD_A && bufor.size() < MAX_BUFFER_SIZE)
        {
            cout<<"KONSUMENT "<<nr<<" BUDZI PRODUCENTA A\n";
            
            a++;
            cout<<"a: "<<a<<" b: "<<b<<endl;
            A.v();
        }
        
        else if(B_waits && bufor.size() < MAX_BUFFER_SIZE)
        {
            cout<<"KONSUMENT "<<nr<<" BUDZI PRODUCENTA B\n";
            b++;
            cout<<"a: "<<a<<" b: "<<b<<endl;
            B.v();
        }
        else
        {
            cout<<"KONSUMENT "<<nr<<" PODNOSI MUTEX I WYCHODZI Z SK\n";
            mutex.v();
        }
    }
}


int main(void) 
{
    pthread_t kons1, kons2, pB, pA;

    unsigned int *c1, *c2;
	
	srand(time(NULL));	
	
    c1 = new unsigned int;
    c2 = new unsigned int;

    *c1 = 1;
    *c2 = 2;
    
    pthread_create(&pA, NULL, prodA, NULL);
    pthread_create(&pB, NULL, prodB, NULL);
    pthread_create(&kons1, NULL, consumer, (void*) c1);
    pthread_create(&kons2, NULL, consumer, (void*) c2);

    pthread_join(pA, NULL);
    pthread_join(pB, NULL);
    pthread_join(kons1, NULL);
    pthread_join(kons2, NULL);

    delete c1;
    delete c2;

    return 0;
}
