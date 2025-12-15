Capitolul 2 – Prezentarea părții hardware
2.1 Platforma de dezvoltare – Arduino Uno

În cadrul acestui proiect a fost utilizată placa de dezvoltare Arduino Uno, bazată pe microcontrolerul ATmega328P. Aceasta a fost aleasă datorită simplității în utilizare, numărului suficient de pini digitali și analogici, precum și compatibilității cu numeroase biblioteci disponibile.

Arduino Uno îndeplinește rolul de unitate centrală de control, fiind responsabilă de:

citirea comenzilor de la matricea de butoane 4×4;

implementarea logicii jocului X și O;

controlul și actualizarea matricei de LED-uri 8×8;

alternarea între modul Player vs Player și Player vs Computer.

2.2 Matricea de LED-uri 8×8 (WS2812B / NeoPixel)
Descriere generală

Pentru afișarea jocului a fost utilizată o matrice de LED-uri RGB 8×8, formată din 64 de LED-uri adresabile individual de tip WS2812B (NeoPixel). Fiecare LED poate afișa o culoare diferită și este controlat printr-un singur pin de date.

Rol în proiect

Matricea de LED-uri are următoarele funcții:

afișarea grilei jocului X și O;

afișarea mutărilor jucătorilor

X – culoare roșie

O – culoare albastră;

evidențierea câștigătorului prin animații de tip blink;

indicarea modului Player vs Computer printr-un LED dedicat.

Figura 1: Matrice de LED-uri 8×8

Conexiuni hardware

Matricea de LED-uri este conectată la placa Arduino Uno astfel:

VCC – conectat la pinul de 5V al plăcii Arduino (alimentare);

GND – conectat la pinul GND al plăcii Arduino (masă comună);

DIN (Data In) – conectat la pinul digital 6 al plăcii Arduino, utilizat pentru transmiterea datelor.

Detalii tehnice

În codul sursă:

pinul de date este definit prin #define LEDPIN 6;

dimensiunea matricei este definită prin:

#define W 8
#define H 8


Comunicarea cu LED-urile WS2812B se realizează printr-un protocol serial proprietar, care nu utilizează magistrale standard precum I2C sau SPI. Controlul și adresarea individuală a fiecărui LED sunt realizate cu ajutorul bibliotecii FastLED, care permite afișarea culorilor și animațiilor necesare funcționării jocului.

2.3 Matricea de butoane 4×4 (Keypad)
Descriere generală

Pentru introducerea comenzilor a fost utilizată o tastatură matricială 4×4, formată din 16 butoane. Aceasta funcționează pe principiul scanării pe rânduri și coloane, reducând semnificativ numărul de pini necesari.

Rol în proiect

Matricea de butoane este utilizată pentru:

selectarea pozițiilor pe tabla de joc (tastele 1–9);

activarea/dezactivarea modului Player vs Computer (tasta A);

posibile funcții suplimentare, ce pot fi adăugate ulterior.

Figura 2: Matrice de butoane 4×4

Conexiuni hardware

Matricea de butoane este conectată la placa Arduino Uno prin 8 pini digitali, organizați astfel:

Rânduri (ROWS)

R1 → pin digital 7

R2 → pin digital 8

R3 → pin digital 9

R4 → pin digital 10

Coloane (COLS)

C1 → pin digital 2

C2 → pin digital 3

C3 → pin digital 4

C4 → pin digital 5

Detalii tehnice

În codul sursă, aceste conexiuni sunt definite prin vectorii rowPins și colPins. Controlul tastaturii este realizat cu ajutorul bibliotecii Keypad, care:

gestionează scanarea rândurilor și coloanelor;

detectează apăsarea tastelor;

elimină efectul de bouncing.

Astfel, comenzile sunt introduse corect și stabil în cadrul jocului.

2.4 Concluzie privind partea hardware

Partea hardware a proiectului este simplă, eficientă și bine optimizată. Utilizarea unei matrice de LED-uri adresabile reduce numărul de conexiuni necesare, iar tastatura matricială permite controlul complet al jocului folosind un număr redus de pini.

Placa Arduino Uno gestionează eficient atât interfața cu utilizatorul, cât și logica jocului, oferind o soluție robustă și ușor de extins pentru dezvoltări viitoare.

Capitolul 3 – Prezentarea părții software
3.1 Arhitectura generală a aplicației

Partea software a proiectului este dezvoltată în Arduino IDE, folosind limbajul C/C++. Aplicația este organizată modular, fiecare componentă software având un rol bine definit în gestionarea logicii jocului, a interacțiunii cu utilizatorul și a controlului componentelor hardware.

Software-ul realizează legătura dintre:

tastatura matricială 4×4 (componenta de intrare);

matricea de LED-uri 8×8 (componenta de ieșire).

Sunt implementate regulile jocului X și O, precum și un adversar virtual pentru modul Player vs Computer.

3.2 Biblioteci utilizate

În cadrul proiectului sunt utilizate două biblioteci externe principale:

FastLED
Utilizată pentru controlul matricei de LED-uri RGB adresabile. Biblioteca permite setarea culorilor individuale pentru fiecare LED, controlul luminozității și actualizarea rapidă a afișajului.

Keypad
Utilizată pentru gestionarea tastaturii matriciale 4×4. Aceasta se ocupă de scanarea rândurilor și coloanelor, detectarea apăsării tastelor și eliminarea efectului de bouncing.

3.3 Structura datelor și variabile globale

La nivel software sunt definite constantele necesare configurării hardware-ului (dimensiunea matricei de LED-uri, nivelul de luminozitate, culorile utilizate în joc). De asemenea, sunt configurate rândurile și coloanele tastaturii matriciale, precum și mapa tastelor.

Variabilele globale sunt utilizate pentru:

stocarea tablei de joc (matrice 3×3);

reținerea jucătorului curent;

gestionarea modului de joc (Player vs Player sau Player vs Computer);

monitorizarea stării generale a jocului.

3.4 Reprezentarea grafică a jocului

Tabla de joc este reprezentată logic printr-o matrice 3×3, care este mapată pe matricea de LED-uri 8×8. Fiecare celulă a jocului este afișată ca un grup de LED-uri, asigurând o reprezentare clară și lizibilă.

Grila jocului este desenată prin linii fixe pe matricea de LED-uri, iar starea jocului este actualizată în timp real prin ștergerea afișajului, redesenarea grilei și afișarea mutărilor jucătorilor.

3.5 Logica jocului și condițiile de câștig

Software-ul include o logică dedicată pentru verificarea condițiilor de câștig, analizând:

liniile;

coloanele;

diagonalele tablei de joc.

În cazul detectării unui câștig, pozițiile corespunzătoare sunt evidențiate vizual prin animații de tip blink, folosind culoarea jucătorului câștigător. Este tratată și situația de remiză, atunci când toate pozițiile sunt ocupate fără a exista un câștigător.

3.6 Modul Player vs Computer

Modul Player vs Computer este implementat printr-o logică simplă de inteligență artificială. Calculatorul:

încearcă să identifice o mutare câștigătoare;

blochează o posibilă mutare câștigătoare a jucătorului;

ocupă poziția centrală, dacă este disponibilă;

alege o mutare liberă aleatorie.

Această abordare oferă un nivel de dificultate echilibrat și este potrivită pentru un sistem embedded cu resurse limitate.

3.7 Concluzie

Partea software coordonează eficient interacțiunea dintre utilizator și componentele hardware ale sistemului. Tastatura matricială permite introducerea comenzilor, iar matricea de LED-uri afișează în timp real starea jocului.

Prin utilizarea bibliotecilor FastLED și Keypad, aplicația asigură o funcționare stabilă, clară și ușor de extins, oferind o experiență coerentă de joc pe platforma Arduino Uno.
