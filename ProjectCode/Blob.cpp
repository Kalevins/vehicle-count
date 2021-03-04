/**
 * @file Blob.cpp
 * @author Kevin Muñoz Rengifo,
 *         Carlos Sanchez Meneses,
 *         Juan Mateo Albán Méndez,
 *         Jorge Manuel Castillo Camargo
 * @brief Archivo blob
 * @version 1.0
 * @date 2020-03-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Blob.h"

Blob::Blob(std::vector<cv::Point> _contour) {

    currentContour = _contour; // Asigna el parametro al contorno actual

    currentBoundingRect = cv::boundingRect(currentContour); // Crea el rectángulo delimitador actual a partir del contorno actual

    cv::Point currentCenter; // Punto central actual

    currentCenter.x = (currentBoundingRect.x + currentBoundingRect.x + currentBoundingRect.width) / 2; // Calcula la posicion en x del rectangulo que encierra el vehiculo, con la distancia del borde izquierdo de la pantalla al borde izquierdo del rectangulo (currentBoundingRect.x) y el ancho del rectangulo (currentBoundingRect.width)                                                                                                     
    currentCenter.y = (currentBoundingRect.y + currentBoundingRect.y + currentBoundingRect.height) / 2; // Calcula la posicion en y del rectangulo que encierra el vehiculo, con la distancia del borde superior de la pantalla al borde superior del rectangulo (currentBoundingRect.x) y el alto del rectangulo (currentBoundingRect.width)                                                                                                     

    centerPositions.push_back(currentCenter); // Agrega punto central actual al fondo de la matriz.

    dblCurrentDiagonalSize = sqrt(pow(currentBoundingRect.width, 2) + pow(currentBoundingRect.height, 2)); // Calcula el tamaño de la diagonal actual mediante el teorema de Pitágoras

    dblCurrentAspectRatio = (float)currentBoundingRect.width / (float)currentBoundingRect.height; // Calcula la relación de aspecto actual

    blnStillBeingTracked = true; // Se continua con el rastreo actual
    blnCurrentMatchFoundOrNewBlob = true; // Se encuentra coincidencia actual o nuevo Blob

    intNumOfConsecutiveFramesWithoutAMatch = 0; // Se reinicia el número de cuadros consecutivos sin una coincidencia a 0
}

void Blob::predictNextPosition(void) {

    int numPositions = (int)centerPositions.size(); // Identidica el numero de posiciones de la matriz centerPositions

    if (numPositions == 1) { // Si solo hay una posición registrada

        predictedNextPosition.x = centerPositions.back().x; // La siguiente posicion va a ser la ultima registrada
        predictedNextPosition.y = centerPositions.back().y; 

    } else if (numPositions == 2) { // Si hay dos posiciones registradas

        int deltaX = centerPositions[1].x - centerPositions[0].x; // Se calcula la diferencia entre la posicion actual y la pasada
        int deltaY = centerPositions[1].y - centerPositions[0].y;

        predictedNextPosition.x = centerPositions.back().x + deltaX; // La siguiente posicion va a ser la ultima registrada mas la diferencia
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    } else if (numPositions == 3) { // Si hay tres posiciones registradas

        int sumOfXChanges = ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 3.0);

        int sumOfYChanges = ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 3.0);

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    } else if (numPositions == 4) { // Si hay cuatro posiciones registradas

        int sumOfXChanges = ((centerPositions[3].x - centerPositions[2].x) * 3) +
            ((centerPositions[2].x - centerPositions[1].x) * 2) +
            ((centerPositions[1].x - centerPositions[0].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 6.0);

        int sumOfYChanges = ((centerPositions[3].y - centerPositions[2].y) * 3) +
            ((centerPositions[2].y - centerPositions[1].y) * 2) +
            ((centerPositions[1].y - centerPositions[0].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 6.0);

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    } else if (numPositions >= 5) { // Si hay cinco posiciones registradas

        int sumOfXChanges = ((centerPositions[numPositions - 1].x - centerPositions[numPositions - 2].x) * 4) +
            ((centerPositions[numPositions - 2].x - centerPositions[numPositions - 3].x) * 3) +
            ((centerPositions[numPositions - 3].x - centerPositions[numPositions - 4].x) * 2) +
            ((centerPositions[numPositions - 4].x - centerPositions[numPositions - 5].x) * 1);

        int deltaX = (int)std::round((float)sumOfXChanges / 10.0);

        int sumOfYChanges = ((centerPositions[numPositions - 1].y - centerPositions[numPositions - 2].y) * 4) +
            ((centerPositions[numPositions - 2].y - centerPositions[numPositions - 3].y) * 3) +
            ((centerPositions[numPositions - 3].y - centerPositions[numPositions - 4].y) * 2) +
            ((centerPositions[numPositions - 4].y - centerPositions[numPositions - 5].y) * 1);

        int deltaY = (int)std::round((float)sumOfYChanges / 10.0);

        predictedNextPosition.x = centerPositions.back().x + deltaX;
        predictedNextPosition.y = centerPositions.back().y + deltaY;

    } else {
        // Nunca deberia entrar aquí
    }

}

