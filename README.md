### Disclamer: This lab is heavily based on the 576 homework that you can find here: ###
#### https://github.com/holynski/cse576_sp20_hw1 ####

# Esercitazione 1 di AI-Lab #

## Setup generale ##
Questo è il setup da seguire per impostare il repo sulla vostra macchina personale. 
Più sotto trovate le istruzioni per impostarlo sulla macchina virtuale di laboratorio. 

**NB: è noevolmente più semplice impostare tutti i tool e le librerie necessarie su una distribuzione linux. 
In ogni caso non verrà dato supporto all'installazione da parte del docente e dei TA su macchine personali.**

### Scaricate il repository ###
Il repository si trova su github:

    git clone https://github.com/Sapienza-AI-Lab-a-a-2021-22/esercitazione1.git

### Installate CMake ###
Seguite le istruzioni che trovate qui:

    https://cmake.org/install/

### Compilazione ###
Potete usare la classica pipeline `cmake` e `make`, oppure lo script
`compile.sh`. In alternativa potete utilizzare un IDE come CLion per compilare tutto.

Go to the downloaded folder and here are some commands that can help you:
Per il file `compile.sh`, andate nella cartella del progetto e usate questi comandi:

    ./compile.sh # chiama cmake e make per voi e produce ./test0
    # questo è quello che vi serve per lavorare. I
    # n aggiunta potreste voler usare anche questo comandi:
    ./clean.sh # cancella tutti i file generati
    ./compile.sh # compila di nuovo

Per compilare manualmente:

    cd build
    rm -rf *
    cmake ..
    make -j4

### Esecuzione/Test ###

Ogni volta che fate dei cambiamenti al codice dovete compilare. Il programma da girare è 

    ./test0

che praticamente esegue dei test sui metodi delle classi che l'esercitazione prevede di implementare. 
Di base, senza modifiche, l'output che dovreste avere è questo:

    20 tests, 3 passed, 17 failed

Una volta che l'esercitazione sia completata con successo, dovreste avere questo output:

    20 tests, 20 passed, 0 failed

Il fatto che il vostro codice superi tutti i test è un buon segno, ma non assicura che sia totalmente corretto. 
Potete aggiungere altri test per rendere più robuste, o efficienti, le vostre implementazioni.  

## La struttura 'Image' ##

Nel file src/image.h trovate una struttura di base per le immagini. Questa struct `Image` contiene i metadati come width, 
height e il numero dei canali. I dati dell'immagine vera e propria sono contenuti in un array di float. 
La struttura ha questo aspetto:

    struct Image
    {
        int h,w,c;
        float *data;
        .......
    };

Per creare una nuova immagine, basta chiamare il costruttore:

    Image im(w,h,c);

La struttura ha diversi operatori per copiare, assegnare, passare come riferimento, etc.:

    Image im(5,6,7);
    Image im2=im;

    im=im2;

    void scramble_image(const Image& image);

Quando passate le immagini alle funzioni o metodi, se intendete passarle solo in lettura scrivete `const Image& image`, 
altrimenti usate `Image& image` per modificarle. Nel codice trovate esempi di questi due comportamenti.

Per accedere ai pixel potete usare questi metodi:

    float value = im(0,2,1); // gets the pixel at column 0, row 2 and channel 1
    im(3,0,2) = 0.76;  // sets the pixel at column 3, row 0 and channel 2

Vi daranno errore se mettere in input valori che vanno oltre le dimensioni dell'immagine. 

Sono fornite anche alcune funzioni (membri della classe e standalone) per caricare e salvare le immagini.
Peer caricare:

    Image im = load_image("image.jpg");
    im.load_image("another.png");
    im.load_binary("binary.bin");

Per salvere:

    im.save_image("output");   // save_image saves the image as a .jpg file
    im.save_png("output");     // save_png  saves the image as a .png file
    im.save_binary("output.bin") // save_binary saves as a raw binary file

Potete esplorare il file `image.h` e `struct Image {...}` e anche tutti i file correlati che implementano metodi e 
funzioni, se siete interessati. Il caricamento e il salvataggio usano la libreria stb_image, in quanto la gestione dei 
formati immagine è al di là dei nostri scopi. 
NB: gli unici file che dovranno essere modificati per questo esercizio sono 
`src/process_image.cpp` e `src/access_image.cpp`.


## 1. Accedere e modificare singoli pixel ##

L'operazione fondamentale di cui abbiamo bisogno è cambiare i pixel dell'immagine. Un'immagine nel nostro caso è un 
tensore tridimensionale che rappresenta le componenti di colore che compongono l'immagine:

![RGB format](figs/rgb.png)

Per convenzione l'origine del sistema di coordinate è in alto a sinistra:

![Image coordinate system](figs/coords.png)

Nel nostro array i dati dell'immagine sono memorizzati con la sequenza 'canale-altezza-profondità' (`CHW`). Vale a dire,
il primo pixel è quello del canale 0, riga 0, colonna 0, il secondo è canale 0, riga 0, colonna 1, il terzo canale 0, 
riga 0, colonna 2, e così via. L'operatore di accesso `image(1,2,1)` richiede di sapere l'indirizzo del pixel per 
operare e questo è l'oggetto del primo esercizio. 

Dovreste riempire la funzione che trovate in `src/access_image.cpp`:

    int pixel_address(const Image& im, int x, int y, int ch);

la funzione `pixel_address` dovrebbe restituire la posizione nell'arrey di dati del pixel che si trova in `x,y,ch`.
Fate caso che l'operatore `()` della struttura `Image` è un overload della funzione di accesso al pixel. Userà proprio 
la funzione che implementerete.
Anche se l'operatore di accesso `image(1,2,1)` farà il controllo del superamento degli estremi dell'immagine, è più semplice lavorare 
introducendo una strategia di padding. Ci sono diverse strategie per questo:

![Image padding strategies](figs/pad.png)

Useremo la strategia `clamp`, vale a dire che se il programmatore cerca di accedere al pixel in colonna -3, 
la funzione userà la colonna 0. Se invece cercherà di accedere alla colonna 300 per un'immagine 256x256, la funzione 
userà la colonna 255. 

Implementate le seguenti due funzioni:

    float get_clamped_pixel(const Image& im, int x, int y, int ch);
    void set_pixel(Image& im, int x, int y, int c, float value);

`set_pixel` dovrebbe uscire senza far nulla se passate delle coordinate fuori dagli estremi dell'immagine (out-of-bound).
In `get_clamped_pixel` fare  il padding dell'immagine. 
Potete usare l'operatore `()` per accedere ai pixel, ad esempio `im(3, 2, 0)`.

Possiamo testare in nostro codice sull'immagine del cane rimuovendo il canale rosso. Potete anche creare un altro 
eseguibile simile a `test0` per esplorare le funzioni che via via scriverete. Seguite l'esempio del `CMakeLists.txt` e
`src/test/test0.cpp` per farlo. 

    // 1-2. Getting and setting pixels
    Image im2 = load_image("data/dog.jpg");
    for (int i=0; i<im2.w; i++)
        for (int j=0; j<im2.h; j++)
            im2(i, j, 0) = 0;
    im2.save_image("output/set_pixel_result");

L'immagine del cane senza il canale rosso dovrebbe essere così:

![](figs/dog_no_red.jpg)


## 2. Copiare le immagini ##

Qualche volta avrete il bisogno di copiare una immagine. Per farlo dovete creare una nuova immagine della stessa 
dimensione e travasare i dati. Lo potete fare assegnando sistematicamente un pixel da una immagine all'altra usando 
dei cicli, o usando la funzione di libreria `memcpy`.

Ora implementate la funzione `void copy_image(Image& to, const Image& from)` contenuta in `src/access_image.cpp`.

## 3. Immagini in scala di grigi ##

Trasformare le immagini in scala di grigi non è un processo così ovvio come uno potrebbe supporre. Abbiamo visto a lezione
che la percezione umana compensa alcuni effetti fisici generando una percezione diversa (effetti ottici) a seconda della 
illuminazione generale dell'immagine. 
Inoltre noi siamo sensibili in maniera differente alle varie lunghezze d'onda, come abbiamo visto a lezione:

![Eye sensitivity to different wavelengths](figs/sensitivity.png)

Questo fa molta differenza nella pratica. Ad esempio questa colorbar:

![Color bar](figs/colorbar.png)

Se la trasformiamo in scala di grigi con un approccio naif, usando una media dei canali K = (R+G+B)/3, otteniamo un 
risultato che non combacia con la nostra percezione di brillantezza dei colori originali:

![Averaging grayscale](figs/avggray.jpg)

Proprio perché esiste questa differenza, è meglio usare una media pesata. Ci sono molti modi di farlo 
([gamma compression][1], [relative luminance](https://en.wikipedia.org/wiki/Relative_luminance)), e noi 
considereremo solo un modo molto semplice. 

Quella che usiamo è basata su di un valore chiamato [luma][2] che approssima intensità percepita in un immagine. Questa 
approssimazione lavora su immagini con correzione di gamma, come le immagini sRGB che abbiamo. 
La media pesata è questa:

    Y' = 0.299 R' + 0.587 G' + .114 B'

Usando questa formula dovrebbe permettere al file di test `test0.cpp` che contiene la funzione `test_grayscale()`
di dare questo output (`grayscale_result.jpg`):

![Grayscale colorbars](figs/gray.png)

Voi dovrete implementare questa conversione nella funzione `rgb_to_grayscale` in `process_image.cpp`. 
Fate in modo che la funzioe ritorni una nuova immagine che sia della stessa dimensione dell'originale, ma con un solo
canale contenente il valore luma.

## 4. Shifting dei colori dell'immagine ##

L'esercizio successivo vi chiede di aggiungere un valore costante ad un canale dell'immagine. Questo tipo di operazione
si fa per rendere, quando applicata a tutti i canali, l'immagine più brillante o più scura. Oppure puà essere usata
per cambiare solo uno dei colori.

Riempite la funzione  `void shift_image(image im, int c, float v);` inel file `process_image.cpp`. 
Dovrebbe aggiungere il valore `v` ad ogni pixel del canale `c`. Una volta implementata, provate a shiftare tutti i canali
di 0.4 (vale a dire 40%) e visualizzate l'immagine risultante:

    // 4. Shift Image
    shift_image(im, 0, .4);
    shift_image(im, 1, .4);
    shift_image(im, 2, .4);
    im.save_image("output/shift_result");

Vedrete che l'immagine ha un aspetto strano, le parti chiare sono diventate scure e viceversa. Questo avviene perché i 
pixel hanno superato il valore 1 e c'è stato overflow:

![Overflow](figs/overflow.jpg)

## 5. "Clamping" dei valori dell'immagine

Anche i valori dei pixel vanno limitati. In generale le immagini rappresentano i canali rosso, verde e blu con un byte 
ciascuno, senza segno, dando un intervallo di possibili valori che va da 0 a 255. 0 rappresenta nessuna luce di quel 
colore, 255 la massimo valore del colore primario.

Noi invece stiamo rappresentando i pixel con valori float compresi tra 0 e 1. Ma quando le salviamo su disco e le 
riapriamo con un software, i pixel vengono prima convertiti a tipo byte, generando l'overflow. Così valori grandi 
(maggiori di 1) sono diventati piccoli dopo l'overflow, e le zone brillanti sono diventate scure.

Vogliamo che i pixel rimangano nel range 0-1 qualuque sia l'operazione che subisono. 
Implementate questa funzionalità facendo in modo che ogni valore minore di 0 divenga 0 e ogni valore maggiore di 1 divenga 1.

La funzioe è `void clamp_image(image im);` dentro il file `process_image.cpp`, da implementare in modo da modificare 
l'immagine sul posto (senza creare una copia). Fatto questo, salvando il risultato come nel file `test.c`:

    // 5. Clamp Image
    clamp_image(im);
    im.save_image("output/clamp_result");

dovreste vedere questo `clamp_result.jpg`:

![](figs/fixed.jpg)

## 6. Conversione da RGB a Hue, Saturation, Value (HSV) ##

Fin'ora abbiamo lavorato solo con lo spazio RGB, ma ci sono altri spazi per descrivere il colore. Qui lavoreremo su uno
dei più comuni, cioè [Hue, Saturation, and Value (HSV)](https://en.wikipedia.org/wiki/HSL_and_HSV). Da un punto
di vista geometrico è come trasformare uno spazion cubico in uno cilindrico:

![RGB HSV conversion](figs/convert.png)

[Hue](https://en.wikipedia.org/wiki/Hue) può essere inteso come il colore di base. 
[Saturation](https://en.wikipedia.org/wiki/Colorfulness#Saturation) è l'intensità del colore confrontata con il bianco 
(il colore meno saturo).
[Value](https://en.wikipedia.org/wiki/Lightness) è la luminosità percepita confrontata con il nero. Potere provare 
questa [demo](http://math.hws.edu/graphicsbook/demos/c2/rgb-hsv.html) per avere un'impressione delle differenze tra i due
spazi. Per una interpretazione geometrica si può fare riferimento a questa figura:

![RGB to HSV geometry](figs/rgbtohsv.png)

Ci sono molti problemi con questo spazio di colore ([lots of issues](http://poynton.ca/notes/colour_and_gamma/ColorFAQ.html#RTFToC36)).
Tuttavia è semplice da implementare e ci aiuta a capire questo concetto. 
La componente Value si calcola così:

    V = max(R,G,B)

La saturazione è una misura di quanto il colore è intenso rispetto al grigio neutrale o al bianco. 
I colori si dicono neutrali quando hanno la stessa quantità dei tre colori primari. Quindi per calcolare la saturazione 
si cerca di capire la distanza dal colore neutro più vicino. Questo si fa prima calcolando la componente minima:

    m = min(R,G,B)

poi si vede quanto sono distanti il minimo e il massimo (che abbiamo già calcolato con V):

    C = V - m

infine la Saturazione sarà il rapporto tra la differenza e il massimo (differenza normalizzata):

    S = C / V

C'è una eccezione: se tutte e tre R, G e B sono nulli, V risulta nullo e la Saturazione non si può calcolare così. 
In questo caso, si imposta Saturation = 0.

Infine, per calcolare lo Hue dobbiamo calcolare quanto distante un colore è sulla ruota dei colori. Nel nostro caso
si rappresenta con un esagono:

![color hex](figs/hex.png)

Si inizia a contare dal Rosso. Ogni passo sull'esagono conta come una unità di distanza. La distanza tra i punti è misurata
come il rapporto relativo dei colori secondari. Si può usare la formula seguente ([Wikipedia](https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma)):

<img src="figs/eq.svg" width="256">

Se il colore è un grigio, cioè C = 0, tutti i canali RGB sono uguali e il colore è una varietà di grigio. In questo
caso non c'è un valore corretto di Hue, in quanto ci troviamo nel centro del cilindro. Nel codice semplicemente impostiamo
H = 0 se C = 0.

Notate che H = \[0,1) e che dovrà essere circolare quando il valore supera l'1. Per questo si controlla se è negativo 
e si aggiunge 1 nel caso. Ci sono altri metodi in cui H è codificato come un numero tra 0 e 6, o tra 0 e 360. Per salvare 
questa rappresentazione dell'immagine, semplicemente copieremo H sul canale R, S su canale G e V sul canale B.

Riempite `void rgb_to_hsv(Image& im)` in `process_image.cpp` per convertire l'immagine RGB in HSV sul posto. Visto che 
è pixel a pixel, potere iterare e convertire ciascun pixel indipendentemente dagli altri.

## 7. HSV to RGB ##

Ora implementiamo la trasformazione inversa:

```

// Given the H, S, V channels of an image:
C = V * S
X = C * (1 - abs((6*H mod 2) - 1))  // You can use the fmod() function to do a floating point modulo.
m = V - C
```

Nota: fate attenzine ad usare i floating point, o avrete divisioni intere che arrotondano a 0. 

<img src="figs/hsv_to_rgb.svg" width="256">


```
(R, G, B) = (R'+m, G'+m, B'+m)
```

Riempite `void hsv_to_rgb(Image& im)` in `process_image.cpp` con una trasfomazione sul posto.

In `test.c` converte un immagine HSV, aumenta la saturazione, e poi la ritrasforma in RGB: 

    // 6-7. Colorspace and saturation
    Image im2 = load_image("data/dog.jpg");
    rgb_to_hsv(im2);
    shift_image(im2, 1, .2);
    clamp_image(im2);
    hsv_to_rgb(im2);
    im2.save_image("output/colorspace_result");

![Saturated dog picture](figs/dog_saturated.jpg)

Notate che ci sono alcuni piccoli artefatti perché cerchiamo di aumentare la saturazione in aree che hanno poco colore. 
Invece di fare lo shift è meglio moltiplicare il valore per ottenere un risultato più uniforme.

## 8. Scalatura dei canali ##

Implementate `void scale_image(image im, int c, float v);` per scalare un canale di un certo valore. Questo darà un
miglior effetto di saturazione/desaturazione. Dovrete aggiungere la definizione nei file header. Dovrebbe essere simile
a quello che c'è per `shift_image`. Adesso usando una scalatura di `2` invece di fare lo shift, si ottiene un risultato
migliore:

    Image im = load_image("data/dog.jpg");
    rgb_to_hsv(im);
    scale_image(im, 1, 2);
    clamp_image(im);
    hsv_to_rgb(im);
    im.save_image("output/dog_scale_saturated");

![Dog saturated smoother](figs/dog_scale_saturated.jpg)

[//]: # (## 9. Conversione a LUV e viceversa ##)

[//]: # ()
[//]: # (Implementate la traformazione da RGB a [Hue, Chroma, Lightness]&#40;https://en.wikipedia.org/wiki/CIELUV#Cylindrical_representation_.28CIELCH.29&#41;,)

[//]: # (una versione più accurata dal punto di vista della percezione umana. Questa include la gamma decompressioin, la converione )

[//]: # (a CIE XYZ, poi a CIE LUV, infine a HCL. Implementate anche la trasformazione inversa.)

[1]: https://en.wikipedia.org/wiki/SRGB#The_sRGB_transfer_function_("gamma")
[2]: https://en.wikipedia.org/wiki/Luma_(video)

