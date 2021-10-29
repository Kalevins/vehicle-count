/**
 * @file main.cpp
 * @author Kevin Muñoz Rengifo,
 *         Carlos Sanchez Meneses,
 *         Juan Mateo Albán Méndez,
 *         Jorge Manuel Castillo Camargo
 * @brief Archivo principal
 * @version 1.0
 * @date 2020-03-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

//////////Librerías//////////

#include<ctime>
#include<time.h> 
#include<string>
#include<mysql/mysql.h>
#include<stdio.h>
#include<iomanip>
#include<fstream>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>

#include "Blob.cpp"

//////////Variables Globales//////////

/**
 * @brief Constante que tiene el codigo del color negro
 * 
 */
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
/**
 * @brief Constante que tiene el codigo del color blanco
 * 
 */
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
/**
 * @brief Constante que tiene el codigo del color amarillo
 * 
 */
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
/**
 * @brief Constante que tiene el codigo del color verde
 * 
 */
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
/**
 * @brief Constante que tiene el codigo del color rojo
 * 
 */
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);
/**
 * @brief Contiene el registro de los vehiculos
 * 
 */
int contadorCarros=0;
/**
 * @brief Controla el intevalo de tiempo
 * 
 */
int tiempo0=0; 
/**
 * @brief Controla el intevalo de tiempo
 * 
 */
int tiempo1=0;
/**
 * @brief Controla si se quiere mostrar el numero del vehiculo en la imagen
 * 
 */
bool mostrarNumeroEnVehiculos=false;
/**
 * @brief Guardar la fecha en la que se toman los datos
 * 
 */
char fecha[80];
/**
 * @brief Tabla para la base de datos
 * 
 */
std::string tablaCounting;
/**
 * @brief Datos a enviar a la base de datos
 * 
 */
std::string datoEnviar;
/**
 * @brief ID de la camara
 * 
 */
std::string camara;

//////////Funciones//////////

/**
 * @brief Funcion para obtener el tiempo actual
 * 
 * @return int con el tiempo
 */
int tiempo(){
    std::time_t t = std::time(0); // Toma el tiempo actual de la BIOS
    return t; // Lo retorna
}
/**
 * @brief Agrega un nuevo Blob
 * 
 * @param currentFrameBlob Blob del fotograma actual
 * @param existingBlobs Blobs existentes
 */
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true; // Se encuentra un nuevo Blob
    existingBlobs.push_back(currentFrameBlob); // Agrega el Blob del fotograma actual al fondo de la matriz existingBlobs.
}
/**
 * @brief Calcula la distancia entre dos puntos
 * 
 * @param point1 Punto 1 a medir
 * @param point2 Punto 2 a medir
 * @return double 
 */
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {
    int intX = abs(point1.x - point2.x); // Valor absoluto de la distancia en x de los dos puntos
    int intY = abs(point1.y - point2.y); // Valor absoluto de la distancia en y de los dos puntos
    return(sqrt(pow(intX, 2) + pow(intY, 2))); // Calcula la distancia con Pitágoras
}
/**
 * @brief Agrega el Blob a los Blobs existentes
 * 
 * @param currentFrameBlob Blob del fotograma actual
 * @param existingBlobs Blobs existentes
 * @param intIndex Indice
 */
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
    existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour; // Inicializa el Blob
    existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;
    existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());
    existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
    existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;
    existingBlobs[intIndex].blnStillBeingTracked = true;
    existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}
/**
 * @brief Coincidencia de los Blobs del fotograma actual con los Blobs existentes
 * 
 * @param existingBlobs Blobs existentes
 * @param currentFrameBlobs Blob del fotograma actual
 */
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {
    for (auto &existingBlob : existingBlobs) { // Recorre todos los Blobs existentes
        existingBlob.blnCurrentMatchFoundOrNewBlob = false; // Se indica que no es un Blob nuevo
        existingBlob.predictNextPosition(); // Se hace la prediccion de la siguinte posición
    }
    for (auto &currentFrameBlob : currentFrameBlobs) { // Recorre todos Blob de los fotogramas actuales
        int intIndexOfLeastDistance = 0; // Se inicializa el índice de menor distancia en cero
        double dblLeastDistance = 100000.0; // Se inicializa la menor distancia en 100000
        for (unsigned int i = 0; i < existingBlobs.size(); i++) { // Recorrido con la cantidad de Blobs existentes
            if (existingBlobs[i].blnStillBeingTracked == true) {  // Si el Blob está actualmente en rastreo
                double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition); // Se calcula distancia entre la ultima posicion del Blob del fotograma y de la posicion predecida del Blob del arreglo
                if (dblDistance < dblLeastDistance) { // Si la distancia es menor a la establecida
                    dblLeastDistance = dblDistance; // Se reemplaza por el valor de la distancia
                    intIndexOfLeastDistance = i; // El índice de menor distancia toa el valor del Blob
                }
            }
        }
        if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) { // Si la menor distancia es menor a la mitad de la diagonal del Blob del fotograma actual
            addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance); // Se agrega el Blob a los Blobs existentes
        }
        else { // Si no
            addNewBlob(currentFrameBlob, existingBlobs); // Se crea un nuevo Blob
        }
    }
    for (auto &existingBlob : existingBlobs) { // Se recorren todos los blob existentes
        if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) { // Si el Blob no es nuevo
            existingBlob.intNumOfConsecutiveFramesWithoutAMatch++; // El número de fotogramas consecutivos sin una coincidencia aumenta
        }
        if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) { // Si este número de fotogramas consecutivos sin una coincidencia es mayor o igual a 5
            existingBlob.blnStillBeingTracked = false; // Se desactva el rastreo actual
        }
    }
}
/**
 * @brief Dibuja y muestra los contornos
 * 
 * @param imageSize Tamaño de la imagen
 * @param contours Contornos
 * @param strImageName Nombre de la imagen
 */
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK); // Transforma la imagen inicial a negro
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1); // Dibuja los contornos blancos de la imgen negra
    //cv::imshow(strImageName, image); // Muestra la imagen
}
/**
 * @brief Dibuja y muestra los contornos del Blob
 * 
 * @param imageSize Tamaño de la imagen
 * @param blobs (Vehiculos)
 * @param strImageName Nombre de la imagen
 */
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {
    cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK); // Transforma la imagen inicial a negro
    std::vector<std::vector<cv::Point> > contours; // Vector con los puntos del contorno
    for (auto &blob : blobs) { // Recorre cada Blob
        if (blob.blnStillBeingTracked == true) { // Si el Blob está actualmente en rastreo
            contours.push_back(blob.currentContour); // Agrega el contorno actual al fondo de la matriz de contornos.
        }
    }
    cv::drawContours(image, contours, -1, SCALAR_WHITE, -1); // Dibuja los contornos blancos de la imgen negra
    //cv::imshow(strImageName, image); // Muestra la imagen
}
/**
 * @brief Verificar si los Blobs cruzaron la línea
 * 
 * @param blobs (Vehiculos)
 * @param intHorizontalLinePosition Posición de la línea horizontal
 * @param carCount Contador de vehiculos
 * @return true 
 * @return false 
 */
bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount) {
    bool blnAtLeastOneBlobCrossedTheLine = false; // Variable para controlar si al menos un Blob cruzó la línea
    for (auto blob : blobs) { // Recorre cada Blob
        if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) { // Si el Blob está actualmente en rastreo y tiene mas de una posiciones central registrada
            int prevFrameIndex = (int)blob.centerPositions.size() - 2; // El indice del fotograma previo es el dos veces anterior al ultimo
            int currFrameIndex = (int)blob.centerPositions.size() - 1; // El indice del fotograma actual es el anterior al ultimo
            if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition) { // Si la posición central en y del Blob previo es mayor a la linea y a su vez la posición central en y del Blob actual es menor o igual al de la linea (Conteo de vehiculos de arriba hacia abajo)
                carCount++; // Se incrementa el conteo de los vehiculos
                blnAtLeastOneBlobCrossedTheLine = true; // El Blob cruzo la linea
                contadorCarros=carCount; // Se iguala la variable para la base de datos
            }
            if (blob.centerPositions[prevFrameIndex].y < intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y >= intHorizontalLinePosition) { // Si la posición central en y del Blob previo es menor a la linea y a su vez la posición central en y del Blob actual es mayor o igual al de la linea (Conteo de vehiculos de abajo hacia arriba)
                carCount++; // Se incrementa el conteo de los vehiculos
                blnAtLeastOneBlobCrossedTheLine = true; // El Blob cruzo la linea
                contadorCarros=carCount; // Se iguala la variable para la base de datos
            }
        }
    }
    return blnAtLeastOneBlobCrossedTheLine; // Retorna si un Blob a cruzado la linea
}
/**
 * @brief Dibuja la información del blob en la imagen
 * 
 * @param blobs (Vehiculos)
 * @param imgFrame2Copy Copia del segundo fotograma
 */
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {
    for (unsigned int i = 0; i < blobs.size(); i++) { // Recoore todos lo Blob
        if (blobs[i].blnStillBeingTracked == true) { // Si el Blob está actualmente en rastreo
            cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_YELLOW, 2); // Dibuja un rectangulo rojo en cada Blob
            if (mostrarNumeroEnVehiculos == true) {
                int intFontFace = CV_FONT_HERSHEY_SIMPLEX; // Se selecciona la fuente de los numeros a mostrar
                double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0; // Se reduce la escala a 60 veces menor la diagonal del Blob
                int intFontThickness = (int)std::round(dblFontScale * 1.0); // Se ajusta el grosor de la fuente a la escala reducida
                cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness); // Se dibuja en pantalla
            }
        }
    }
}
/**
 * @brief Dibuja el conteo de vehiculos en la imagen
 * 
 * @param carCount Contador de vehiculos
 * @param imgFrame2Copy Copia del segundo fotograma
 */
void drawCarCountOnImage(int &carCount, cv::Mat &imgFrame2Copy) {
    int intFontFace = CV_FONT_HERSHEY_SIMPLEX; // Se selecciona la fuente de los numeros a mostrar
    double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 200000.0; // Se ajusta la escala del indicador de conteo
    int intFontThickness = (int)std::round(dblFontScale * 1.5); // Se ajusta el grosor de la fuente a la escala reducida
    cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0); // Calcula el ancho y el alto de una cadena de texto.
    cv::Point ptTextBottomLeftPosition; // Se crea el punto de la posición inferior izquierda del texto
    ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25); // Se calcula la posicion en x
    ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25); // Se calcula la posicion en y
    cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness); // Se dibuja en pantalla
}

//////////Main//////////

/**
 * @brief Funcion principal a ejecutar
 *
 */
int main(void) {
    
    MYSQL* conn; //Se crea la variable conn para realizar la conexion con la base de datos
   
    cv::VideoCapture capVideo(2); // Guarda el video en una variable por el puerto que se escoja como parametro

    cv::Mat imgFrame1; // Matriz con el fotograma 1
    cv::Mat imgFrame2; // Matriz con el fotograma 2

    std::vector<Blob> blobs; // Vector con lo Blobs

    cv::Point crossingLine[2]; // Vector de puntos de cruze

    int carCount = 0; // Se inicializa el contador de vehiculos
    
    capVideo.read(imgFrame1); // El fotograma 1 toma la captura de la imagen actual
    capVideo.read(imgFrame2); // El fotograma 2 toma la captura de la imagen actual

    int intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.35); // Ubicacion de la linea horizontal en un 0.35 la resolucion en 'y' para el conteo

    crossingLine[0].x = 0; // Cero en 'x' para el inicio
    crossingLine[0].y = intHorizontalLinePosition; // Posicion de la linea en 'y' para el inicio

    crossingLine[1].x = imgFrame1.cols - 1; // Resolucion en 'x' para el final
    crossingLine[1].y = intHorizontalLinePosition; // Posicion de la linea en 'y' para el final

    char chCheckForEscKey = 0; // Variable para terminar el proceso

    bool blnFirstFrame = true; // Se indica que es el primer frame capturado

    tiempo0=tiempo(); // Se obtiene el tiempo justo antes de empezar el while para hacer empezar a correr la ventana de tiempo de conteo de carros
    camara="1"; // Id de la camara

    while (capVideo.isOpened() && chCheckForEscKey != 27) { // Ciclo infinito mientras se este capturando video y no se halla presionado la tecla de salida
        
        tiempo1=tiempo(); // Se obtiene el tiempo que lleva ejecutandose el while para realizar el calculo de la ventana de tiempo

        if(tiempo1-tiempo0>600){ // tiempo1-tiempo0 corresponde a la ventana de tiempo en segundos

            conn=mysql_init(0); // Inicia MySQL

            conn=mysql_real_connect(conn,"54.38.44.26","jblucwvu_admin","query20580","jblucwvu_counting",3306,NULL,0); // Se presentan las credenciales de la base de datos donde se realizara la conexion los parametros son(variable MYSQL, direccion ip del host, el usuario, la contrasenia, el nombre de la base de datos)

            if(conn!=0){ // Verificacion, sí se ha podido realizar la conexion con la base de datos

                tiempo0=tiempo1; // Desde este instante se debe iniciar la nueva ventana de tiempo

                std::cout << "Conectado\n"; // Imprime en consola
            
                time_t fechaString = tiempo1; // Variable auxiliar para guardar la fecha en string

                struct tm *tm= localtime(&fechaString); // Guarda la fecha con el formato de una fecha legible
                strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm); // El UNIX time se transforma a fecha legible del tipo string para subirlo a la base de datos

                std::string fecha2(fecha);// Se convieerte de fecha a string

                std::cout << tiempo1 << " Segundos desde el 01-Enero-1970\n"; // Imprime en consola los segundos desde 1970
                datoEnviar= std::to_string(carCount); // Se transforma a string la cantidad de vehiculos contados
                std::cout << "Vehiculos contados: "<<datoEnviar << " \n"; // Imprime en consola los vehiculos contados
            
                // Para insertar datos en la tabla de datos llamada counting con columnas cont correspondiente a la cantidad de carros contados, la fecha en que se realizó ese conteo y el id de la camara
                tablaCounting="INSERT INTO counting (cont,fecha,camara) VALUES ('"+datoEnviar+"','"+fecha2+"','"+camara+"')";

                std::cout<<tablaCounting; // Se imprime en consola la sentencia SQL, para hacer control
                mysql_query(conn, tablaCounting.c_str()); // Se envian los datos a la base de datos
                std::cout<<tablaCounting.c_str(); // Se imprime en consola
                carCount=0;// Se reinicia el contador de carros
                mysql_close(conn);// Se cierra la conexion con la base de datos

            }else{ // No se ha podido establecer conexion con la base de datos
                std::cout << "No se ha conectado a las base de datos\n"; // Imprime en consola
            }
        }
        
        std::vector<Blob> currentFrameBlobs; // Vector con los Blobs de cada fotograma actual

        cv::Mat imgFrame1Copy = imgFrame1.clone(); // Se hace una copia del fotograma 1
        cv::Mat imgFrame2Copy = imgFrame2.clone(); // Se hace una copia del fotograma 2

        cv::Mat imgDifference; // Guarda la diferencia de imagenes
        cv::Mat imgThresh; // Guarda el desgranado de imagen

        cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY); // Transforma el fotograma 1 a escala de grises
        cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY); // Transforma el fotograma 2 a escala de grises

        cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0); // Suaviza los bordes del fotograma 1
        cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0); // Suaviza los bordes del fotograma 2

        cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference); // Guarda la diferencia de imagenes

        cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY); // Se aplica un umbral eliminar el ruido de la imagen

        cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)); // Se guarda una estructura de tamaño y forma especificos
        
        for (unsigned int i = 0; i < 2; i++) {
            cv::dilate(imgThresh, imgThresh, structuringElement5x5); // Dilata la imagen
            cv::dilate(imgThresh, imgThresh, structuringElement5x5); // Dilata la imagen
            cv::erode(imgThresh, imgThresh, structuringElement5x5); // Erosiona la imagen
        }

        cv::Mat imgThreshCopy = imgThresh.clone(); // Se hace una copia del imgThresh

        std::vector<std::vector<cv::Point> > contours; // Vector con los puntos del contorno

        cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // Encuentra los contornos de la imagen

        drawAndShowContours(imgThresh.size(), contours, "imgContours"); // Llama la funcion drawAndShowContours

        std::vector<std::vector<cv::Point> > convexHulls(contours.size()); // Crea un vector del mismo tamaño de los contornos para guardar los convexHulls

        for (unsigned int i = 0; i < contours.size(); i++) { // For para recorrer los contornos y los convexHulls
            cv::convexHull(contours[i], convexHulls[i]); // Encuentra el convexHulls de el conjunto de puntos.
        }

        drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls"); // Llama la funcion drawAndShowContours

        for (auto &convexHull : convexHulls) { // Recorre todos los convexHulls
            Blob possibleBlob(convexHull); // Variable de control para los posibles Blob

            if (possibleBlob.currentBoundingRect.area() > 400 && // Si cumple con ciertos requisitos para ser un Blob
                possibleBlob.dblCurrentAspectRatio > 0.2 &&
                possibleBlob.dblCurrentAspectRatio < 4.0 &&
                possibleBlob.currentBoundingRect.width > 30 &&
                possibleBlob.currentBoundingRect.height > 30 &&
                possibleBlob.dblCurrentDiagonalSize > 60.0 &&
                (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
                currentFrameBlobs.push_back(possibleBlob); // Se agrega al fondo de la matriz de Blobs del fotograma actual
            }
        }

        drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs"); // Llama la funcion drawAndShowContours

        if (blnFirstFrame == true) { // Si es el primer fotograma que se captura
            for (auto &currentFrameBlob : currentFrameBlobs) { // Recorre todos los Blobs de los fotogramas actuales
                blobs.push_back(currentFrameBlob); // Se agrega al fondo de la matriz de Blobs
            }
        } else {
            matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs); // Llama la funcion matchCurrentFrameBlobsToExistingBlobs
        }

        drawAndShowContours(imgThresh.size(), blobs, "imgBlobs"); // Llama la funcion drawAndShowContours

        imgFrame2Copy = imgFrame2.clone(); // Se obtiene otra copia del fotograma 2 ya se cambiamo la copia del fotograma 2 que se tenia anteriormente en el procesamiento anterior

        drawBlobInfoOnImage(blobs, imgFrame2Copy); // Llama la funcion drawBlobInfoOnImage

        bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount); // Se guarda el valor que retorna la funcion checkIfBlobsCrossedTheLine

        if (blnAtLeastOneBlobCrossedTheLine == true) { // Si al menos un Blob cruzó la línea
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2); // La linea se vuelve verde
        } else {
            cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2); // La linea se vuelve roja
        }
 
        drawCarCountOnImage(carCount, imgFrame2Copy); // Llama la funcion drawCarCountOnImage

        cv::imshow("imgFrame2Copy", imgFrame2Copy); // Despliega la imagen

        // En este punto se prepara la nueva interacción

        currentFrameBlobs.clear(); // Se borra la matriz con los Blobs de los fotogramas actuales

        imgFrame1 = imgFrame2.clone(); // Se mueve el fotograma 1 al fotograma 2

        capVideo.read(imgFrame2); // El fotograma 2 toma la captura de la imagen actual

        blnFirstFrame = false; // Se cambia la variable de control del primer fotograma
        chCheckForEscKey = cv::waitKey(1); // Captura la tecla que se presione
    }

    if (chCheckForEscKey != 27) { // Si se presionó esc
        std::cout << "Fin del proceso\n"; // Imprime en consola
        cv::waitKey(0); // Mantenga abiertas las ventanas para permitir que se muestre el mensaje "Fin del proceso"
    }

    return(0);
}
