# Physically based rendering

Physically based rendering jest zbiorem technik renderowania, które bazują na teorii, która ma za zadanie odwzorowywać świat rzeczywisty. 

Podstawową dla obliczenia jest *rendering equation* następującej postaci.

$$L_o(p,\omega_o)=\int_\Omega f_r(p,\omega_i,\omega_o)L_i(p,\omega_i)n\cdot \omega_i\ d\omega_i$$
Całka przechodzi po wszystkich kierunkach na półkuli $\Omega$ oblicza wyjściowe oświetlenie $L_O$ bazując na wejściowym oświetleniu $L_i$ i funkcji $f_r$ znanej jako **BRDF**, czyli *bidirectional reflective distribution function*. W najbardziej ogólnym przypadku należałoby traktować $L_i$ jako dystrybucję i jest nadmiernym formalizmem w naszym przypadku, gdy będziemy wykorzystywać wyłącznie światła punktowe i kierunkowe. Dlatego o powyższej całce możemy myśleć jak o sumie po źródłach światła. 

By uzyskać realistyczny efekt, potrzebujemy realistycznej funkcji $f_r$. Wybierzemy *Cook-Torrance BRDF*, który jest najczęściej wykorzystywaną funkcją przy liczeniu PBR w czasie rzeczywistym. 

 *Cook-Torrance BRDF* rozdzielamy na światło rozproszone (diffuse) i odbite kierunkowo (specular) 

$$f_r=k_d f_{lambert}+k_s f_{cook−torrance},$$
gdzie $k_d$ i $k_s$ to są współczynniki, rozdzielające ile światła zostało rozproszone a ile odbite, ich suma musi być równa 1. komponent $f_{lambert}$ jest znany jako *Lambertian diffuse* i wynosi
$$f_{lambert}=\frac{c}{\pi}$$
przy czym $c$ to jest jest kolorem powierzchni a $\pi$ odpowiada za normalizację. 

Część odbicia kierunkowego jest bardziej skomplikowana i wygląda następująco:

$$f_{cook−torrance}=\frac{DFG}{4(w_o\cdot n)(w_i\cdot n)}$$
gdzie $D$, $F$ i $G$ są funkcjami, które zależą od normalnej, wektora wejściowego i wyjściowego oraz parametrów chropowatości $\alpha$ i odbicia $F_0$. Te funkcje to odpowiednio:
- $D$ - **Normal distribution function** przybliża ilość powierzchni, która jest ustawiona prostopadle do wektora połówkowego korzystamy z funkcji Trowbridge-Reitz GGX $$D(n,h,\alpha) = \frac{\alpha^2}{π((n⋅h)^2(α^2−1)+1)^2},$$
gdzie $h$ to: $$h = \frac{\omega_o+\omega_i}{||\omega_o+\omega_i||}$$
- G - **Geometry function** opisuje stopień samo-zacieninienia. Wykorzystujemy Schlick-GGX
$$G(n,\omega_o,k)=\frac{n⋅\omega_o}{(n⋅\omega_o)(1−k)+k}$$
przy czym $k$ wynosi 
$$k=\frac{(\alpha+1)^2}{8}$$
- F -  **Fresnel equation** opisuje stopień odbicia w zależności od kąta padania. Wykorzystujemy aproksymację Fresnela-Schlicka 
$$F(h,v,F_0)=F_0+(1−F_0)(1−(h⋅v))^5$$
Parametr $F_0$ jest własnością materiału dla uproszczenia uznaje się, że niemetale mają wartość (0.04,0.4,0.4) a dla metali jest ona równa kolorowi obiektu. Wprowadza się **parametr metaliczności**, który jest współczynnikiem, jakim miksujemy między (0.04,0.4,0.4) a kolorem, żeby uzyskać $F_0$.

### Zadanie
W projekcie znajduje się jedna kula, punktowe oświetlenie i cieniowanie Phonga. Zaimplementuj PBR i rozmieść kule w macierzy 10 x 10 zmieniając stopniowo w nich parametr chropowatości $\alpha$ i metaliczności, jak na poniższym obrazku. 
![](./img/lighting_result.png)

## Teksturowanie 
Do uzyskania realistycznego efektu wykorzystuje się szereg tekstur:

![](./img/textures.png)
Tekstury **Albedo** i **Normalnych** odpowiadają za kolor i normalne jak przy cieniowaniu Phonga. **Metalic** i **Roughness** przechowuje wartość metaliczności i chropowatości, które pojawiły się w BRDF. Natomiast **AO** (ambient occlision) odpowiada za stopień samo-zacienienia, który modyfikuje oświetlenie ambientowe w danym punkcie, uwzględnienie jej podkreśla detale obiektów.

Karty graficzne posiadają ograniczenia, jeżeli chodzi o ilość tekstur, które można przesłać za jednym razem (współcześnie są to 32 tekstury). Może być to znaczące ograniczenie, dlatego **AO** **Roughness** i **Metalic** mogą być przesłane w jednej teksturze kolejno jako kanał r, g i b. Taką teksturę określa się jako arm od pierwszych liter nazw. 

### Zadanie 
W `ex_8_2.hpp` znajduje się podstawowa scena. Zaimplementuj w niej PBR z użyciem tekstur. 
Stwórz nową parę shaderów, które będą obsługiwać PBR![](./img/textures.png)W oparciu o tekstury. Wyświetl statek zestawem tekstur w folderze  `textures/spaceshipPBR`. Możesz też wziąć swój układ słoneczny z poprzednich zajęć. 
Ładowanie tekstur jest męczące, napisz klasę/strukturę `PBRTexturesHandler`, która przechowuje zestaw tekstur *albedo*, *normal* i *arm*. Posiada konstruktor, który ładuje tekstury po ścieżkach oraz funkcję `activateTextures(int[] indices)`, która aktywuje tekstury z indeksami podanymi w tablicy. `int[] indices`.

## PBR Multitexturing 
Podobnie jak w przypadku zwykłych tekstur możemy mieszać tekstury przesyłane przez PBR, należy wtedy mieszać wszystkie tekstury z takim samym współczynnikiem. Jednak stopniowe przejście z jednej tekstury do drugiej często prowadzi to do nienaturalnych rezultatów, gdy próbujemy mieszać  tekstury o różnej częstotliwości detali, jak na przykład poniżej:
![](./img/blending1.webp)
W prawdziwym świecie, przy stopniowej zmianie z kamiennego podłoża na piaszczyste, wiedzielibyśmy, jak piasek stopniowo zaczyna wypełniać szpary między kamieniami i przykrywać je coraz bardziej. 
Istnieje wiele metod mieszania tekstur np [ta](https://onlinelibrary.wiley.com/doi/10.1002/cav.1460). My wykorzystamy technikę opartą na mapach wysokości, opisaną w 
[https://www.gamedeveloper.com/programming/advanced-terrain-texture-splatting](https://www.gamedeveloper.com/programming/advanced-terrain-texture-splatting).

>	Mapa wysokości to dodatkowa tekstura, która wskazuje wysokość obiektów znajdujących się na teksturze względem płaskiej powierzchni. Może być również wykorzystana przy paralax mappingu czy tessalacji. Jest ona nazywana po angielsku *heightmap*, *displacement map*  czy *bump map* w zależności od zastosowania. Przykładowe znajdują się w teksutrach.

Możemy te mapy wykorzystać do mieszania dwóch map tak, że wyświetlana będzie ta, która jest wyżej. Zaprezentować to możemy w poniższym jednowymiarowym schemacie. Niebieska linia reprezentuje mapę piasku a czerwona kamieni.
![](./img/2.png)
Jako rezultat otrymujemy
![](./img/3.webp)

Kod funkcji mieszającej:
```C++
float3 blend(float3 texture1, float height1, float3 texture2, float height2, float blend_ratio)
{
	if (height1>height2){
		return texture1;
	}
	else{
		return texture2;
	}
}
```

To pozwala nam zmieszać dwie tekstury. Jednak samo w sobie nie powoduję przejścia z jednej do drugiej, to możemy osiągnąć modyfikując wysokość parametrem `blend_ratio`, który będzie z zakresu od 0 do 1. Na jednowymiarowym schemacie wartość wysokości wygląda to następująco.
![](./img/4.png)
Daje nam to to poniższy efekt
![](./img/5.webp)
Kod nowej funkcji mieszającej:
```C++
float3 blend(float3 texture1, float height1, float3 texture2, float height2, float blend_ratio)
{
	float h1=height1+1.0-blend_ratio;
	float h2=height2+blend_ratio;
	if (h1>h2){
		return texture1;
	}
	else{
		return texture2;
	}
}
```
To podejście dało nam bardziej naturalne przejście z piasku do kamieni. Widać jak kamienie stopniowo wyłaniają się spod piasku. Jednak pojawiły się artefakty w miejscach, gdzie wartości mapy wysokości są bardzo zbliżone. Wynika to z ograniczeń kwantyzacji. By zminimalizować te błędy, będziemy mieszać wartości tekstur, ale tylko, gdy wartości będą blisko siebie. 
![](./img/6.webp)
Zdefiniujemy dodatkowy parametr `mix_threshold`, który będzie określał, jak blisko mają być wartości, żeby je mieszać. Jeżeli różnica między jedną wysokością a drugą będzie mniejsza od `mix_threshold` to będziemy je mieszać proporcjonalnie do ich różnicy podzielonej przez `mix_threshold`, w przeciwnym wypadku wybierzemy tą, która jest wyżej. W celu optymalizacji zamiast optymalizacji korzystamy z funkcji clamp

```C++
float3 blend(float3 texture1, float height1, float3 texture2, float height2, float blend_ratio)
{
	float mix_threshold=0.1;
	float h1=height1+1.0-blend_ratio;
	float h2=height2+blend_ratio;
	float havg=(h1+h2)/2.;
	
	h1 = clamp((h1-hav+0.5*mix_threshold)/(mix_threshold),0.,1.);
    h2 = clamp((h2-hav+0.5*mix_threshold)/(mix_threshold),0.,1.);
    
	return (texture1*h1+texture2*h2)
}
```

Stosunek w jakim je zmieszamy:
![](./img/7.webp)

Finalna tekstura
![](./img/8.webp)

### Zadanie
W teksturach znajduje plik `heightmap.png` wykorzystaj go jako mapę wysokości przy rysowaniu jednej z planet. Mapa ta zawiera wartości, od 0 do 1. Dla różnych wysokości chcemy wykorzystywać różne tekstury, ustal na przykład, że poniżej 0,3 znajduje się woda, między 0,3 a 0,6 trawa, natomiast powyżej skały. 

### Zadanie*
Zmodyfikuj swój układ słoneczny z poprzednich zajęć, zmień w nim model oświetlenia z Phonga na PBR. Tekstury możesz pobrać na przykład z [https://polyhaven.com/textures](https://polyhaven.com/textures)
lub [www.texturecan.com](www.texturecan.com).