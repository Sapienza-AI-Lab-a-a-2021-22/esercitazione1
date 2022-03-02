# Disclamer: This lab is heavily based on the 576 homework that you can find here: # 
## https://github.com/holynski/cse576_sp20_hw1 ##

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

### Compile ###
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

### Run/Test ###

Ogni volta che fate dei cambiamenti al codice dovete compilare. Il programma da girare è 

    ./test0

che praticamente esegue dei test sui metodi delle classi che l'esercitazione prevede di implementare. 
Di base, senza modifiche, l'output che dovreste avere è questo:

    17 tests, 3 passed, 14 failed

Una volta che l'esercitazione sia completata con successo, dovreste avere questo output:

    17 tests, 17 passed, 0 failed

Il fatto che il vostro codice superi tutti i test è un buon segno, ma non assicura che sia totalmente corretto. 
Potete aggiungere altri test per rendere più robuste, o efficienti, le vostre implementazioni.  

## Image basics ##

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


## 1. Getting and setting pixels ##

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

We will use the `clamp` padding strategy. This means that if the programmer asks for a pixel at column -3, 
use column 0, or if they ask for column 300 and the image is only 256x256 you will use column 255 
(because of zero-based indexing).

Useremo la strategia `clamp`, vale a dire che se il programmatore cerca di accedere al pixel in colonna -3, 
la funzione userà la colonna 0. Se invece cercherà di accedere alla colonna 300 per un'immagine 256x256, la funzione 
userà la colonna 255. 

Implementate le seguenti due funzioni:

    float get_clamped_pixel(const Image& im, int x, int y, int ch);
    void set_pixel(Image& im, int x, int y, int c, float value);

`set_pixel` dovrebbe uscire senza far nulla se passate delle coordinate fuori dagli estremi dell'immagine (out-of-bound).
In `get_clamped_pixel` fare  il padding dell'immagine. 
Potete usare l'operatore `()` per accederer ai pixel, ad esempio `im(3, 2, 0)`.

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


## 2. Copying images ##

Qualche volta avete il bisogno di copiare una immagine. Per farlo dovete creare una nuova immagine della stessa 
dimensione e travasare i dati. Lo potete fare assegnando sistematicamente un pixel da una immagine all'altra usando 
dei cicli, o usando la funzione di libreria `memcpy`.

Ora implementate la funzione `void copy_image(Image& to, const Image& from)` contenuta in `src/access_image.cpp`.

## 3. Grayscale image ##

Now let's start messing with some images! People like making images grayscale. It makes them look... old? Or something? Let's do it.

Remember how humans don't see all colors equally? Here's the chart to remind you:

![Eye sensitivity to different wavelengths](figs/sensitivity.png)

This actually makes a huge difference in practice. Here's a colorbar we may want to convert:

![Color bar](figs/colorbar.png)

If we convert it using an equally weighted mean K = (R+G+B)/3 we get a conversion that doesn't match our perceptions of the given colors:

![Averaging grayscale](figs/avggray.jpg)

Instead we are going to use a weighted sum. Now, there are a few ways to do this. If we wanted the most accurate conversion it would take a fair amount of work. sRGB uses [gamma compression][1] so we would first want to convert the color to linear RGB and then calculate [relative luminance](https://en.wikipedia.org/wiki/Relative_luminance).

But we don't care about being toooo accurate so we'll just do the quick and easy version instead. Video engineers use a calculation called [luma][2] to find an approximation of perceptual intensity when encoding video signal, we'll use that to convert our image to grayscale. It operates directly on the gamma compressed sRGB values that we already have! We simply perform a weighted sum:

    Y' = 0.299 R' + 0.587 G' + .114 B'

Using this conversion technique we get a pretty good grayscale image! Now `test_grayscale()` in `test0.cpp` will output `grayscale_result.jpg`.

![Grayscale colorbars](figs/gray.png)

Implement this conversion for the function `rgb_to_grayscale` in `process_image.cpp`. Return a new image that is the same size but only one channel containing the calculated luma values.

## 4. Shifting the image colors ##

Now let's write a function to add a constant factor to a channel in an image. We can use this across every channel in the image to make the image brighter or darker. We could also use it to, say, shift an image to be more or less of a given color.

Fill in the code for `void shift_image(image im, int c, float v);` in `process_image.cpp`. It should add `v` to every pixel in channel `c` in the image. Now we can try shifting all the channels in an image by `.4` or 40%. See lines 82-86 in `test.c`:

    // 4. Shift Image
    shift_image(im, 0, .4);
    shift_image(im, 1, .4);
    shift_image(im, 2, .4);
    im.save_image("output/shift_result");

But wait, when we look at the resulting image `shift_result.jpg` we see something bad has happened! The light areas of the image went past 1 and when we saved the image back to disk it overflowed and made weird patterns:

![Overflow](figs/overflow.jpg)

## 5. Clamping the image values

Our image pixel values have to be bounded. Generally images are stored as byte arrays where each red, green, or blue value is an unsigned byte between 0 and 255. 0 represents none of that color light and 255 represents that primary color light turned up as much as possible.

We represent our images using floating point values between 0 and 1. However, we still have to convert between our floating point representation and the byte arrays that are stored on disk. In the example above, our pixel values got above 1 so when we converted them back to byte arrays and saved them to disk they overflowed the byte data type and went back to very small values. That's why the very bright areas of the image looped around and became dark.

We want to make sure the pixel values in the image stay between 0 and 1. Implement clamping on the image so that any value below zero gets set to zero and any value above 1 gets set to one.

Fill in `void clamp_image(image im);` in `process_image.cpp` to modify the image in-place. Then when we clamp the shifted image and save it we see much better results, see lines 87-89 in `test.c`:

    // 5. Clamp Image
    clamp_image(im);
    im.save_image("output/clamp_result");

and the resulting image, `clamp_result.jpg`:

![](figs/fixed.jpg)

## 6. RGB to Hue, Saturation, Value ##

So far we've been focussing on RGB and grayscale images. But there are other colorspaces out there too we may want to play around with. Like [Hue, Saturation, and Value (HSV)](https://en.wikipedia.org/wiki/HSL_and_HSV). We will be translating the cubical colorspace of sRGB to the cylinder of hue, saturation, and value:

![RGB HSV conversion](figs/convert.png)

[Hue](https://en.wikipedia.org/wiki/Hue) can be thought of as the base color of a pixel. [Saturation](https://en.wikipedia.org/wiki/Colorfulness#Saturation) is the intensity of the color compared to white (the least saturated color). The [Value](https://en.wikipedia.org/wiki/Lightness) is the perception of brightness of a pixel compared to black. You can try out this [demo](http://math.hws.edu/graphicsbook/demos/c2/rgb-hsv.html) to get a better feel for the differences between these two colorspaces. For a geometric interpretation of what this transformation:

![RGB to HSV geometry](figs/rgbtohsv.png)

Now, to be sure, there are [lots of issues](http://poynton.ca/notes/colour_and_gamma/ColorFAQ.html#RTFToC36) with this colorspace. But it's still fun to play around with and relatively easy to implement. The easiest component to calculate is the Value, it's just the largest of the 3 RGB components:

    V = max(R,G,B)

Next we can calculate Saturation. This is a measure of how much color is in the pixel compared to neutral white/gray. Neutral colors have the same amount of each three color components, so to calculate saturation we see how far the color is from being even across each component. First we find the minimum value

    m = min(R,G,B)

Then we see how far apart the min and max are:

    C = V - m

and the Saturation will be the ratio between the difference and how large the max is:

    S = C / V

Except if R, G, and B are all 0. Because then V would be 0 and we don't want to divide by that, so just set the saturation 0 if that's the case.

Finally, to calculate Hue we want to calculate how far around the color hexagon our target color is.

![color hex](figs/hex.png)

We start counting at Red. Each step to a point on the hexagon counts as 1 unit distance. The distance between points is given by the relative ratios of the secondary colors. We can use the following formula from [Wikipedia](https://en.wikipedia.org/wiki/HSL_and_HSV#Hue_and_chroma):

<img src="figs/eq.svg" width="256">

There is no "correct" Hue if C = 0 because all of the channels are equal so the color is a shade of gray, right in the center of the cylinder. However, for now let's just set H = 0 if C = 0 because then your implementation will match mine.

Notice that we are going to have H = \[0,1) and it should circle around if it gets too large or goes negative. Thus we check to see if it is negative and add one if it is. This is slightly different than other methods where H is between 0 and 6 or 0 and 360. We will store the H, S, and V components in the same image, so simply replace the R channel with H, the G channel with S, etc. Watch out for divide-by-zero when computing the saturation!

Fill in `void rgb_to_hsv(Image& im)` in `process_image.cpp` to convert an RGB image into an HSV image in-place. Since this transformation is done per-pixel you can just iterate over each pixel and convert them.

## 7. HSV to RGB ##

Okay, now let's do the reverse transformation. You can use the following equations:

```

// Given the H, S, V channels of an image:
C = V * S
X = C * (1 - abs((6*H mod 2) - 1))  // You can use the fmod() function to do a floating point modulo.
m = V - C
```

Note: Make sure that you use floating points! 1/6 with integer division is zero.

<img src="figs/hsv_to_rgb.svg" width="256">


```
(R, G, B) = (R'+m, G'+m, B'+m)
```

Fill in `void hsv_to_rgb(Image& im)` in `process_image.cpp` with this in-place transformation.

Finally, when your done we can mess with some images! In `test.c` we convert an image to HSV, increase the saturation, then convert it back, lines 108-114:

    // 6-7. Colorspace and saturation
    Image im2 = load_image("data/dog.jpg");
    rgb_to_hsv(im2);
    shift_image(im2, 1, .2);
    clamp_image(im2);
    hsv_to_rgb(im2);
    im2.save_image("output/colorspace_result");

![Saturated dog picture](figs/dog_saturated.jpg)

Hey that's exciting! Play around with it a little bit, see what you can make. Note that with the above method we do get some artifacts because we are trying to increase the saturation in areas that have very little color. Instead of shifting the saturation, you could scale the saturation by some value to get smoother results!

## 8. A small amount of extra credit ##

Implement `void scale_image(image im, int c, float v);` to scale a channel by a certain amount. This will give us better saturation results. Note, you will have to add the necessary lines to the header file(s), it should be very similar to what's already there for `shift_image`. Now if we scale saturation by `2` instead of just shifting it all up we get much better results:

    Image im = load_image("data/dog.jpg");
    rgb_to_hsv(im);
    scale_image(im, 1, 2);
    clamp_image(im);
    hsv_to_rgb(im);
    im.save_image("output/dog_scale_saturated");

![Dog saturated smoother](figs/dog_scale_saturated.jpg)

## 9. Super duper extra credit ##

Implement RGB to [Hue, Chroma, Lightness](https://en.wikipedia.org/wiki/CIELUV#Cylindrical_representation_.28CIELCH.29), a perceptually more accurate version of Hue, Saturation, Value. Note, this will involve gamma decompression, converting to CIEXYZ, converting to CIELUV, converting to HCL, and the reverse transformations. The upside is a similar colorspace to HSV but with better perceptual properties!

## Turn it in ##

You only need to turn in ONLY the 2 files: `access_image.cpp` and `process_image.cpp`. Do not change any of the other files and do not change the signature lines of the functions. We will rely on those to be consistent for testing. You are free to define any amount of extra structs/classes, functions, global variables that you wish. Use the canvas link on the class website. Do not submit anything extra.

[1]: https://en.wikipedia.org/wiki/SRGB#The_sRGB_transfer_function_("gamma")
[2]: https://en.wikipedia.org/wiki/Luma_(video)

#### Grading ####
    pixel_address        2
    get_clamped_pixel    1
    set_pixel            3
    copy_image           2
    rgb_to_grayscale     2
    shift_image          3
    clamp_image          2
    rgb_to_hsv           5
    hsv_to_rgb           5
                        25
    
    EXTRA CREDIT:
    
    scale_image                   1   OPTIONAL                         
    rgb_to_luv                    2   OPTIONAL 
    luv_to_rgb                    2   OPTIONAL  
