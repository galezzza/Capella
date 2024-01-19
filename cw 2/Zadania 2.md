## Ładowanie VAO i VBO

W trakcie tych zajęć przećwiczymy ładowanie danych do pamięci. Tym razem, zamiast korzystać z gotowej funkcji utworzymy je samodzielnie. W openglu wykorzystujemy do tego VBO (Vertex Buffer Object) i VAO (Vertex Array Object). Pierwsza jest buforem, który zawiera dane modeli. Natomiast drugi zawiera informacje jak dane bufory interpretować. 

W zadaniu `2_1` będziemy przesyłać prostopadłościan. Tablica zawierająca jego definicję jest w pliku `Box.cpp`. Każdy wierzchołek składa się z ośmiu floatów, pierwsze cztery określają jego pozycję, a cztery kolejny określają jego kolor. 

Inicjalizacje będziemy wykonywać wewnątrz funkcji `init`. Pierwszym krokiem jest wygenerowanie jednego VAO i jednego VBO. Wykorzystuje się do tego odpowiednio funkcje `glGenVertexArrays` i  `glGenBuffers`. Pierwszym argumentem jest liczba buforów czy array object, które tworzymy, drugim jest adres, w którym ma być bufor/array object umieszczony. 
W naszym przypadku pierwszym argumentem będzie 1, natomiast drugim będzie wskaźnik na zmienną `VAO` i `VBO` odpowiednio. Następnie należy aktywować `VAO` za pomocą funkcji `glBindVertexArray`, po czym podpiąć do niego bufor `VBO` za pomocą `glBindBuffer(GLenum target, GLuint buffer)` nasz target to `GL_ARRAY_BUFFER`, czyli bufor, który oznacza atrybuty wierzchołków. 

Kolejnym krokiem jest umieszczenie danych w buforze za pomocą funkcji `glBufferData(GLenum target, GLsizeiptr size, const void * data, GLenum usage)`.  Pierwszym argumentem jest ponownie `GL_ARRAY_BUFFER`, drugi to rozmiar tablicy w bajtach, trzecim adres tablicy, a czwartym sposób używania tablicy, w naszym przypadku `GL_STATIC_DRAW`.

Pozostaje opisanie atrybutów wierzchołków, musimy opisać, gdzie się znajdują, jaką mają strukturę i jak ma się do nich odnieść shader. My mamy 2 atrybuty, jest to pozycja i kolor. Pierwszym krokiem jest aktywacja atrybutów za pomocą `glEnableVertexAttribArray(GLuint index)`, przy czym po indeksie będą one odnajdywane przez shader. W naszym przypadku będą to odpowiednio 0 i 1. Następnie należy opisać jak GPU ma odczytywać atrybuty z bufora za pomocą funkcji `glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * offset)`. Jej argumenty to kolejno:
* `index` - indeksy odpowiadające atrybutowi,
* `size` - liczba elementów w atrybucie wierzchołka, może wynosić 1, 2, 3 lub 4,
* `type` - typ danych jako enum, w naszym przypadku `GL_FLOAT`,
* `normalized` - określa czy wartość ma być znormalizowana, u nas będzie to `GL_FALSE`,
* `stride` - określa dystans pomiędzy atrybutami w kolejnych wierzchołkach
* `offset` - wskaźnik na pierwszy atrybut w tablicy, licząc względem początku tablicy i typu (void \*)

Struktura naszego prostopadłościanu ma na przemian pozycje i kolory, dlatego w obu przypadkach stride będzie wynosił ośmiokrotność rozmiaru *floata*. Natomiast *offest* będzie wynosił zero i czterokrotność rozmiaru *floata*. 

![](./img/stride_offest.jpg)

Na koniec uwolnij `VAO` za pomocą instrukcji: `glBindVertexArray(0);`.

Pozostaje narysować prostopadłościan. W funkcji `renderScene` wywołaj `glBindVertexArray` z argumentem `VAO`. Następnie narysuj za pomocą `glDrawArrays(GLenum mode, GLint first, GLsizei count)`.  Pierwszym argumentem jest typ rysowanego obiektu, w naszym przypadku jest to `GL_TRIANGLES`, indeks pierwszego wierzchołka, czyli 0 i liczba wierzchołków czyli 36.

### Zadanie 
podążając za powyższymi instrukcjami zainicjalizuj box, następnie obróć go za pomocą funkcji `glm::eulerAngleXYZ` tak, żeby było widać trzy ściany prostopadłościany. Dodaj również obrót wokół osi Y w czasie z użyciem funkcji `glm::eulerAngleXYZ`.

### Zadanie* 
Wykonaj zadania z `ex_2_1b.hpp`. 


## Shadery

Shadery są programami uruchamianymi na karcie graficznej. W openglu wykorzystujemy język GLSL, który jest bardzo podobny do C++, posiada on liczne słowa kluczowe i funkcje matematyczne. Istnieją różne rodzaje shaderów, w tej sekcji skupimy się na dwóch z nich: shader wierzchołków i shader fragmentów. Pierwszy rodzaj wykonują operacje na wierzchołkach, przykładowo w tym zadaniu odpowiada za przemnożenie macierzy obrotu przez wierzchołki. Natomiast drugi określa kolor konkretnego fragmentu/piksela. Shadery są łączone w *pipeline*, to znaczy wykonuje się je sekwencyjnie, dane z poprzedniego są wysyłane do następnego. 
W zadaniu `2_1` wykorzystujemy następujące shadery

shader wierzchołków
```GLSL
#version 430 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 transformation;


void main()
{
	gl_Position = transformation * vertexPosition;
}
```

shader fragmentów
```GLSL
#version 430 core


out vec4 out_color;
void main()
{
	out_color = vec4(0.8,0.2,0.9,1.0);
}
```

Shader wierzchołków odbiera 2 typy danych. Pierwszym są dane z bufora w liniach.
```
layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec4 vertexColor;
```
są one różne dla każdego wierzchołka. Zmienną, która ma odebrać te dane deklaruje się globalnie funkcją `main` i poprzedza się słowem kluczowym `in`.  Prefiks `layout(location = ..)` jest opcjonalny i służy określeniu indeksu atrybutu, jest to ta sama wartość, którą ustawiliśmy w `glVertexAttribPointer`. Można je usunąć, wtedy o indeksie będzie decydować kolejność.
Drugim typem jest `uniform`, w przeciwieństwie do danych z bufora, są one takie same dla każdego wierzchołka. W tym przypadku przesyłamy za jej pomocą macierz obrotu. 

Shader również wysyła dane. Domyślnie musi wysłać wyjściową pozycję wierzchołka, robi to przez zapisanie w `gl_Position` wektora 4-wymiarowego w funkcji `main`. Poza tym może również przesłać inne informacje. Wykonuje się to przez deklaracje zmiennej globalnej, którą poprzedza się słowem kluczowym `out`. Następnie należy ją wypełnić. W tym przypadku przesyłamy kolor wierzchołka. 

Shader fragmentów odbiera kolor z Shadera wierzchołków. Podobnie jak z obieraniem danych z buforu robimy to za pomocą słowa kluczowego `in`, w przypadku przesyłania zmiennej z jednego shadera do drugiego nazwy zmiennych muszą być **takie same** przy słowie kluczowym `out` i `in`. Zmiennej obranej nie można modyfikować.

W najnowszej wersji opengla fragment shader nie ma domyślnego wyjścia na kolor, musimy sami je z definiować. Robimy to instrukcją `out vec4 out_color` następnie w funkcji `main` przypisujemy mu jakąś wartość. 

### Zadanie 
W tej chwili nasz prostopadłościan jest jednolitego koloru i nie możemy rozróżnić jego ścian. Przesłana przez nas wcześniej informacja o kolorze nie została wykorzystana. Bazując na powyższych informacjach prześlij wartość koloru zapisaną w `vertexColor` do shadera fragmentu i przypisz ją do wyjściowego koloru. Dodaj zmienną `in vec4 color` w shaderze fragmentów, następnie w funkcji `main` przypisz do niej wartość koloru. W shaderze fragmentów odbierz ją za pomocą `out` i przypisz do wyjściowego koloru.

Zauważ, że kolor ścian nie jest jednolity, zamiast tego przechodzą gradientem od jednego koloru do drugiego. Dzieje się tak, ponieważ na etapie rasteryzacji kolor jest interpolowany. To znaczy: wartość jest uśredniana pomiędzy wierzchołkami trójkąta. 

### Zadanie
Sprawdź, jak będzie wyglądać prostopadłościan z wyłączoną interpolacją. Dodaj przed `in` i `out` ` color` słowo kluczowe `flat`.

### Zadanie
Prześlij czas od startu aplikacji do shadera fragmentów. Użyj funkcji `glfwGetTime`, by uzyskać czas. Utwórz zmienną `uniform` typu `float` we shaderze fragmentów. Następnie prześlij do niej wynik funkcji `glfwGetTime`. Aby przesłać czas do shadera, wykorzystaj funkcje `glUniform1f`. Pierwszym argumentem jest lokacja uniforma, drugim przypisywana wartość. Lokację uzyskamy za pomocą funckji `glGetUniformLocation(progam,"time")` pierwszym argumentem jest program, którego używamy a drugim nazwa zmiennej uniform. 
Podziel kolor przez czas, by uzyskać efekt, w którym prostopadłościan robi się czarny.

### Zadanie* 
Wykorzystaj przesłany czas, żeby sprawić, żeby prostopadłościan znikał przez mieszanie go z kolorem tła. Wykorzystaj do tego następujące funkcje GLSL: `mix`, `sin`, `vec4`. Opis ich działania możesz znaleźć w dokumentacji https://docs.gl/.

### Zadanie  

Prześlij pozycję lokalną i globalną pozycję wierzchołków do shadera fragmentów i wyświetl ją.

W shaderze wierzchołków obok deklaracji `out vec4 color;` dodaj analogiczne o nazwie `pos_local` i `pos_global`. Do `pos_local` przypisz `vertexPosition`, a do `pos_global` przypisz `transformation * vertexPosition`. Podobnie dopisz odebranie ich w shaderze fragmentów. Użyj `pos_local`, następnie `pos_global` jako zamiast koloru. Dlaczego otrzymaliśmy taki efekt?

### Zadanie*
Użyj jednej ze zmiennych z poprzedniego zadania do zrobienia pasków na przynajmniej jednej ze ścian sześcianu. Wykorzystaj czas, żeby paski się przesuwały. 

